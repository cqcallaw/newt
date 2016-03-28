/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <basic_variable.h>
#include <sstream>
#include <defaults.h>
#include <execution_context.h>
#include <assignment_statement.h>
#include <function_declaration.h>
#include <function.h>
#include <sum.h>
#include <sum_type.h>
#include <record_type.h>
#include <record.h>
#include <nested_type_specifier.h>

#include "assert.h"
#include "expression.h"

BasicVariable::BasicVariable(const_shared_ptr<string> name,
		const yy::location location) :
		BasicVariable(name, location, NamespaceQualifierList::GetTerminator()) {
}

BasicVariable::BasicVariable(const_shared_ptr<string> name,
		const yy::location location, const NamespaceQualifierListRef space) :
		Variable(name, location), m_space(space) {
}

BasicVariable::~BasicVariable() {
}

const_shared_ptr<string> BasicVariable::ToString(
		const shared_ptr<ExecutionContext> context) const {
	ostringstream buffer;
	buffer << *GetName();
	return make_shared<string>(buffer.str());
}

const_shared_ptr<TypeSpecifier> BasicVariable::GetType(
		const shared_ptr<ExecutionContext> context,
		AliasResolution resolution) const {
	auto symbol = context->GetSymbol(GetName(), DEEP);
	return symbol->GetType();
}

const_shared_ptr<Result> BasicVariable::Evaluate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<Symbol> symbol = context->GetSymbol(GetName(), DEEP);
	auto result_symbol = Symbol::GetDefaultSymbol();

	if (symbol && symbol != Symbol::GetDefaultSymbol()) {
		result_symbol = symbol;
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().begin.line, GetLocation().begin.column,
						*(GetName())), errors);
	}

	const_shared_ptr<Result> result = make_shared<Result>(
			result_symbol->GetValue(), errors);
	return result;
}

const ErrorListRef BasicVariable::AssignValue(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Expression> expression,
		const AssignmentType op) const {
	return AssignValue(context, expression, context, op);
}

const ErrorListRef BasicVariable::AssignValue(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Expression> expression,
		const shared_ptr<ExecutionContext> output_context,
		const AssignmentType op) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto variable_name = GetName();
	const int variable_line = GetLocation().begin.line;
	const int variable_column = GetLocation().begin.column;

	const_shared_ptr<Symbol> symbol = output_context->GetSymbol(variable_name,
			DEEP);
	const_shared_ptr<TypeSpecifier> symbol_type = symbol->GetType();
	auto symbol_value = symbol->GetValue();

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			dynamic_pointer_cast<const PrimitiveTypeSpecifier>(symbol_type);
	if (as_primitive) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			const_shared_ptr<Result> result = AssignmentStatement::do_op(
					variable_name, basic_type, variable_line, variable_column,
					*(static_pointer_cast<const int>(symbol_value)), expression,
					op, context);

			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				errors = SetSymbol(output_context, result->GetData<bool>());
			}
			break;
		}
		case INT: {
			const_shared_ptr<Result> result = AssignmentStatement::do_op(
					variable_name, basic_type, variable_line, variable_column,
					*(static_pointer_cast<const int>(symbol_value)), expression,
					op, context);

			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				errors = SetSymbol(output_context, result->GetData<int>());
			}
			break;
		}
		case DOUBLE: {
			const_shared_ptr<Result> result = AssignmentStatement::do_op(
					variable_name, basic_type, variable_line, variable_column,
					*(static_pointer_cast<const double>(symbol_value)),
					expression, op, context);

			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				errors = SetSymbol(output_context, result->GetData<double>());
			}
			break;
		}
		case STRING: {
			const_shared_ptr<Result> result = AssignmentStatement::do_op(
					variable_name, basic_type, variable_line, variable_column,
					static_pointer_cast<const string>(symbol_value), expression,
					op, context);
			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				errors = SetSymbol(output_context, result->GetData<string>());
			}
			break;
		}
		default:
			assert(false);
		}
	}

	//TODO: don't allow += or -= operations on array specifiers
	const_shared_ptr<ArrayTypeSpecifier> as_array = std::dynamic_pointer_cast<
			const ArrayTypeSpecifier>(symbol_type);
	if (as_array) {
		//re-assigning an array reference
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto result_as_array = expression_evaluation->GetData<Array>();

			if (result_as_array) {
				errors = SetSymbol(output_context, result_as_array);
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::ASSIGNMENT_TYPE_ERROR, variable_line,
								variable_column, as_array->ToString(),
								expression->GetType(context)->ToString()),
						errors);
			}
		}
	}

//TODO: don't allow += or -= operations on compound type specifiers
	const_shared_ptr<RecordTypeSpecifier> as_record = std::dynamic_pointer_cast<
			const RecordTypeSpecifier>(symbol_type);
	if (as_record) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto new_instance = expression_evaluation->GetData<Record>();

			//we're assigning a struct reference
			errors = SetSymbol(output_context, new_instance);
		}
	}

//TODO: don't allow += or -= operations on function type specifiers
	const_shared_ptr<FunctionTypeSpecifier> as_function =
			std::dynamic_pointer_cast<const FunctionTypeSpecifier>(symbol_type);
	if (as_function) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto function = expression_evaluation->GetData<Function>();

			errors = SetSymbol(output_context, function);
		}
	}

	const_shared_ptr<SumTypeSpecifier> as_sum_specifier =
			std::dynamic_pointer_cast<const SumTypeSpecifier>(symbol_type);
	if (as_sum_specifier) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto sum = static_pointer_cast<const Sum>(symbol->GetValue());
			auto expression_type = expression->GetType(context);

			plain_shared_ptr<Sum> new_sum;
			if (expression_type->IsAssignableTo(symbol_type,
					context->GetTypeTable())) {
				//we're re-assigning
				new_sum = expression_evaluation->GetData<Sum>();
			} else {
				//test for widening
				auto sum_type_name = as_sum_specifier->GetTypeName();
				auto sum_type = context->GetTypeTable()->GetType<SumType>(
						sum_type_name);

				auto widening_analysis = sum_type->AnalyzeWidening(
						*expression_type, *sum_type_name);

				if (widening_analysis == UNAMBIGUOUS) {
					//we're widening
					auto tag = sum_type->MapSpecifierToVariant(*expression_type,
							*sum_type_name);
					new_sum = make_shared<Sum>(as_sum_specifier, tag,
							expression_evaluation->GetRawData());
				} else {
					assert(false);
				}
			}

			errors = SetSymbol(output_context, new_sum);
		}
	}

	const_shared_ptr<NestedTypeSpecifier> as_nested_specifier =
			std::dynamic_pointer_cast<const NestedTypeSpecifier>(symbol_type);
	if (as_nested_specifier) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto parent_type_definition = context->GetTypeTable()->GetType<
					ComplexType>(as_nested_specifier->GetParent());

			if (parent_type_definition) {
				auto as_sum = dynamic_pointer_cast<const SumType>(
						parent_type_definition);

				if (as_sum) {
					auto child_record_type = as_sum->GetDefinition()->GetType<
							RecordType>(as_nested_specifier->GetMemberName());
					if (child_record_type) {
						auto value = expression_evaluation->GetData<Record>();
						//auto expression_type = expression->GetType(context);
						errors = SetSymbol(output_context, value,
								as_nested_specifier->GetParent());
					} else {
						assert(false);
					}
				} else {
					assert(false);
				}
			} else {
				assert(false);
			}
		}
	}

	return errors;
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<bool> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetType(), PrimitiveTypeSpecifier::GetBoolean());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<int> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetType(), PrimitiveTypeSpecifier::GetInt());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<double> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetType(), PrimitiveTypeSpecifier::GetDouble());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<string> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetType(), PrimitiveTypeSpecifier::GetString());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetType(), value->GetTypeSpecifier());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Record> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable(),
					container), symbol->GetType(), value->GetTypeSpecifier());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Function> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetType(), value->GetType());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Sum> sum) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), sum, context->GetTypeTable()),
			symbol->GetType(), sum->GetType());
}

const_shared_ptr<Variable> BasicVariable::GetDefaultVariable() {
	static const_shared_ptr<string> name = const_shared_ptr<string>(
			new string("!!!!!DefaultVariable!!!!!"));
	static const const_shared_ptr<Variable> instance =
			make_shared<BasicVariable>(name, GetDefaultLocation(),
					NamespaceQualifierList::GetTerminator());

	return instance;
}

const ErrorListRef BasicVariable::Validate(
		const shared_ptr<ExecutionContext> context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto symbol = context->GetSymbol(GetName(), DEEP);

	if (symbol == nullptr || symbol == Symbol::GetDefaultSymbol()) {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().begin.line, GetLocation().begin.column,
						*(GetName())), errors);
	}

	return errors;
}

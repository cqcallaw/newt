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
#include <primitive_type.h>
#include <array_type.h>
#include <function_type.h>
#include <maybe_type_specifier.h>
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

const_shared_ptr<TypeSpecifier> BasicVariable::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> context,
		AliasResolution resolution) const {
	auto symbol = context->GetSymbol(GetName(), DEEP);
	if (symbol) {
		return symbol->GetTypeSpecifier();
	}
	return PrimitiveTypeSpecifier::GetNone();
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
			result_symbol ? result_symbol->GetValue() : nullptr, errors);
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
	const_shared_ptr<TypeSpecifier> symbol_type_specifier =
			symbol->GetTypeSpecifier();
	auto symbol_type = symbol_type_specifier->GetType(context->GetTypeTable(),
			RESOLVE);
	auto symbol_value = symbol->GetValue();

	const_shared_ptr<PrimitiveType> as_primitive = dynamic_pointer_cast<
			const PrimitiveType>(symbol_type);
	if (as_primitive) {
		const BasicType basic_type = as_primitive->GetType();
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
	const_shared_ptr<ArrayType> as_array = std::dynamic_pointer_cast<
			const ArrayType>(symbol_type);
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
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										variable_line, variable_column,
										symbol_type_specifier->ToString(),
										expression->GetTypeSpecifier(context)->ToString()),
								errors);
			}
		}
	}

//TODO: don't allow += or -= operations on compound type specifiers
	const_shared_ptr<RecordType> as_record = std::dynamic_pointer_cast<
			const RecordType>(symbol_type);
	if (as_record) {
		auto complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
				symbol_type_specifier);
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto new_instance = expression_evaluation->GetData<Record>();

			//we're assigning a struct reference
			errors = SetSymbol(output_context, complex, new_instance);
		}
	}

//TODO: don't allow += or -= operations on function type specifiers
	const_shared_ptr<FunctionType> as_function = std::dynamic_pointer_cast<
			const FunctionType>(symbol_type);
	if (as_function) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto function = expression_evaluation->GetData<Function>();

			errors = SetSymbol(output_context, function);
		}
	}

	const_shared_ptr<SumType> as_sum = std::dynamic_pointer_cast<const SumType>(
			symbol_type);
	if (as_sum) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);
		errors = expression_evaluation->GetErrors();

		if (ErrorList::IsTerminator(errors)) {
			auto sum = static_pointer_cast<const Sum>(symbol->GetValue());
			auto expression_type_specifier = expression->GetTypeSpecifier(
					context);

			if (expression_type_specifier->IsAssignableTo(symbol_type_specifier,
					context->GetTypeTable())) {
				auto expression_type = expression_type_specifier->GetType(
						context->GetTypeTable(), RESOLVE);
				auto expression_type_as_sum =
						dynamic_pointer_cast<const SumType>(expression_type);

				plain_shared_ptr<Sum> new_sum = nullptr;
				auto as_complex = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(symbol_type_specifier);
				if (as_complex) {
					if (expression_type_as_sum) {
						new_sum = expression_evaluation->GetData<Sum>();
					} else {
						//we're widening
						auto tag = as_sum->MapSpecifierToVariant(*as_complex,
								*expression_type_specifier);
						new_sum = make_shared<Sum>(tag,
								expression_evaluation->GetRawData());
					}
					errors = SetSymbol(output_context, as_complex, new_sum);
				}

				auto as_maybe = dynamic_pointer_cast<const MaybeTypeSpecifier>(
						symbol_type_specifier);
				if (as_maybe) {
					if (expression_type_as_sum) {
						//we're _likely_ doing a direct assignment
						//TODO: we have to consider sums that should be widened, not directly assigned
						new_sum = expression_evaluation->GetData<Sum>();
					} else {
						//we're widening
						auto base_type_specifier = as_maybe->GetTypeSpecifier();
						auto base_type = base_type_specifier->GetType(
								context->GetTypeTable(), RESOLVE);

						plain_shared_ptr<void> data =
								expression_evaluation->GetRawData();
						auto base_as_sum = dynamic_pointer_cast<const SumType>(
								base_type);
						if (base_as_sum) {
							auto sum_type_specifier = static_pointer_cast<
									const ComplexTypeSpecifier>(
									base_type_specifier);
							auto base_tag = base_as_sum->MapSpecifierToVariant(
									*sum_type_specifier,
									*expression_type_specifier);
							data = make_shared<Sum>(base_tag, data);
						}

						auto tag = as_maybe->MapSpecifierToVariant(
								*expression_type_specifier,
								context->GetTypeTable());

						new_sum = make_shared<Sum>(tag, data);
					}
					errors = SetSymbol(output_context, as_maybe, new_sum);
				}
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
			symbol->GetTypeSpecifier(), PrimitiveTypeSpecifier::GetBoolean());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<int> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetTypeSpecifier(), PrimitiveTypeSpecifier::GetInt());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<double> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetTypeSpecifier(), PrimitiveTypeSpecifier::GetDouble());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<string> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetTypeSpecifier(), PrimitiveTypeSpecifier::GetString());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetTypeSpecifier(), value->GetTypeSpecifier());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Record> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), type, value,
					context->GetTypeTable()), symbol->GetTypeSpecifier(), type);
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<MaybeTypeSpecifier> type,
		const_shared_ptr<Sum> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), type, value,
					context->GetTypeTable()), symbol->GetTypeSpecifier(), type);
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Function> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), value, context->GetTypeTable()),
			symbol->GetTypeSpecifier(), value->GetType());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Sum> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(
			context->SetSymbol(*GetName(), type, value,
					context->GetTypeTable()), symbol->GetTypeSpecifier(), type);
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

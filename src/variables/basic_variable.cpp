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

#include "assert.h"
#include "expression.h"

BasicVariable::BasicVariable(const_shared_ptr<string> name,
		const yy::location location) :
		Variable(name, location) {
}

BasicVariable::~BasicVariable() {
}

const string* BasicVariable::ToString(
		const shared_ptr<ExecutionContext> context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << ">";
	return new string(buffer.str());
}

const_shared_ptr<TypeSpecifier> BasicVariable::GetType(
		const shared_ptr<ExecutionContext> context) const {
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
				errors = SetSymbol(output_context,
						static_pointer_cast<const bool>(result->GetData()));
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
				errors = SetSymbol(output_context,
						static_pointer_cast<const int>(result->GetData()));
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
				errors = SetSymbol(output_context,
						static_pointer_cast<const double>(result->GetData()));
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
				errors = SetSymbol(output_context,
						static_pointer_cast<const string>(result->GetData()));
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
			auto result_as_array = static_pointer_cast<const Array>(
					expression_evaluation->GetData());

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
	const_shared_ptr<CompoundTypeSpecifier> as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(symbol_type);
	if (as_compound) {
		const_shared_ptr<Result> expression_evaluation = expression->Evaluate(
				context);

		errors = expression_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto new_instance = static_pointer_cast<const CompoundTypeInstance>(
					expression_evaluation->GetData());

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
			auto function = static_pointer_cast<const Function>(
					expression_evaluation->GetData());

			errors = SetSymbol(output_context, function);
		}
	}

	return errors;
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<bool> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetBoolean());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<int> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetInt());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<double> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetDouble());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<string> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetString());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Array> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetTypeSpecifier());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<CompoundTypeInstance> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetTypeSpecifier());
}

const ErrorListRef BasicVariable::SetSymbol(
		const shared_ptr<ExecutionContext> context,
		const_shared_ptr<Function> value) const {
	auto symbol = context->GetSymbol(*GetName(), DEEP);
	return ToErrorListRef(context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetType());
}

const_shared_ptr<Variable> BasicVariable::GetDefaultVariable() {
	static const_shared_ptr<string> name = const_shared_ptr<string>(
			new string("!!!!!DefaultVariable!!!!!"));
	static const const_shared_ptr<Variable> instance =
			make_shared<BasicVariable>(name, GetDefaultLocation());

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

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

const string* BasicVariable::ToString(const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << ">";
	return new string(buffer.str());
}

const_shared_ptr<TypeSpecifier> BasicVariable::GetType(
		const ExecutionContext* context) const {
	auto symbol = context->GetSymbolContext()->GetSymbol(GetName(), DEEP);
	return symbol->GetType();
}

const Result* BasicVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName(), DEEP);
	const Symbol* result_symbol = Symbol::GetDefaultSymbol();

	if (symbol != nullptr && symbol != Symbol::GetDefaultSymbol()) {
		result_symbol = symbol;
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().begin.line, GetLocation().begin.column,
						*(GetName())));
	}

	const Result* result = new Result(result_symbol->GetValue(), errors);
	return result;
}

const LinkedList<const Error*>* BasicVariable::AssignValue(
		const ExecutionContext* context, const Expression* expression,
		const AssignmentType op) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	auto variable_name = GetName();
	const int variable_line = GetLocation().begin.line;
	const int variable_column = GetLocation().begin.column;

	SymbolTable* symbol_table = (SymbolTable*) context->GetSymbolContext();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name, DEEP);
	const_shared_ptr<TypeSpecifier> symbol_type = symbol->GetType();
	auto symbol_value = symbol->GetValue();

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			dynamic_pointer_cast<const PrimitiveTypeSpecifier>(symbol_type);
	if (as_primitive != nullptr) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					*(static_pointer_cast<const int>(symbol_value)), expression,
					op, context);

			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::GetTerminator()) {
				errors = SetSymbol(context,
						static_pointer_cast<const bool>(result->GetData()));
			}
			break;
		}
		case INT: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					*(static_pointer_cast<const int>(symbol_value)), expression,
					op, context);

			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::GetTerminator()) {
				errors = SetSymbol(context,
						static_pointer_cast<const int>(result->GetData()));
			}
			break;
		}
		case DOUBLE: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					*(static_pointer_cast<const double>(symbol_value)),
					expression, op, context);

			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::GetTerminator()) {
				errors = SetSymbol(context,
						static_pointer_cast<const double>(result->GetData()));
			}
			break;
		}
		case STRING: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					static_pointer_cast<const string>(symbol_value).get(),
					expression, op, context);
			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::GetTerminator()) {
				errors = SetSymbol(context,
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
		const Result* expression_evaluation = expression->Evaluate(context);

		errors = expression_evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::GetTerminator()) {
			auto result_as_array = static_pointer_cast<const Array>(
					expression_evaluation->GetData());

			if (result_as_array) {
				errors = SetSymbol(context, result_as_array);
			} else {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
								variable_line, variable_column,
								as_array->ToString(),
								expression->GetType(context)->ToString()));
			}
		}
	}

	//TODO: don't allow += or -= operations on compound type specifiers
	const_shared_ptr<CompoundTypeSpecifier> as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(symbol_type);
	if (as_compound != nullptr) {
		const Result* expression_evaluation = expression->Evaluate(context);

		errors = expression_evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::GetTerminator()) {
			auto new_instance = static_pointer_cast<const CompoundTypeInstance>(
					expression_evaluation->GetData());

			//we're assigning a struct reference
			errors = SetSymbol(context, new_instance);
		}
	}

	//TODO: don't allow += or -= operations on function type specifiers
	const_shared_ptr<FunctionTypeSpecifier> as_function =
			std::dynamic_pointer_cast<const FunctionTypeSpecifier>(symbol_type);
	if (as_function != nullptr) {
		const Result* expression_evaluation = expression->Evaluate(context);

		errors = expression_evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::GetTerminator()) {
			auto function = static_pointer_cast<const Function>(
					expression_evaluation->GetData());

			//we're assigning a struct reference
			errors = SetSymbol(context, function);
		}
	}

	return errors;
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<bool> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetBoolean());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<int> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetInt());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<double> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetDouble());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<string> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetString());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const_shared_ptr<Array> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetTypeSpecifier());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context,
		const_shared_ptr<CompoundTypeInstance> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetTypeSpecifier());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context,
		const_shared_ptr<Function> value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName(), DEEP);
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetType());
}

const Variable* BasicVariable::GetDefaultVariable() {
	static const_shared_ptr<string> name = const_shared_ptr<string>(
			new string("!!!!!DefaultVariable!!!!!"));
	const static Variable* instance = new BasicVariable(name,
			GetDefaultLocation());

	return instance;
}

const LinkedList<const Error*>* BasicVariable::Validate(
		const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName(), DEEP);

	if (symbol == nullptr || symbol == Symbol::GetDefaultSymbol()) {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().begin.line, GetLocation().begin.column,
						*(GetName())));
	}

	return errors;
}

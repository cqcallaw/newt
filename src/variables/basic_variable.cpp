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

BasicVariable::BasicVariable(const string* name, const YYLTYPE location) :
		Variable(name, location) {
}

BasicVariable::~BasicVariable() {
}

const string* BasicVariable::ToString(const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << ">";
	return new string(buffer.str());
}

const TypeSpecifier* BasicVariable::GetType(
		const ExecutionContext* context) const {
	auto symbol = context->GetSymbolContext()->GetSymbol(GetName());
	return symbol->GetType();
}

const Result* BasicVariable::Evaluate(const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName());
	const Symbol* result_symbol = Symbol::DefaultSymbol;

	if (symbol != nullptr && symbol != Symbol::DefaultSymbol) {
		result_symbol = symbol;
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	const Result* result = new Result(result_symbol->GetValue(), errors);
	return result;
}

const LinkedList<const Error*>* BasicVariable::AssignValue(
		const ExecutionContext* context, const Expression* expression,
		const AssignmentType op) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const string* variable_name = GetName();
	const int variable_line = GetLocation().first_line;
	const int variable_column = GetLocation().first_column;

	SymbolTable* symbol_table = (SymbolTable*) context->GetSymbolContext();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);
	const TypeSpecifier* symbol_type = symbol->GetType();
	const void* symbol_value = symbol->GetValue();

	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(symbol_type);
	if (as_primitive != nullptr) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					*((int*) symbol_value), expression, op, context);

			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::Terminator) {
				errors = SetSymbol(context, (bool*) result->GetData());
			}
			break;
		}
		case INT: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					*((int*) symbol_value), expression, op, context);

			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::Terminator) {
				errors = SetSymbol(context, (int*) result->GetData());
			}
			break;
		}
		case DOUBLE: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					*((double*) symbol_value), expression, op, context);

			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::Terminator) {
				errors = SetSymbol(context, (double*) result->GetData());
			}
			break;
		}
		case STRING: {
			const Result* result = AssignmentStatement::do_op(variable_name,
					basic_type, variable_line, variable_column,
					(string*) symbol_value, expression, op, context);
			errors = result->GetErrors();
			if (errors == LinkedList<const Error*>::Terminator) {
				errors = SetSymbol(context, (string*) result->GetData());
			}
			break;
		}
		default:
			assert(false);
		}
	}

	//TODO: don't allow += or -= operations on array specifiers
	const ArrayTypeSpecifier* as_array =
			dynamic_cast<const ArrayTypeSpecifier*>(symbol_type);
	if (as_array) {
		//re-assigning an array reference
		const Result* expression_evaluation = expression->Evaluate(context);

		errors = expression_evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			const Array* result_as_array =
					static_cast<const Array*>(expression_evaluation->GetData());

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
	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(symbol_type);
	if (as_compound != nullptr) {
		const Result* expression_evaluation = expression->Evaluate(context);

		errors = expression_evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			const CompoundTypeInstance* new_instance =
					(const CompoundTypeInstance*) expression_evaluation->GetData();

			//we're assigning a struct reference
			errors = SetSymbol(context, new_instance);
		}
	}

	//TODO: don't allow += or -= operations on function type specifiers
	const FunctionTypeSpecifier* as_function =
			dynamic_cast<const FunctionTypeSpecifier*>(symbol_type);
	if (as_function != nullptr) {
		const Result* expression_evaluation = expression->Evaluate(context);

		errors = expression_evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			const Function* function =
					(const Function*) expression_evaluation->GetData();

			//we're assigning a struct reference
			errors = SetSymbol(context, function);
		}
	}

	return errors;
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const bool* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetBoolean());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const int* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetInt());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const double* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetDouble());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const string* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), PrimitiveTypeSpecifier::GetString());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const Array* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetTypeSpecifier());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context,
		const CompoundTypeInstance* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetTypeSpecifier());
}

const LinkedList<const Error*>* BasicVariable::SetSymbol(
		const ExecutionContext* context, const Function* value) const {
	auto symbol_context = context->GetSymbolContext();
	auto symbol = symbol_context->GetSymbol(*GetName());
	return ToErrorList(symbol_context->SetSymbol(*GetName(), value),
			symbol->GetType(), value->GetType());
}

const Variable* BasicVariable::GetDefaultVariable() {
	const static string* name = new string("!!!!!DefaultVariable!!!!!");
	const static Variable* instance = new BasicVariable(name, DefaultLocation);

	return instance;
}

const LinkedList<const Error*>* BasicVariable::Validate(
		const ExecutionContext* context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const SymbolContext* symbol_context = context->GetSymbolContext();
	const Symbol* symbol = symbol_context->GetSymbol(GetName());

	if (symbol == nullptr || symbol == Symbol::DefaultSymbol) {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						GetLocation().first_line, GetLocation().first_column,
						*(GetName())));
	}

	return errors;
}

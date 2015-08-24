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

#include <assignment_statement.h>
#include <sstream>
#include <expression.h>
#include <variable.h>
#include <array_variable.h>
#include <error.h>
#include <defaults.h>
#include <execution_context.h>
#include <typeinfo>

AssignmentStatement::AssignmentStatement(const Variable* variable,
		const AssignmentType op_type, const Expression* expression) :
		m_variable(variable), m_op_type(op_type), m_expression(expression) {
}

AssignmentStatement::~AssignmentStatement() {
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const bool old_value, const bool expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		bool &out) {
	switch (op) {
	case ASSIGN:
		out = expression_value;
		break;
	default:
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
				variable_column, *variable_name, type_to_string(variable_type));
	}
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const int old_value, const int expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		int& out) {
	switch (op) {
	case PLUS_ASSIGN:
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		out = old_value - expression_value;
		break;
	case ASSIGN:
		out = expression_value;
		break;
	default:
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
				variable_column, *variable_name, type_to_string(variable_type));
	}
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const double old_value, const double expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		double &out) {
	switch (op) {
	case PLUS_ASSIGN:
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		out = old_value - expression_value;
		break;
	case ASSIGN:
		out = expression_value;
		break;
	default:
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
				variable_column, *variable_name, type_to_string(variable_type));
	}
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const string* old_value, const bool expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		string* &out) {
	do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const string* old_value, const int expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		string* &out) {
	do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const string* old_value, const double expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		string* &out) {
	do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const void AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const string* old_value, const string* expression_value,
		const AssignmentType op, const ExecutionContext* execution_context,
		string* &out) {
	ostringstream buffer;
	switch (op) {
	case PLUS_ASSIGN: {
		buffer << *old_value;
		buffer << *expression_value;
		out = new string(buffer.str());
		break;
	}
	case ASSIGN:
		out = new string(*expression_value);
		break;
	case MINUS_ASSIGN:
	default:
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT, variable_line,
				variable_column, *(variable_name),
				type_to_string(variable_type));
	}
}
const void* AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const int value, const Expression* expression, const AssignmentType op,
		const ExecutionContext* execution_context) {
	int new_value = 0;
	const Result* evaluation = expression->Evaluate(
			execution_context);
	const void* void_value = evaluation->GetData();
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((bool*) void_value), op, execution_context, new_value);
		break;
	case INT: {
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((int*) void_value), op, execution_context, new_value);
		break;
	}
	default:
		Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR, variable_line,
				variable_column, type_to_string(INT),
				type_to_string(expression->GetType(execution_context)));
	}

	delete (evaluation);
	return new int(new_value);
}

const void* AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const double value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	double new_value = 0;
	const Result* evaluation = expression->Evaluate(
			execution_context);
	const void* void_value = evaluation->GetData();
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((bool*) void_value), op, execution_context, new_value);
		break;
	case INT: {
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((int*) void_value), op, execution_context, new_value);
		break;
	}
	case DOUBLE: {
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((double*) void_value), op, execution_context,
				new_value);
		break;
	}
	default:
		Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR, variable_line,
				variable_column, type_to_string(DOUBLE),
				type_to_string(expression->GetType(execution_context)));
	}

	delete (evaluation);
	return new double(new_value);
}

const void* AssignmentStatement::do_op(const string* variable_name,
		const Type variable_type, int variable_line, int variable_column,
		const string* value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	string* new_value;
	const Result* evaluation = expression->Evaluate(
			execution_context);
	const void* void_value = evaluation->GetData();
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((bool*) void_value), op, execution_context, new_value);
		break;
	case INT: {
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((int*) void_value), op, execution_context, new_value);
		break;
	}
	case DOUBLE: {
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, *((double*) void_value), op, execution_context,
				new_value);
		break;
	}
	case STRING: {
		do_op(variable_name, variable_type, variable_line, variable_column,
				value, (string*) void_value, op, execution_context, new_value);
		break;
	}
	default:
		Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR, variable_line,
				variable_column, type_to_string(STRING),
				type_to_string(expression->GetType(execution_context)));
	}

	delete (evaluation);
	return new_value;
}

const void AssignmentStatement::do_op(const Variable* variable,
		const Expression* expression, const AssignmentType op,
		const ExecutionContext* execution_context) {

	const string* variable_name = variable->GetName();
	const Type variable_type = variable->GetType(execution_context);
	const int variable_line = variable->GetLocation().first_line;
	const int variable_column = variable->GetLocation().first_column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);
	const Type symbol_type = symbol->GetType();
	const void* symbol_value = symbol->GetValue();

	switch (symbol_type) {
	case BOOLEAN: {
		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, *((int*) symbol_value), expression, op,
				execution_context);
		symbol_table->SetSymbol(*variable_name, (bool*) result);
		break;
	}
	case INT: {
		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, *((int*) symbol_value), expression, op,
				execution_context);
		symbol_table->SetSymbol(*variable_name, (int*) result);
		break;
	}
	case DOUBLE: {
		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, *((double*) symbol_value), expression, op,
				execution_context);
		symbol_table->SetSymbol(*variable_name, (double*) result);
		break;
	}
	case STRING: {
		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, (string*) symbol_value, expression, op,
				execution_context);
		symbol_table->SetSymbol(*variable_name, (string*) result);
		break;
	}
	case INT_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) variable;
		const Result* evaluation =
				array_variable->GetIndexExpression()->Evaluate(
						execution_context);
		int index = *((int*) evaluation->GetData());
		delete (evaluation);

		ArraySymbol* array_symbol = (ArraySymbol*) symbol;
		if (index >= array_symbol->GetSize() || index < 0) {
			Error::semantic_error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					variable_line, variable_column, array_symbol->GetName(),
					*AsString(index));
			index = 0;
		}
		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, *(((int*) symbol_value) + index), expression,
				op, execution_context);
		symbol_table->SetSymbol(*variable_name, index, (int*) result);
		break;
	}
	case DOUBLE_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) variable;
		const Result* evaluation =
				array_variable->GetIndexExpression()->Evaluate(
						execution_context);
		int index = *((int*) evaluation->GetData());
		delete (evaluation);

		ArraySymbol* array_symbol = (ArraySymbol*) symbol;
		if (index >= array_symbol->GetSize() || index < 0) {
			Error::semantic_error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					variable_line, variable_column, array_symbol->GetName(),
					*AsString(index));
			index = 0;
		}
		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, *(((double*) symbol_value) + index),
				expression, op, execution_context);
		symbol_table->SetSymbol(*variable_name, index, (double*) result);
		break;
	}
	case STRING_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) variable;
		const Result* evaluation =
				array_variable->GetIndexExpression()->Evaluate(
						execution_context);
		int index = *((int*) evaluation->GetData());
		delete (evaluation);

		ArraySymbol* array_symbol = (ArraySymbol*) symbol;
		if (index >= array_symbol->GetSize() || index < 0) {
			Error::semantic_error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					variable_line, variable_column, array_symbol->GetName(),
					*AsString(index));
			index = 0;
		}

		const void* result = do_op(variable_name, variable_type, variable_line,
				variable_column, *(((string**) symbol_value) + index),
				expression, op, execution_context);
		symbol_table->SetSymbol(*variable_name, index, (string*) result);
		break;
	}
	default:
		assert(false);
	}
}

void AssignmentStatement::execute(
		const ExecutionContext* execution_context) const {
	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
		return;
	}

	const string* variable_name = m_variable->GetName();
	int variable_line = m_variable->GetLocation().first_line;
	int variable_column = m_variable->GetLocation().first_column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);

	if (symbol == NULL || symbol == Symbol::DefaultSymbol) {
		Error::semantic_error(Error::UNDECLARED_VARIABLE, variable_line,
				variable_column, *(variable_name));
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) == NULL
			&& (symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))) {
		//we're trying to reference an array variable without an index
		//(this probably isn't legitimate and may need to be revised for the statement "var arr1 = arr2")
		Error::semantic_error(Error::UNDECLARED_VARIABLE, variable_line,
				variable_column, *(variable_name));
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) != NULL
			&& !(symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))) {
		//trying to reference a non-array variable as an array.
		Error::semantic_error(Error::VARIABLE_NOT_AN_ARRAY, variable_line,
				variable_column, *(variable_name));
	} else {
		do_op(m_variable, m_expression, m_op_type, execution_context);
	}
}

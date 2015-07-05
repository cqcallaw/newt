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

const void AssignmentStatement::do_op(const bool old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, bool &out) const {
	switch (op) {
	case ASSIGN:
		out = expression_value;
		break;
	default:
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()),
				type_to_string(m_variable->GetType(execution_context)));
	}
}

const void AssignmentStatement::do_op(const int old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, int& out) const {
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
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()),
				type_to_string(m_variable->GetType(execution_context)));
	}
}

const void AssignmentStatement::do_op(const double old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, double &out) const {
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
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()),
				type_to_string(m_variable->GetType(execution_context)));
	}
}

const void AssignmentStatement::do_op(const string* old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) const {
	do_op(old_value, AsString(expression_value), op, execution_context, out);
}

const void AssignmentStatement::do_op(const string* old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) const {
	do_op(old_value, AsString(expression_value), op, execution_context, out);
}

const void AssignmentStatement::do_op(const string* old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) const {
	do_op(old_value, AsString(expression_value), op, execution_context, out);
}

const void AssignmentStatement::do_op(const string* old_value,
		const string* expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) const {
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
		Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()),
				type_to_string(m_variable->GetType(execution_context)));
	}
}

const void* AssignmentStatement::do_op(const int value,
		const Expression* expression, const AssignmentType op,
		const ExecutionContext* execution_context) const {
	int new_value = 0;
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		do_op(value, *((bool*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	case INT: {
		do_op(value, *((int*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	}
	default:
		Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column, type_to_string(INT),
				type_to_string(expression->GetType(execution_context)));
	}

	return new int(new_value);
}

const void* AssignmentStatement::do_op(const double value,
		const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) const {
	double new_value = 0;
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		do_op(value, *((bool*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	case INT: {
		do_op(value, *((int*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	}
	case DOUBLE: {
		do_op(value, *((double*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	}
	default:
		Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column, type_to_string(DOUBLE),
				type_to_string(expression->GetType(execution_context)));
	}

	return new double(new_value);
}

const void* AssignmentStatement::do_op(const string* value,
		const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) const {
	string* new_value;
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		do_op(value, *((bool*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	case INT: {
		do_op(value, *((int*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	}
	case DOUBLE: {
		do_op(value, *((double*) expression->Evaluate(execution_context)), op,
				execution_context, new_value);
		break;
	}
	case STRING: {
		do_op(value, (string*) expression->Evaluate(execution_context),
				m_op_type, execution_context, new_value);
		break;
	}
	default:
		Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column, type_to_string(STRING),
				type_to_string(expression->GetType(execution_context)));
	}

	return new_value;
}

void AssignmentStatement::execute(
		const ExecutionContext* execution_context) const {
	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
		return;
	}

	/*if (m_variable == DefaultVariable || m_expression == DefaultExpression) {
		return;
	}*/

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();
	const Symbol* symbol = symbol_table->GetSymbol(m_variable->GetName());

	if (symbol == NULL || symbol == Symbol::DefaultSymbol) {
		Error::semantic_error(Error::UNDECLARED_VARIABLE,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()));
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) == NULL
			&& (symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))) {
		//we're trying to reference an array variable without an index
		Error::semantic_error(Error::UNDECLARED_VARIABLE,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()));
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) != NULL
			&& !(symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))) {
		//trying to reference a non-array variable as an array.
		Error::semantic_error(Error::VARIABLE_NOT_AN_ARRAY,
				m_variable->GetLocation().first_line,
				m_variable->GetLocation().first_column,
				*(m_variable->GetName()));
		/*} else if (!(m_variable->GetType(execution_context)
		 & (BOOLEAN | INT | DOUBLE | STRING))) {
		 //cannot assign
		 Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT,
		 m_variable->GetLocation().first_line,
		 m_variable->GetLocation().first_column,
		 *(m_variable->GetName()),
		 type_to_string(m_variable->GetType(execution_context)));*/
	} else {

		/*Type variable_type = $1->GetType();
		 Type expression_type = $3->GetType();

		 if (!(variable_type & (BOOLEAN | INT | DOUBLE | STRING))) {
		 Error::semantic_error(Error::INVALID_LHS_OF_ASSIGNMENT, @1.first_line, @1.first_column, *($1->GetName()), type_to_string(variable_type));
		 $$ = DefaultAssignmentStatement;
		 } else if (variable_type <= STRING && expression_type > variable_type) {
		 Error::semantic_error(Error::ASSIGNMENT_TYPE_ERROR, @3.first_line, @3.first_column, type_to_string(variable_type), type_to_string(expression_type));
		 $$ = DefaultAssignmentStatement;
		 } else {*/

		const string symbol_name = symbol->GetName();
		const Type symbol_type = symbol->GetType();
		const void* symbol_value = symbol->GetValue();

		switch (symbol_type) {
		case BOOLEAN: {
			const void* result = do_op(*((int*) symbol_value), m_expression,
					m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, (bool*) result);
			break;
		}
		case INT: {
			const void* result = do_op(*((int*) symbol_value), m_expression,
					m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, (int*) result);
			break;
		}
		case DOUBLE: {
			const void* result = do_op(*((double*) symbol_value), m_expression,
					m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, (double*) result);
			break;
		}
		case STRING: {
			const void* result = do_op((string*) symbol_value, m_expression,
					m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, (string*) result);
			break;
		}
		case INT_ARRAY: {
			ArrayVariable* array_variable = (ArrayVariable*) m_variable;
			int index = *((int*) array_variable->GetIndexExpression()->Evaluate(
					execution_context));

			ArraySymbol* array_symbol = (ArraySymbol*) symbol;
			if (index >= array_symbol->GetSize() || index < 0) {
				Error::semantic_error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						array_symbol->GetName(), *AsString(index));
				index = 0;
			}
			const void* result = do_op(*(((int*) symbol_value) + index),
					m_expression, m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, index, (int*) result);
			break;
		}
		case DOUBLE_ARRAY: {
			ArrayVariable* array_variable = (ArrayVariable*) m_variable;
			int index = *((int*) array_variable->GetIndexExpression()->Evaluate(
					execution_context));
			ArraySymbol* array_symbol = (ArraySymbol*) symbol;
			if (index >= array_symbol->GetSize() || index < 0) {
				Error::semantic_error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						array_symbol->GetName(), *AsString(index));
				index = 0;
			}
			const void* result = do_op(*(((double*) symbol_value) + index),
					m_expression, m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, index, (double*) result);
			break;
		}
		case STRING_ARRAY: {
			ArrayVariable* array_variable = (ArrayVariable*) m_variable;
			int index = *((int*) array_variable->GetIndexExpression()->Evaluate(
					execution_context));
			ArraySymbol* array_symbol = (ArraySymbol*) symbol;
			if (index >= array_symbol->GetSize() || index < 0) {
				Error::semantic_error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						array_symbol->GetName(), *AsString(index));
				index = 0;
			}

			const void* result = do_op(*(((string**) symbol_value) + index),
					m_expression, m_op_type, execution_context);
			symbol_table->SetSymbol(symbol_name, index, (string*) result);
			break;
		}
		default:
			assert(false);
		}
	}
}

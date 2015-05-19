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

#include <sstream>
#include "assignment_statement.h"

#include "expression.h"
#include "variable.h"
#include "array_variable.h"
#include "error.h"
#include "defaults.h"

AssignmentStatement::AssignmentStatement(const Variable* variable,
		const AssignmentType op_type, const Expression* expression) :
		m_variable(variable), m_op_type(op_type), m_expression(expression) {
}

AssignmentStatement::~AssignmentStatement() {
}

const void AssignmentStatement::do_op(const bool old_value,
		const bool expression_value, const AssignmentType op, bool &out) const {
	switch (op) {
	case ASSIGN:
		out = expression_value;
		break;
	default:
		assert(false);
	}
}

const void AssignmentStatement::do_op(const int old_value,
		const int expression_value, const AssignmentType op, int& out) const {
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
		assert(false);
	}
}

const void AssignmentStatement::do_op(const double old_value,
		const double expression_value, const AssignmentType op,
		double &out) const {
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
		assert(false);
	}
}

const void AssignmentStatement::do_op(const string* old_value,
		const bool expression_value, const AssignmentType op,
		string* &out) const {
	do_op(old_value, AsString(expression_value), op, out);
}

const void AssignmentStatement::do_op(const string* old_value,
		const int expression_value, const AssignmentType op,
		string* &out) const {
	do_op(old_value, AsString(expression_value), op, out);
}

const void AssignmentStatement::do_op(const string* old_value,
		const double expression_value, const AssignmentType op,
		string* &out) const {
	do_op(old_value, AsString(expression_value), op, out);
}

const void AssignmentStatement::do_op(const string* old_value,
		const string* expression_value, const AssignmentType op,
		string* &out) const {
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
		assert(false);
	}
}

const void* AssignmentStatement::do_op(const int value,
		const Expression* expression, const AssignmentType op) const {
	int new_value = 0;
	switch (expression->GetType()) {
	case BOOLEAN:
		do_op(value, *((bool*) expression->Evaluate()), op, new_value);
		break;
	case INT: {
		do_op(value, *((int*) expression->Evaluate()), op, new_value);
		break;
	}
	default:
		assert(false);
	}

	return new int(new_value);
}

const void* AssignmentStatement::do_op(const double value,
		const Expression* expression, AssignmentType op) const {
	double new_value = 0;
	switch (expression->GetType()) {
	case BOOLEAN:
		do_op(value, *((bool*) expression->Evaluate()), op, new_value);
		break;
	case INT: {
		do_op(value, *((int*) expression->Evaluate()), op, new_value);
		break;
	}
	case DOUBLE: {
		do_op(value, *((double*) expression->Evaluate()), op, new_value);
		break;
	}
	default:
		assert(false);
	}

	return new double(new_value);
}

const void* AssignmentStatement::do_op(const string* value,
		const Expression* expression, AssignmentType op) const {
	string* new_value;
	switch (expression->GetType()) {
	case BOOLEAN:
		do_op(value, *((bool*) expression->Evaluate()), op, new_value);
		break;
	case INT: {
		do_op(value, *((int*) expression->Evaluate()), op, new_value);
		break;
	}
	case DOUBLE: {
		do_op(value, *((double*) expression->Evaluate()), op, new_value);
		break;
	}
	case STRING: {
		do_op(value, (string*) expression->Evaluate(), m_op_type, new_value);
		break;
	}
	default:
		assert(false);
	}

	return new_value;
}

void AssignmentStatement::execute() const {
	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
		return;
	}

	if (m_variable == DefaultVariable || m_expression == DefaultExpression) {
		return;
	}

	SymbolTable* symbol_table = SymbolTable::instance();
	const Symbol* symbol = symbol_table->GetSymbol(m_variable->GetName());
	const string symbol_name = symbol->GetName();
	const Type symbol_type = symbol->GetType();
	const void* symbol_value = symbol->GetValue();

	switch (symbol_type) {
	case BOOLEAN: {
		const void* result = do_op(*((int*) symbol_value), m_expression,
				m_op_type);
		symbol_table->SetSymbol(symbol_name, (bool*) result);
		break;
	}
	case INT: {
		const void* result = do_op(*((int*) symbol_value), m_expression,
				m_op_type);
		symbol_table->SetSymbol(symbol_name, (int*) result);
		break;
	}
	case DOUBLE: {
		const void* result = do_op(*((double*) symbol_value), m_expression,
				m_op_type);
		symbol_table->SetSymbol(symbol_name, (double*) result);
		break;
	}
	case STRING: {
		const void* result = do_op((string*) symbol_value, m_expression,
				m_op_type);
		symbol_table->SetSymbol(symbol_name, (string*) result);
		break;
	}
	case INT_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) m_variable;
		int index = *((int*) array_variable->GetIndexExpression()->Evaluate());

		ArraySymbol* array_symbol = (ArraySymbol*) symbol;
		if (index >= array_symbol->GetSize() || index < 0) {
			Error::error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					array_symbol->GetName(), *AsString(index));
			index = 0;
		}
		const void* result = do_op(*(((int*) symbol_value) + index),
				m_expression, m_op_type);
		symbol_table->SetSymbol(symbol_name, index, (int*) result);
		break;
	}
	case DOUBLE_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) m_variable;
		int index = *((int*) array_variable->GetIndexExpression()->Evaluate());
		ArraySymbol* array_symbol = (ArraySymbol*) symbol;
		if (index >= array_symbol->GetSize() || index < 0) {
			Error::error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					array_symbol->GetName(), *AsString(index));
			index = 0;
		}
		const void* result = do_op(*(((double*) symbol_value) + index),
				m_expression, m_op_type);
		symbol_table->SetSymbol(symbol_name, index, (double*) result);
		break;
	}
	case STRING_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) m_variable;
		int index = *((int*) array_variable->GetIndexExpression()->Evaluate());
		ArraySymbol* array_symbol = (ArraySymbol*) symbol;
		if (index >= array_symbol->GetSize() || index < 0) {
			Error::error(Error::ARRAY_INDEX_OUT_OF_BOUNDS,
					array_symbol->GetName(), *AsString(index));
			index = 0;
		}

		const void* result = do_op(*(((string**) symbol_value) + index),
				m_expression, m_op_type);
		symbol_table->SetSymbol(symbol_name, index, (string*) result);
		break;
	}
	default:
		assert(false);
	}
}

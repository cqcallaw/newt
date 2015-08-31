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

LinkedList<const Error*>* AssignmentStatement::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();
	const Symbol* symbol = symbol_table->GetSymbol(m_variable->GetName());
	const BasicType symbol_type = symbol->GetType();

	switch (symbol_type) {
	case INT_ARRAY:
	case DOUBLE_ARRAY:
	case STRING_ARRAY: {
		const ArrayVariable* array_variable = (ArrayVariable*) m_variable;
		const string* variable_name = m_variable->GetName();
		const Expression* array_index_expression =
				array_variable->GetIndexExpression();
		const YYLTYPE array_index_expression_position =
				array_index_expression->GetPosition();
		const BasicType index_type = array_index_expression->GetType(
				execution_context);

		if (!(index_type & INT)) {
			ostringstream os;
			os << "A " << index_type << " expression";

			errors = new LinkedList<const Error*>(
					new Error(Error::SEMANTIC,
							Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
							array_index_expression_position.first_line,
							array_index_expression_position.first_column,
							*variable_name, os.str()));
		} else {
			errors = LinkedList<const Error*>::Terminator;
		}
		break;
	}
	case BOOLEAN:
	case INT:
	case DOUBLE:
	case STRING:
	default: {
		errors = LinkedList<const Error*>::Terminator;
		break;
	}
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const bool old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, bool &out) {
	const LinkedList<const Error*>* errors;
	switch (op) {
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = expression_value;
		break;
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *variable_name,
						type_to_string(variable_type)));
		out = false;
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const int old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, int& out) {
	const LinkedList<const Error*>* errors;

	switch (op) {
	case PLUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value - expression_value;
		break;
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = expression_value;
		break;
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *variable_name,
						type_to_string(variable_type)));
		out = 0;
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const double old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, double &out) {
	const LinkedList<const Error*>* errors;

	switch (op) {
	case PLUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value + expression_value;
		break;
	case MINUS_ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = old_value - expression_value;
		break;
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = expression_value;
		break;
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *variable_name,
						type_to_string(variable_type)));
		out = 1.0;
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const bool expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const int expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const double expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	return do_op(variable_name, variable_type, variable_line, variable_column,
			old_value, AsString(expression_value), op, execution_context, out);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const string* variable_name, const BasicType variable_type,
		int variable_line, int variable_column, const string* old_value,
		const string* expression_value, const AssignmentType op,
		const ExecutionContext* execution_context, string* &out) {
	const LinkedList<const Error*>* errors;

	ostringstream buffer;
	switch (op) {
	case PLUS_ASSIGN: {
		errors = LinkedList<const Error*>::Terminator;
		buffer << *old_value;
		buffer << *expression_value;
		out = new string(buffer.str());
		break;
	}
	case ASSIGN:
		errors = LinkedList<const Error*>::Terminator;
		out = new string(*expression_value);
		break;
	case MINUS_ASSIGN:
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::INVALID_LHS_OF_ASSIGNMENT,
						variable_line, variable_column, *(variable_name),
						type_to_string(variable_type)));
	}

	return errors;
}
const Result* AssignmentStatement::do_op(const string* variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const int value, const Expression* expression, const AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	const void* result;

	int new_value = 0;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	}

	const void* void_value = evaluation->GetData();
	delete (evaluation);
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((bool*) void_value), op,
				execution_context, new_value);
		break;
	case INT: {
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((int*) void_value), op,
				execution_context, new_value);
		break;
	}
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						variable_line, variable_column, type_to_string(INT),
						type_to_string(
								expression->GetType(execution_context))));
	}
	result = new int(new_value);

	return new Result(result, errors);
}

const Result* AssignmentStatement::do_op(const string* variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const double value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	const void* result;

	double new_value = 0;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	}

	const void* void_value = evaluation->GetData();
	delete (evaluation);
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((bool*) void_value), op,
				execution_context, new_value);
		break;
	case INT: {
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((int*) void_value), op,
				execution_context, new_value);
		break;
	}
	case DOUBLE: {
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((double*) void_value), op,
				execution_context, new_value);
		break;
	}
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						variable_line, variable_column, type_to_string(DOUBLE),
						type_to_string(
								expression->GetType(execution_context))));
	}

	result = new double(new_value);

	return new Result(result, errors);
}

const Result* AssignmentStatement::do_op(const string* variable_name,
		const BasicType variable_type, int variable_line, int variable_column,
		const string* value, const Expression* expression, AssignmentType op,
		const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;

	string* new_value;
	const Result* evaluation = expression->Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	}

	const void* void_value = evaluation->GetData();
	delete (evaluation);
	switch (expression->GetType(execution_context)) {
	case BOOLEAN:
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((bool*) void_value), op,
				execution_context, new_value);
		break;
	case INT: {
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((int*) void_value), op,
				execution_context, new_value);
		break;
	}
	case DOUBLE: {
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, *((double*) void_value), op,
				execution_context, new_value);
		break;
	}
	case STRING: {
		errors = do_op(variable_name, variable_type, variable_line,
				variable_column, value, (string*) void_value, op,
				execution_context, new_value);
		break;
	}
	default:
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						variable_line, variable_column, type_to_string(STRING),
						type_to_string(
								expression->GetType(execution_context))));
	}

	return new Result(new_value, errors);
}

const LinkedList<const Error*>* AssignmentStatement::do_op(
		const Variable* variable, const Expression* expression,
		const AssignmentType op, const ExecutionContext* execution_context) {
	const LinkedList<const Error*>* errors;
	SetResult set_result = NO_SET_RESULT;

	const string* variable_name = variable->GetName();
	const BasicType variable_type = variable->GetType(execution_context);
	const int variable_line = variable->GetLocation().first_line;
	const int variable_column = variable->GetLocation().first_column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);
	const BasicType symbol_type = symbol->GetType();
	const void* symbol_value = symbol->GetValue();

	switch (symbol_type) {
	case BOOLEAN: {
		const Result* result = do_op(variable_name, variable_type,
				variable_line, variable_column, *((int*) symbol_value),
				expression, op, execution_context);

		errors = result->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			set_result = symbol_table->SetSymbol(*variable_name,
					(bool*) result->GetData());
		}
		break;
	}
	case INT: {
		const Result* result = do_op(variable_name, variable_type,
				variable_line, variable_column, *((int*) symbol_value),
				expression, op, execution_context);

		errors = result->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			set_result = symbol_table->SetSymbol(*variable_name,
					(int*) result->GetData());
		}
		break;
	}
	case DOUBLE: {
		const Result* result = do_op(variable_name, variable_type,
				variable_line, variable_column, *((double*) symbol_value),
				expression, op, execution_context);

		errors = result->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			set_result = symbol_table->SetSymbol(*variable_name,
					(double*) result->GetData());
		}
		break;
	}
	case STRING: {
		const Result* result = do_op(variable_name, variable_type,
				variable_line, variable_column, (string*) symbol_value,
				expression, op, execution_context);
		errors = result->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			set_result = symbol_table->SetSymbol(*variable_name,
					(string*) result->GetData());
		}
		break;
	}
	case INT_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) variable;
		const Result* evaluation =
				array_variable->GetIndexExpression()->Evaluate(
						execution_context);

		errors = evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			int index = *((int*) evaluation->GetData());
			delete (evaluation);

			ArraySymbol* array_symbol = (ArraySymbol*) symbol;
			if (index >= array_symbol->GetSize() || index < 0) {
				errors = new LinkedList<const Error*>(
						new Error(Error::SEMANTIC,
								Error::ARRAY_INDEX_OUT_OF_BOUNDS, variable_line,
								variable_column, array_symbol->GetName(),
								*AsString(index)));
			} else {
				const Result* result = do_op(variable_name, variable_type,
						variable_line, variable_column,
						*(((int*) symbol_value) + index), expression, op,
						execution_context);

				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name, index,
							(int*) result->GetData());
				}
			}
		}
		break;
	}
	case DOUBLE_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) variable;
		const Result* evaluation =
				array_variable->GetIndexExpression()->Evaluate(
						execution_context);

		errors = evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			int index = *((int*) evaluation->GetData());
			delete (evaluation);

			ArraySymbol* array_symbol = (ArraySymbol*) symbol;
			if (index >= array_symbol->GetSize() || index < 0) {
				errors = new LinkedList<const Error*>(
						new Error(Error::SEMANTIC,
								Error::ARRAY_INDEX_OUT_OF_BOUNDS, variable_line,
								variable_column, array_symbol->GetName(),
								*AsString(index)));
			} else {
				const Result* result = do_op(variable_name, variable_type,
						variable_line, variable_column,
						*(((double*) symbol_value) + index), expression, op,
						execution_context);

				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name, index,
							(double*) result->GetData());
				}
			}
		}
		break;
	}
	case STRING_ARRAY: {
		ArrayVariable* array_variable = (ArrayVariable*) variable;
		const Result* evaluation =
				array_variable->GetIndexExpression()->Evaluate(
						execution_context);

		errors = evaluation->GetErrors();
		if (errors == LinkedList<const Error*>::Terminator) {
			int index = *((int*) evaluation->GetData());
			delete (evaluation);

			ArraySymbol* array_symbol = (ArraySymbol*) symbol;
			if (index >= array_symbol->GetSize() || index < 0) {
				errors = new LinkedList<const Error*>(
						new Error(Error::SEMANTIC,
								Error::ARRAY_INDEX_OUT_OF_BOUNDS, variable_line,
								variable_column, array_symbol->GetName(),
								*AsString(index)));
			} else {
				const Result* result = do_op(variable_name, variable_type,
						variable_line, variable_column,
						*(((string**) symbol_value) + index), expression, op,
						execution_context);

				errors = result->GetErrors();
				if (errors == LinkedList<const Error*>::Terminator) {
					set_result = symbol_table->SetSymbol(*variable_name, index,
							(string*) result->GetData());
				}
			}
		}
		break;
	}
	default:
		assert(false);
	}

	if (set_result != NO_SET_RESULT) {
		assert(set_result == SET_SUCCESS);
	}

	return errors;
}

const LinkedList<const Error*>* AssignmentStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors;

	if (m_variable == nullptr || m_expression == nullptr) {
		assert(false);
	}

	const string* variable_name = m_variable->GetName();
	int variable_line = m_variable->GetLocation().first_line;
	int variable_column = m_variable->GetLocation().first_column;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();
	const Symbol* symbol = symbol_table->GetSymbol(variable_name);

	if (symbol == NULL || symbol == Symbol::DefaultSymbol) {
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *(variable_name)));
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) == NULL
			&& (symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))) {
//we're trying to reference an array variable without an index
//(this probably isn't legitimate and may need to be revised for the statement "var arr1 = arr2")
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						variable_line, variable_column, *(variable_name)));
	} else if (dynamic_cast<const ArrayVariable*>(m_variable) != NULL
			&& !(symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))) {
//trying to reference a non-array variable as an array.
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC, Error::VARIABLE_NOT_AN_ARRAY,
						variable_line, variable_column, *(variable_name)));
	} else {
		errors = do_op(m_variable, m_expression, m_op_type, execution_context);
	}

	return errors;
}

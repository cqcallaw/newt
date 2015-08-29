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

#include <variable_expression.h>
#include <variable.h>
#include <array_variable.h>
#include <error.h>
#include <sstream>
#include <execution_context.h>
#include <type.h>

VariableExpression::VariableExpression(const YYLTYPE position,
		const Variable* variable) :
		Expression(position), m_variable(variable) {
}

const PrimitiveType VariableExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_variable->GetType(execution_context);
}

const Result* VariableExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;
	void* result = nullptr;

	const Symbol* symbol = execution_context->GetSymbolTable()->GetSymbol(
			m_variable->GetName());

	if (symbol == nullptr || symbol == Symbol::DefaultSymbol) {
		errors = (LinkedList<const Error*>*) errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						*(m_variable->GetName())));
	} else {
		switch (symbol->GetType()) {
		case NONE:
			break;
		case INT_ARRAY:
		case DOUBLE_ARRAY:
		case STRING_ARRAY: {
			ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;

			ArrayVariable* as_array_variable = (ArrayVariable*) m_variable;
			const Expression* array_index_expression =
					as_array_variable->GetIndexExpression();

			const Result* evaluation =
					array_index_expression->Evaluate(execution_context);
			const LinkedList<const Error*>* evaluation_errors =
					evaluation->GetErrors();

			if (evaluation_errors != LinkedList<const Error*>::Terminator) {
				errors = (LinkedList<const Error*>*) evaluation_errors;
			} else {
				int index = *((int *) evaluation->GetData());

				if (index > as_array_symbol->GetSize()) {
					ostringstream buffer;
					buffer << index;
					errors = (LinkedList<const Error*>*) errors->With(
							new Error(Error::SEMANTIC,
									Error::ARRAY_INDEX_OUT_OF_BOUNDS,
									m_variable->GetLocation().first_line,
									m_variable->GetLocation().first_column,
									*(m_variable->GetName()), buffer.str()));
				} else {
					result = (void *) as_array_symbol->GetValue(index);
				}
			}

			delete (evaluation);
			break;
		}

		case INT:
		case DOUBLE:
		case STRING:
		default: {
			result = (void*) symbol->GetValue();
			break;
		}
		}
	}

	return new Result(result, errors);
}

const LinkedList<const Error*>* VariableExpression::Validate(
		const ExecutionContext* execution_context) const {
	const string* variable_name = m_variable->GetName();
	const Symbol* symbol = execution_context->GetSymbolTable()->GetSymbol(
			variable_name);

	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;

	if (symbol == NULL || symbol == Symbol::DefaultSymbol) {
		result = (LinkedList<const Error*>*) result->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						m_variable->GetLocation().first_line,
						m_variable->GetLocation().first_column,
						*(variable_name)));
		return result;
	}

	switch (symbol->GetType()) {
	case INT_ARRAY:
	case DOUBLE_ARRAY:
	case STRING_ARRAY: {
		ArrayVariable* as_array_variable = (ArrayVariable*) m_variable;
		const Expression* array_index_expression =
				as_array_variable->GetIndexExpression();
		const YYLTYPE array_index_expression_position =
				array_index_expression->GetPosition();

		PrimitiveType index_type = array_index_expression->GetType(execution_context);
		if (index_type != INT) {
			ostringstream os;
			os << "A " << index_type << " expression";

			result = (LinkedList<const Error*>*) result->With(
					new Error(Error::SEMANTIC,
							Error::ARRAY_INDEX_MUST_BE_AN_INTEGER,
							array_index_expression_position.first_line,
							array_index_expression_position.first_column,
							*variable_name, os.str()));
		} else if (array_index_expression->IsConstant()) {
			//index expression is a constant; validate it as appropriate
			const Symbol* symbol =
					execution_context->GetSymbolTable()->GetSymbol(
							variable_name);
			ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;

			if (as_array_symbol->IsInitialized()) {
				const Result* evaluation =
						array_index_expression->Evaluate(execution_context);

				const LinkedList<const Error*>* evaluation_errors =
						evaluation->GetErrors();

				if (evaluation_errors != LinkedList<const Error*>::Terminator) {
					result = (LinkedList<const Error*>*) result->Concatenate(
							evaluation_errors, true);
				} else {
					int index = *((int *) evaluation->GetData());

					if (index > as_array_symbol->GetSize()) {
						ostringstream buffer;
						buffer << index;
						result = (LinkedList<const Error*>*) result->With(
								new Error(Error::SEMANTIC,
										Error::ARRAY_INDEX_OUT_OF_BOUNDS,
										m_variable->GetLocation().first_line,
										m_variable->GetLocation().first_column,
										*variable_name, buffer.str()));
					}
				}

				delete (evaluation);
			}
		}
		break;
	}
	case NONE:
	case INT:
	case DOUBLE:
	case STRING:
	default: {
		break;
	}
	}

	return result;
}

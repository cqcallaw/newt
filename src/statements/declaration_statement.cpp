/*
 * declaration_statement.cpp
 *
 *  Created on: Jun 14, 2015
 *      Author: caleb
 */

#include <declaration_statement.h>
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <variable.h>

DeclarationStatement::DeclarationStatement(const BasicType type,
		const YYLTYPE type_position, const std::string* name,
		const YYLTYPE name_position, const Expression* initializer_expression,
		const YYLTYPE initializer_position) :
		m_type(type), m_type_position(type_position), m_name(name), m_name_position(
				name_position), m_initializer_expression(
				initializer_expression), m_initializer_position(
				initializer_position) {
}

DeclarationStatement::~DeclarationStatement() {
}

LinkedList<const Error*>* DeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;
	Symbol* symbol = (Symbol*) Symbol::DefaultSymbol;
	const Expression* expression = m_initializer_expression;
	const string* name = m_name;

	if (expression) {
		result = (LinkedList<const Error*>*) expression->Validate(
				execution_context);
	}

	switch (m_type) {
	case BOOLEAN: {
		if (expression != NULL && expression != nullptr
				&& expression->GetType(execution_context) != NONE) {
			if (!(expression->GetType(execution_context) & (BOOLEAN))) {
				result = (LinkedList<const Error*>*) result->With(
						new Error(Error::SEMANTIC,
								Error::INVALID_TYPE_FOR_INITIAL_VALUE,
								m_initializer_position.first_line,
								m_initializer_position.first_column, *name));
			}
		}

		symbol = new Symbol(name, new bool(false));
		break;
	}
	case INT: {
		if (expression != NULL
				&& expression->GetType(execution_context) != NONE) {
			if (!(expression->GetType(execution_context) & (BOOLEAN | INT))) {
				result = (LinkedList<const Error*>*) result->With(
						new Error(Error::SEMANTIC,
								Error::INVALID_TYPE_FOR_INITIAL_VALUE,
								m_initializer_position.first_line,
								m_initializer_position.first_column, *name));
			}
		}

		symbol = new Symbol(name, new int(0));
		break;
	}
	case DOUBLE: {
		if (expression != NULL
				&& expression->GetType(execution_context) != NONE) {
			if (!(expression->GetType(execution_context)
					& (BOOLEAN | INT | DOUBLE))) {
				result = (LinkedList<const Error*>*) result->With(
						new Error(Error::SEMANTIC,
								Error::INVALID_TYPE_FOR_INITIAL_VALUE,
								m_initializer_position.first_line,
								m_initializer_position.first_column, *name));
			}
		}

		symbol = new Symbol(name, new double(0.0));
		break;
	}
	case STRING: {
		if (expression != NULL
				&& expression->GetType(execution_context) != NONE) {
			if (!(expression->GetType(execution_context)
					& (BOOLEAN | INT | DOUBLE | STRING))) {
				result = (LinkedList<const Error*>*) result->With(
						new Error(Error::SEMANTIC,
								Error::INVALID_TYPE_FOR_INITIAL_VALUE,
								m_initializer_position.first_line,
								m_initializer_position.first_column, *name));
			}
		}

		symbol = new Symbol(name, new string(""));
		break;
	}
	default:
		assert(false);
		break;
	}

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();

	if (symbol != Symbol::DefaultSymbol) {
		InsertResult insert_result = symbol_table->InsertSymbol(symbol);
		if (insert_result == SYMBOL_EXISTS) {
			result = (LinkedList<const Error*>*) result->With(
					new Error(Error::SEMANTIC,
							Error::PREVIOUSLY_DECLARED_VARIABLE,
							m_name_position.first_line,
							m_name_position.first_column, *m_name));
		}
	}

	return result;
}

const LinkedList<const Error*>* DeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	if (m_initializer_expression != nullptr) {
		Variable* temp_variable = new Variable(m_name, m_name_position);
		auto errors = AssignmentStatement::do_op(temp_variable,
				m_initializer_expression, AssignmentStatement::ASSIGN,
				execution_context);
		delete (temp_variable);

		return errors;
	} else {
		return LinkedList<const Error*>::Terminator;
	}
}

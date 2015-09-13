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
#include <variable_expression.h>

DeclarationStatement::DeclarationStatement(const TypeSpecifier* type,
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

const LinkedList<const Error*>* DeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const Symbol* symbol = Symbol::DefaultSymbol;
	const Expression* expression = m_initializer_expression;
	const string* name = m_name;

	if (expression != nullptr) {
		errors = expression->Validate(execution_context);
	}

	const TypeTable* type_table = execution_context->GetTypeTable();

	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(m_type);

	if (as_primitive != nullptr) {
		if (expression != nullptr) {
			const TypeSpecifier* expression_type_specifier =
					expression->GetType(execution_context);

			const PrimitiveTypeSpecifier* expression_as_primitive =
					dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type_specifier);

			if (expression_as_primitive == nullptr
					|| !expression_as_primitive->IsAssignableTo(as_primitive)) {
				errors = errors->With(
						new Error(Error::SEMANTIC,
								Error::INVALID_TYPE_FOR_INITIAL_VALUE,
								m_initializer_position.first_line,
								m_initializer_position.first_column, *name));
			}
		}

		const void* value = m_type->DefaultValue(type_table);
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			symbol = new Symbol(name, (bool*) value);
			break;
		}
		case INT: {
			symbol = new Symbol(name, (int*) value);
			break;
		}
		case DOUBLE: {
			symbol = new Symbol(name, (double*) value);
			break;
		}
		case STRING: {
			symbol = new Symbol(name, (string*) value);
			break;
		}
		default:
			assert(false);
			break;
		}
	} else {
		assert(false); //our basic declaration statements should always be primitives
	}

	if (symbol != Symbol::DefaultSymbol) {
		SymbolTable* symbol_table =
				(SymbolTable*) execution_context->GetSymbolContext();

		InsertResult insert_result = symbol_table->InsertSymbol(symbol);
		if (insert_result == SYMBOL_EXISTS) {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::PREVIOUSLY_DECLARED_VARIABLE,
							m_name_position.first_line,
							m_name_position.first_column, *m_name));
		}
	}

	return errors;
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

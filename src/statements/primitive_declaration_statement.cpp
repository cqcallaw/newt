/*
 * declaration_statement.cpp
 *
 *  Created on: Jun 14, 2015
 *      Author: caleb
 */

#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <basic_variable.h>
#include <primitive_declaration_statement.h>
#include <variable_expression.h>

PrimitiveDeclarationStatement::PrimitiveDeclarationStatement(
		const TypeSpecifier* type, const YYLTYPE type_position,
		const std::string* name, const YYLTYPE name_position,
		const Expression* initializer_expression) :
		m_type(type), m_type_position(type_position), m_name(name), m_name_position(
				name_position), m_initializer_expression(initializer_expression) {
}

PrimitiveDeclarationStatement::~PrimitiveDeclarationStatement() {
}

const LinkedList<const Error*>* PrimitiveDeclarationStatement::preprocess(
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
			const VariableExpression* as_variable =
					dynamic_cast<const VariableExpression*>(expression);

			if (as_variable) {
				errors = as_variable->Validate(execution_context);
			}

			if (errors->IsTerminator()) {
				const TypeSpecifier* expression_type_specifier =
						expression->GetType(execution_context);

				const PrimitiveTypeSpecifier* expression_as_primitive =
						dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type_specifier);

				if (expression_as_primitive == nullptr
						|| !expression_as_primitive->IsAssignableTo(
								as_primitive)) {
					errors =
							errors->With(
									new Error(Error::SEMANTIC,
											Error::INVALID_TYPE_FOR_INITIAL_VALUE,
											m_initializer_expression->GetPosition().first_line,
											m_initializer_expression->GetPosition().first_column,
											*name));
				}
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

const LinkedList<const Error*>* PrimitiveDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	if (m_initializer_expression != nullptr) {
		Variable* temp_variable = new BasicVariable(m_name, m_name_position);
		auto errors = temp_variable->AssignValue(execution_context,
				m_initializer_expression, AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return LinkedList<const Error*>::Terminator;
	}
}

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
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const Expression* initializer_expression) :
		DeclarationStatement(position), m_type(type), m_type_position(
				type_position), m_name(name), m_name_position(name_position), m_initializer_expression(
				initializer_expression) {
}

PrimitiveDeclarationStatement::~PrimitiveDeclarationStatement() {
}

const LinkedList<const Error*>* PrimitiveDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();
	const Symbol* symbol = Symbol::GetDefaultSymbol();
	const Expression* expression = m_initializer_expression;

	if (expression != nullptr) {
		errors = expression->Validate(execution_context);
	}

	auto type_table = execution_context->GetTypeTable();

	auto as_primitive = std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
			m_type);

	if (as_primitive != nullptr) {
		if (expression != nullptr) {
			const VariableExpression* as_variable =
					dynamic_cast<const VariableExpression*>(expression);

			if (as_variable) {
				errors = as_variable->Validate(execution_context);
			}

			if (errors->IsTerminator()) {
				const_shared_ptr<TypeSpecifier> expression_type_specifier =
						expression->GetType(execution_context);

				auto expression_as_primitive = std::dynamic_pointer_cast<
						const PrimitiveTypeSpecifier>(
						expression_type_specifier);

				if (expression_as_primitive == nullptr
						|| !expression_as_primitive->IsAssignableTo(
								as_primitive)) {
					errors =
							errors->With(
									new Error(Error::SEMANTIC,
											Error::INVALID_INITIALIZER_TYPE,
											m_initializer_expression->GetPosition().begin.line,
											m_initializer_expression->GetPosition().begin.column,
											*m_name, as_primitive->ToString(),
											expression_type_specifier->ToString()));
				}
			}
		}

		auto value = m_type->DefaultValue(*type_table);
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			symbol = new Symbol(static_pointer_cast<const bool>(value));
			break;
		}
		case INT: {
			symbol = new Symbol(static_pointer_cast<const int>(value));
			break;
		}
		case DOUBLE: {
			symbol = new Symbol(static_pointer_cast<const double>(value));
			break;
		}
		case STRING: {
			symbol = new Symbol(static_pointer_cast<const string>(value));
			break;
		}
		default:
			assert(false);
			break;
		}
	} else {
		assert(false);
	}

	if (symbol != Symbol::GetDefaultSymbol()) {
		SymbolTable* symbol_table =
				(SymbolTable*) execution_context->GetSymbolContext();

		InsertResult insert_result = symbol_table->InsertSymbol(*m_name,
				symbol);
		if (insert_result == SYMBOL_EXISTS) {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::PREVIOUSLY_DECLARED_VARIABLE,
							m_name_position.begin.line,
							m_name_position.begin.column, *m_name));
		}
	}

	return errors;
}

const LinkedList<const Error*>* PrimitiveDeclarationStatement::execute(
		ExecutionContext* execution_context) const {
	if (m_initializer_expression != nullptr) {
		Variable* temp_variable = new BasicVariable(m_name, m_name_position);
		auto errors = temp_variable->AssignValue(execution_context,
				m_initializer_expression, AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return LinkedList<const Error*>::GetTerminator();
	}
}

const DeclarationStatement* PrimitiveDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	return new PrimitiveDeclarationStatement(GetPosition(), m_type,
			m_type_position, m_name, m_name_position, expression);
}

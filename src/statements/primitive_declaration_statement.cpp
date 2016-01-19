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
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position), m_type(type), m_type_position(
				type_position), m_name(name), m_name_position(name_position), m_initializer_expression(
				initializer_expression) {
}

PrimitiveDeclarationStatement::~PrimitiveDeclarationStatement() {
}

const ErrorListRef PrimitiveDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	auto symbol = Symbol::GetDefaultSymbol();
	const_shared_ptr<Expression> expression = m_initializer_expression;

	if (expression) {
		errors = expression->Validate(execution_context);
	}

	auto type_table = execution_context->GetTypeTable();

	auto as_primitive = std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
			m_type);

	if (as_primitive) {
		if (expression) {
			const_shared_ptr<TypeSpecifier> expression_type_specifier =
					expression->GetType(execution_context);

			auto expression_as_primitive = std::dynamic_pointer_cast<
					const PrimitiveTypeSpecifier>(expression_type_specifier);

			if (expression_as_primitive == nullptr
					|| !expression_as_primitive->IsAssignableTo(as_primitive)) {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::INVALID_INITIALIZER_TYPE,
										m_initializer_expression->GetPosition().begin.line,
										m_initializer_expression->GetPosition().begin.column,
										*m_name, as_primitive->ToString(),
										expression_type_specifier->ToString()),
								errors);
			}
		}

		auto value = m_type->DefaultValue(*type_table);
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case BOOLEAN: {
			symbol = const_shared_ptr<Symbol>(
					new Symbol(static_pointer_cast<const bool>(value)));
			break;
		}
		case INT: {
			symbol = const_shared_ptr<Symbol>(
					new Symbol(static_pointer_cast<const int>(value)));
			break;
		}
		case DOUBLE: {
			symbol = const_shared_ptr<Symbol>(
					new Symbol(static_pointer_cast<const double>(value)));
			break;
		}
		case STRING: {
			symbol = const_shared_ptr<Symbol>(
					new Symbol(static_pointer_cast<const string>(value)));
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
		volatile_shared_ptr<SymbolTable> symbol_table = static_pointer_cast<
				SymbolTable>(execution_context);

		InsertResult insert_result = symbol_table->InsertSymbol(*m_name,
				symbol);
		if (insert_result == SYMBOL_EXISTS) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::PREVIOUS_DECLARATION,
							m_name_position.begin.line,
							m_name_position.begin.column, *m_name), errors);
		}
	}

	return errors;
}

const ErrorListRef PrimitiveDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	if (m_initializer_expression) {
		Variable* temp_variable = new BasicVariable(m_name, m_name_position);
		auto errors = temp_variable->AssignValue(execution_context,
				m_initializer_expression, AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}
}

const DeclarationStatement* PrimitiveDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new PrimitiveDeclarationStatement(GetPosition(), m_type,
			m_type_position, m_name, m_name_position, expression);
}

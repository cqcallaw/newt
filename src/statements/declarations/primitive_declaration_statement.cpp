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
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
	assert(dynamic_pointer_cast<const PrimitiveTypeSpecifier>(type));
}

PrimitiveDeclarationStatement::~PrimitiveDeclarationStatement() {
}

const ErrorListRef PrimitiveDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	auto symbol = Symbol::GetDefaultSymbol();

	if (GetInitializerExpression()) {
		errors = GetInitializerExpression()->Validate(execution_context);
	}

	auto as_primitive = std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
			m_type);

	if (as_primitive) {
		if (GetInitializerExpression()) {
			const_shared_ptr<TypeSpecifier> expression_type_specifier =
					GetInitializerExpression()->GetType(execution_context);

			auto expression_as_primitive = std::dynamic_pointer_cast<
					const PrimitiveTypeSpecifier>(expression_type_specifier);

			if (expression_as_primitive == nullptr
					|| !expression_as_primitive->IsAssignableTo(as_primitive,
							execution_context->GetTypeTable())) {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::INVALID_INITIALIZER_TYPE,
										GetInitializerExpression()->GetPosition().begin.line,
										GetInitializerExpression()->GetPosition().begin.column,
										*GetName(), as_primitive->ToString(),
										expression_type_specifier->ToString()),
								errors);
			}
		}

		auto value = m_type->DefaultValue(*execution_context->GetTypeTable());
		symbol = as_primitive->GetSymbol(value,
				*execution_context->GetTypeTable());
	} else {
		assert(false);
	}

	if (symbol != Symbol::GetDefaultSymbol()) {
		InsertResult insert_result = execution_context->InsertSymbol(*GetName(),
				symbol);
		if (insert_result == SYMBOL_EXISTS) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::PREVIOUS_DECLARATION,
							GetNamePosition().begin.line,
							GetNamePosition().begin.column, *GetName()),
					errors);
		}
	}

	return errors;
}

const ErrorListRef PrimitiveDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	if (GetInitializerExpression()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNamePosition());
		auto errors = temp_variable->AssignValue(execution_context,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}
}

const DeclarationStatement* PrimitiveDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new PrimitiveDeclarationStatement(GetPosition(), m_type,
			m_type_position, GetName(), GetNamePosition(), expression);
}

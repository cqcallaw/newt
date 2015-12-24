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

#include <inferred_declaration_statement.h>
#include <assert.h>
#include <expression.h>
#include <primitive_declaration_statement.h>
#include <array_declaration_statement.h>
#include <struct_instantiation_statement.h>
#include <function_declaration_statement.h>
#include <function_type_specifier.h>
#include <function_expression.h>
#include <type_specifier.h>

InferredDeclarationStatement::InferredDeclarationStatement(
		const yy::location position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position), m_name(name), m_name_position(
				name_position), m_initializer_expression(initializer_expression) {
}

InferredDeclarationStatement::~InferredDeclarationStatement() {
}

const_shared_ptr<Expression> InferredDeclarationStatement::GetInitializerExpression() const {
	return m_initializer_expression;
}

const_shared_ptr<TypeSpecifier> InferredDeclarationStatement::GetType() const {
	assert(false);
	return PrimitiveTypeSpecifier::GetNone();
}

const ErrorList InferredDeclarationStatement::preprocess(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	const_shared_ptr<TypeSpecifier> expression_type =
			m_initializer_expression->GetType(execution_context);

	const_shared_ptr<Statement> temp_statement =
			expression_type->GetDeclarationStatement(GetPosition(),
					expression_type, m_initializer_expression->GetPosition(),
					m_name, m_name_position, m_initializer_expression);
	errors = temp_statement->preprocess(execution_context);
	return errors;
}

const ErrorList InferredDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	const_shared_ptr<TypeSpecifier> expression_type =
			m_initializer_expression->GetType(execution_context);

	const_shared_ptr<Statement> temp_statement =
			expression_type->GetDeclarationStatement(GetPosition(),
					expression_type, m_initializer_expression->GetPosition(),
					m_name, m_name_position, m_initializer_expression);
	errors = temp_statement->execute(execution_context);
	return errors;
}

const DeclarationStatement* InferredDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new InferredDeclarationStatement(GetPosition(), m_name,
			m_name_position, expression);
}

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
#include <complex_instantiation_statement.h>
#include <function_declaration_statement.h>
#include <function_type_specifier.h>
#include <function_expression.h>
#include <type_specifier.h>

InferredDeclarationStatement::InferredDeclarationStatement(
		const yy::location position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()) {
	assert(initializer_expression);
}

InferredDeclarationStatement::~InferredDeclarationStatement() {
}

const_shared_ptr<TypeSpecifier> InferredDeclarationStatement::GetType() const {
	if (GetInitializerExpression()->IsConstant()) {
		return GetInitializerExpression()->GetType(nullptr);
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const ErrorListRef InferredDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<TypeSpecifier> expression_type =
			GetInitializerExpression()->GetType(execution_context);

	if (expression_type != PrimitiveTypeSpecifier::GetNone()) {
		const_shared_ptr<Statement> temp_statement =
				expression_type->GetDeclarationStatement(GetPosition(),
						expression_type,
						GetInitializerExpression()->GetPosition(), GetName(),
						GetNamePosition(), GetInitializerExpression());
		errors = temp_statement->preprocess(execution_context);
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INFERRED_DECLARATION_FAILED,
						GetPosition().begin.line, GetPosition().begin.column),
				errors);
	}
	return errors;
}

const ErrorListRef InferredDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors(ErrorList::GetTerminator());

	const_shared_ptr<TypeSpecifier> expression_type =
			GetInitializerExpression()->GetType(execution_context);

	const_shared_ptr<Statement> temp_statement =
			expression_type->GetDeclarationStatement(GetPosition(),
					expression_type, GetInitializerExpression()->GetPosition(),
					GetName(), GetNamePosition(), GetInitializerExpression());
	errors = temp_statement->execute(execution_context);
	return errors;
}

const DeclarationStatement* InferredDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new InferredDeclarationStatement(GetPosition(), GetName(),
			GetNamePosition(), expression);
}

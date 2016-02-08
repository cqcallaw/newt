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

#include <nested_declaration_statement.h>
#include <sum_type.h>
#include <record_type.h>
#include <memory>
#include <nested_type_specifier.h>
#include <execution_context.h>
#include <member_definition.h>

NestedDeclarationStatement::NestedDeclarationStatement(
		const yy::location position, const_shared_ptr<NestedTypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
}

NestedDeclarationStatement::~NestedDeclarationStatement() {
}

const ErrorListRef NestedDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto parent_type =
			execution_context->GetTypeTable()->GetType<TypeDefinition>(
					m_type->GetParent()->GetTypeName());

	if (parent_type) {
		auto type_name = m_type->GetMemberName();
		auto member_type_specifier = parent_type->GetMemberType(*type_name);

		return member_type_specifier->GetDeclarationStatement(GetPosition(),
				member_type_specifier, GetTypePosition(), GetName(),
				GetNamePosition(), GetInitializerExpression())->preprocess(
				execution_context);
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						GetInitializerExpression()->GetPosition().begin.line,
						GetInitializerExpression()->GetPosition().begin.column,
						m_type->ToString()), errors);
	}

	return errors;
}

const ErrorListRef NestedDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto parent_type =
			execution_context->GetTypeTable()->GetType<TypeDefinition>(
					m_type->GetParent()->GetTypeName());

	if (parent_type) {
		auto type_name = m_type->GetMemberName();
		auto member_type_specifier = parent_type->GetMemberType(*type_name);

		return member_type_specifier->GetDeclarationStatement(GetPosition(),
				member_type_specifier, GetTypePosition(), GetName(),
				GetNamePosition(), GetInitializerExpression())->execute(
				execution_context);
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						GetInitializerExpression()->GetPosition().begin.line,
						GetInitializerExpression()->GetPosition().begin.column,
						m_type->ToString()), errors);
	}

	return errors;
}

const DeclarationStatement* NestedDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new NestedDeclarationStatement(GetPosition(), m_type,
			m_type_position, GetName(), GetNamePosition(), expression);
}

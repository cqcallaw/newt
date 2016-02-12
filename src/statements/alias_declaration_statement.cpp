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

#include <alias_declaration_statement.h>

AliasDeclarationStatement::AliasDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
}

AliasDeclarationStatement::~AliasDeclarationStatement() {
}

const ErrorListRef AliasDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
}

const ErrorListRef AliasDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
}

const DeclarationStatement* AliasDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new AliasDeclarationStatement(GetPosition(), m_type, m_type_position,
			GetName(), GetNamePosition(), expression);
}

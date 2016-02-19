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

#include <type_alias_declaration_statement.h>

TypeAliasDeclarationStatement::TypeAliasDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position) :
		DeclarationStatement(position, name, name_position,
				const_shared_ptr<Expression>(), ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
}

TypeAliasDeclarationStatement::~TypeAliasDeclarationStatement() {
}

const ErrorListRef TypeAliasDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	//no-op
	return ErrorList::GetTerminator();
}

const ErrorListRef TypeAliasDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	//no-op
	return ErrorList::GetTerminator();
}

const DeclarationStatement* TypeAliasDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	//no-op
	return new TypeAliasDeclarationStatement(GetPosition(), m_type,
			m_type_position, GetName(), GetNamePosition());
}

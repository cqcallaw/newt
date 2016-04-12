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

#ifndef STATEMENTS_DECLARATIONS_RECORD_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_RECORD_DECLARATION_STATEMENT_H_

#include <declaration_statement.h>
#include <statement.h>
#include <string>
#include <record_type_specifier.h>

class RecordDeclarationStatement: public DeclarationStatement {
public:
	RecordDeclarationStatement(const yy::location position,
			const_shared_ptr<RecordTypeSpecifier> type,
			const_shared_ptr<string> name, const yy::location name_location,
			DeclarationListRef member_declaration_list,
			const yy::location member_declaration_list_location,
			ModifierListRef modifier_list,
			const yy::location modifiers_location);
	virtual ~RecordDeclarationStatement();

	virtual const ErrorListRef preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const_shared_ptr<TypeSpecifier> GetType() const {
		return m_type;
	}

	DeclarationListRef GetMemberDeclarationList() const {
		return m_member_declaration_list;
	}

	const yy::location GetMemberDeclarationListLocation() const {
		return m_member_declaration_list_location;
	}

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const;

	virtual const_shared_ptr<RecordDeclarationStatement> WithModifiers(
			ModifierListRef modifiers,
			const yy::location modifiers_location) const;

private:
	DeclarationListRef m_member_declaration_list;
	const yy::location m_member_declaration_list_location;
	const_shared_ptr<RecordTypeSpecifier> m_type;
};

#endif /* STATEMENTS_DECLARATIONS_RECORD_DECLARATION_STATEMENT_H_ */

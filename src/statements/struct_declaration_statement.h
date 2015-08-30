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

#ifndef STATEMENTS_STRUCT_DECLARATION_STATEMENT_H_
#define STATEMENTS_STRUCT_DECLARATION_STATEMENT_H_

#include <statement.h>
#include <member_declaration_list.h>
#include <modifier_list.h>
#include <yyltype.h>
#include <string>

class StructDeclarationStatement: public Statement {
public:
	StructDeclarationStatement(const std::string* name,
			const YYLTYPE name_position,
			const MemberDeclarationList* member_declaration_list,
			const YYLTYPE member_declaration_list_position,
			const ModifierList* modifier_list,
			const YYLTYPE modifiers_location);
	virtual ~StructDeclarationStatement();

	const MemberDeclarationList* GetMemberDeclarationList() const {
		return m_member_declaration_list;
	}

	const YYLTYPE GetMemberDeclarationListPosition() const {
		return m_member_declaration_list_position;
	}

	const ModifierList* GetModifierList() const {
		return m_modifier_list;
	}

	const YYLTYPE GetModifiersLocation() const {
		return m_modifiers_location;
	}

	const std::string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

	virtual LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;

private:
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const MemberDeclarationList* m_member_declaration_list;
	const YYLTYPE m_member_declaration_list_position;
	const ModifierList* m_modifier_list;
	const YYLTYPE m_modifiers_location;
};

#endif /* STATEMENTS_STRUCT_DECLARATION_STATEMENT_H_ */

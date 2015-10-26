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

#include <declaration_list.h>
#include <statement.h>
#include <modifier_list.h>
#include <yyltype.h>
#include <string>
#include <declaration_statement.h>

class StructDeclarationStatement: public DeclarationStatement {
public:
	StructDeclarationStatement(const YYLTYPE position, const std::string* name,
			const YYLTYPE name_position,
			const DeclarationList* member_declaration_list,
			const YYLTYPE member_declaration_list_position,
			const ModifierList* modifier_list,
			const YYLTYPE modifiers_location);
	virtual ~StructDeclarationStatement();

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			ExecutionContext* execution_context) const;

	const DeclarationList* GetMemberDeclarationList() const {
		return m_member_declaration_list;
	}

	virtual const TypeSpecifier* GetType() const {
		return m_type_specifier;
	}

	virtual const std::string* GetName() const {
		return m_name;
	}

	virtual const Expression* GetInitializerExpression() const {
		return m_initializer_expression;
	}

	virtual const DeclarationStatement* WithInitializerExpression(
			const Expression* expression) const;

	const YYLTYPE GetMemberDeclarationListPosition() const {
		return m_member_declaration_list_position;
	}

	const ModifierList* GetModifierList() const {
		return m_modifier_list;
	}

	const YYLTYPE GetModifiersLocation() const {
		return m_modifiers_location;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

private:
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const DeclarationList* m_member_declaration_list;
	const YYLTYPE m_member_declaration_list_position;
	const ModifierList* m_modifier_list;
	const YYLTYPE m_modifiers_location;

	const TypeSpecifier* m_type_specifier;
	const Expression* m_initializer_expression;
};

#endif /* STATEMENTS_STRUCT_DECLARATION_STATEMENT_H_ */

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

#ifndef STATEMENTS_DECLARATIONS_DECLARATION_STATEMENT_H_
#define STATEMENTS_DECLARATIONS_DECLARATION_STATEMENT_H_

#include <statement.h>
#include <string>
#include <modifier.h>

class Expression;
class TypeSpecifier;

using namespace std;

class DeclarationStatement: public Statement {
public:
	DeclarationStatement(const yy::location location,
			const_shared_ptr<string> name, const yy::location name_location,
			const_shared_ptr<Expression> initializer_expression,
			ModifierListRef modifier_list,
			const yy::location modifier_list_location);
	virtual ~DeclarationStatement();

	virtual const DeclarationStatement* WithInitializerExpression(
			const_shared_ptr<Expression> expression) const = 0;

	const yy::location GetLocation() const {
		return m_location;
	}

	const_shared_ptr<string> GetName() const {
		return m_name;
	}

	const yy::location GetNameLocation() const {
		return m_name_location;
	}

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const = 0;

	virtual const yy::location GetTypeSpecifierLocation() const = 0;

	const_shared_ptr<Expression> GetInitializerExpression() const {
		return m_initializer_expression;
	}

	ModifierListRef GetModifierList() const {
		return m_modifier_list;
	}

	const yy::location GetModifierListLocation() const {
		return m_modifier_list_location;
	}

private:
	const yy::location m_location;
	const_shared_ptr<string> m_name;
	const yy::location m_name_location;
	const_shared_ptr<Expression> m_initializer_expression;
	ModifierListRef m_modifier_list;
	const yy::location m_modifier_list_location;
};

typedef const LinkedList<const DeclarationStatement, NO_DUPLICATES> DeclarationList;
typedef shared_ptr<DeclarationList> DeclarationListRef;

#endif /* STATEMENTS_DECLARATIONS_DECLARATION_STATEMENT_H_ */

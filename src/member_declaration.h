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

#ifndef MEMBER_DECLARATION_H_
#define MEMBER_DECLARATION_H_

#include <defaults.h>
#include <type.h>

class TypeSpecifier;
class Expression;

class MemberDeclaration {
public:
	MemberDeclaration(const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const std::string* name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression = nullptr,
			const yy::location initializer_expression_position =
					GetDefaultLocation());
	virtual ~MemberDeclaration();

	const_shared_ptr<Expression> GetInitializerExpression() const {
		return m_initializer_expression;
	}

	const yy::location GetInitializerExpressionPosition() const {
		return m_initializer_expression_position;
	}

	const std::string* GetName() const {
		return m_name;
	}

	const yy::location GetNamePosition() const {
		return m_name_position;
	}

	const_shared_ptr<TypeSpecifier> GetType() const {
		return m_type;
	}

	const yy::location GetTypePosition() const {
		return m_type_position;
	}

private:
	const_shared_ptr<TypeSpecifier> m_type;
	const yy::location m_type_position;
	const std::string* m_name;
	const yy::location m_name_position;
	const_shared_ptr<Expression> m_initializer_expression;
	const yy::location m_initializer_expression_position;
};

#endif /* MEMBER_DECLARATION_H_ */

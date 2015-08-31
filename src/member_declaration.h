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

class MemberDeclaration {
public:
	MemberDeclaration(const BasicType type, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* initializer_expression = nullptr,
			const YYLTYPE initializer_expression_position = DefaultLocation);
	virtual ~MemberDeclaration();

	const Expression* GetInitializerExpression() const {
		return m_initializer_expression;
	}

	const YYLTYPE GetInitializerExpressionPosition() const {
		return m_initializer_expression_position;
	}

	const std::string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

	const BasicType GetType() const {
		return m_type;
	}

	const YYLTYPE GetTypePosition() const {
		return m_type_position;
	}

private:
	const BasicType m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_initializer_expression;
	const YYLTYPE m_initializer_expression_position;
};

#endif /* MEMBER_DECLARATION_H_ */

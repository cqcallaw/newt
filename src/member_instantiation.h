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

#ifndef MEMBER_INSTANTIATION_H_
#define MEMBER_INSTANTIATION_H_

#include <defaults.h>
#include <type.h>

class MemberInstantiation {
public:
	MemberInstantiation(const std::string* name, const YYLTYPE name_position,
			const Expression* expression, const YYLTYPE expression_position);
	virtual ~MemberInstantiation();

	const Expression* GetExpression() const {
		return m_expression;
	}

	const YYLTYPE GetExpressionPosition() const {
		return m_expression_position;
	}

	const std::string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

private:
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_expression;
	const YYLTYPE m_expression_position;
};

#endif /* MEMBER_INSTANTIATION_H_ */

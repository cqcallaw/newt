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

class Expression;

class MemberInstantiation {
public:
	MemberInstantiation(const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> expression);
	virtual ~MemberInstantiation();

	const_shared_ptr<Expression> GetExpression() const {
		return m_expression;
	}

	const_shared_ptr<string> GetName() const {
		return m_name;
	}

	const yy::location GetNamePosition() const {
		return m_name_position;
	}

private:
	const_shared_ptr<string> m_name;
	const yy::location m_name_position;
	const_shared_ptr<Expression> m_expression;
};

typedef const LinkedList<const MemberInstantiation, NO_DUPLICATES> MemberInstantiationList;
typedef shared_ptr<MemberInstantiationList> MemberInstantiationListRef;

#endif /* MEMBER_INSTANTIATION_H_ */

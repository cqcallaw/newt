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

#ifndef INDEX_H_
#define INDEX_H_

class Expression;

class Index {
public:
	Index(const yy::location location,
			const_shared_ptr<Expression> index_expression) :
			m_location(location), m_index_expression(index_expression) {
	}

	virtual ~Index() {
	}

	const_shared_ptr<Expression> GetIndexExpression() const {
		return m_index_expression;
	}

	const yy::location GetLocation() const {
		return m_location;
	}

private:
	const yy::location m_location;
	const_shared_ptr<Expression> m_index_expression;
};

typedef const LinkedList<const Index, NO_DUPLICATES> IndexListBase;
typedef shared_ptr<IndexListBase> IndexList;

#endif /* INDEX_H_ */

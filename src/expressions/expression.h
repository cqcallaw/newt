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

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <symbol_table.h>
#include <symbol.h>
#include <array_symbol.h>
#include <utils.h>
#include <assert.h>
#include <type.h>

class Expression {
public:
	Expression(const Type type);
	virtual ~Expression();

	const Type GetType() const {
		return m_type;
	}

	virtual const void* Evaluate() const;

	const string* ToString() const;

private:
	const Type m_type;
};

#endif /* EXPRESSION_H_ */

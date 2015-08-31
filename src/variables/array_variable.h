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

#ifndef ARRAY_VARIABLE_H_
#define ARRAY_VARIABLE_H_

#include <string>
#include "variable.h"

class Expression;
class ExecutionContext;

class ArrayVariable: public Variable {
public:
	ArrayVariable(const std::string* name, YYLTYPE location,
			const Expression* index_expression, YYLTYPE expression_location);
	virtual ~ArrayVariable();

	const Expression* GetIndexExpression() const {
		return m_index_expression;
	}

	virtual const BasicType GetType(const ExecutionContext* context) const;

	virtual const std::string* ToString(const ExecutionContext* context) const;

	const YYLTYPE GetExpressionLocation() const {
		return m_expression_location;
	}

private:
	const Expression* m_index_expression;
	const YYLTYPE m_expression_location;
};

#endif /* ARRAY_VARIABLE_H_ */

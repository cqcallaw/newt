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

#ifndef ARITHMETIC_EXPRESSION_H_
#define ARITHMETIC_EXPRESSION_H_

#include "binary_expression.h"

class ArithmeticExpression: public BinaryExpression {
public:
	ArithmeticExpression(const YYLTYPE position, const OperatorType op, const Expression* left,
			const Expression* right);

protected:
	virtual const void* compute(bool left, bool right) const;
	virtual const void* compute(int left, int right) const;
	virtual const void* compute(double left, double right) const;
	virtual const void* compute(string* left, string* right) const;
};

#endif /* ARITHMETIC_EXPRESSION_H_ */

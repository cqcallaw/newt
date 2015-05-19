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

#ifndef STRING_CONCATENATION_EXPRESSION_H_
#define STRING_CONCATENATION_EXPRESSION_H_

#include "binary_expression.h"

class StringConcatenationExpression: public BinaryExpression {
public:
	StringConcatenationExpression(const Expression* left,
			const Expression* right);

protected:
	virtual const void* compute(bool left, bool right) const {
		assert(false);
		return NULL;
	}
	virtual const void* compute(int left, int right) const {
		assert(false);
		return NULL;
	}
	virtual const void* compute(double left, double right) const {
		assert(false);
		return NULL;
	}
	virtual const void* compute(string* left, string* right) const {
		ostringstream buffer;
		buffer << *left;
		buffer << *right;
		string* result = new string(buffer.str());
		return (void *) result;
	}
};

#endif /* STRING_CONCATENATION_EXPRESSION_H_ */

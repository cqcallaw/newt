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

#include "arithmetic_expression.h"

ArithmeticExpression::ArithmeticExpression(const OperatorType op,
		const Expression* left, const Expression* right) :
		BinaryExpression(op, left, right) {
	assert(
			op == PLUS || op == MINUS || op == MULTIPLY || op == DIVIDE
					|| op == MOD);
	Type left_type = left->GetType();
	Type right_type = right->GetType();
	if (left_type != NONE) {
		assert(left_type & (BOOLEAN | INT | DOUBLE));
	}
	if (right_type != NONE) {
		assert(right_type & (BOOLEAN | INT | DOUBLE));
	}
}

const void* ArithmeticExpression::compute(bool left, bool right) const {
	assert(false);
	return NULL;
}

const void* ArithmeticExpression::compute(int left, int right) const {
	int* result = new int;
	switch (GetOperator()) {
	case PLUS:
		*result = left + right;
		//cout << "Integer summation result: " << *result << "\n";
		break;
	case MULTIPLY:
		*result = left * right;
		//cout << "Integer multiplication result: " << *result << "\n";
		break;
	case DIVIDE:
		*result = left / right;
		break;
	case MINUS:
		*result = left - right;
		break;
	case MOD:
		*result = left % right;
		break;
	default:
		break;
	}

	return (void *) result;
}

const void* ArithmeticExpression::compute(double left, double right) const {
	double* result = new double;
	switch (GetOperator()) {
	case PLUS:
		*result = left + right;
		break;
	case MULTIPLY:
		*result = left * right;
		break;
	case DIVIDE:
		*result = left / right;
		break;
	case MINUS:
		*result = left - right;
		break;
	default:
		break;
	}

	return (void *) result;
}

const void* ArithmeticExpression::compute(string* left, string* right) const {
	assert(false);
	return NULL;
}

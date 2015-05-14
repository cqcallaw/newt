/*
 * arithmetic_expression.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
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

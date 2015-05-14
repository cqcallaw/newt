/*
 * logic_expression.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#include "logic_expression.h"

LogicExpression::LogicExpression(const OperatorType op, const Expression* left,
		const Expression* right) :
		BinaryExpression(op, left, right) {
	assert(op == OR || op == AND);
}

const void* LogicExpression::compute(bool left, bool right) const {
	switch (GetOperator()) {
	case OR:
		return (void *) new bool(left || right);
	case AND:
		return (void *) new bool(left && right);
	default:
		assert(false);
		return NULL;
	}
}

const void* LogicExpression::compute(int left, int right) const {
	switch (GetOperator()) {
	case OR:
		return (void *) new bool(left || right);
	case AND:
		return (void *) new bool(left && right);
	default:
		assert(false);
		return NULL;
	}
}

const void* LogicExpression::compute(double left, double right) const {
	switch (GetOperator()) {
	case OR:
		return (void *) new bool(left || right);
	case AND:
		return (void *) new bool(left && right);
	default:
		assert(false);
		return NULL;
	}
}

const void* LogicExpression::compute(string* left, string* right) const {
	assert(false);
	return NULL;
}

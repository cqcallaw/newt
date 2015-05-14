/*
 * comparison_expression.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#include "comparison_expression.h"

using namespace std;

ComparisonExpression::ComparisonExpression(const OperatorType op,
		const Expression* left, const Expression* right) :
		BinaryExpression(op, left, right) {
	assert(
			op
					& (NOT_EQUAL | LESS_THAN | LESS_THAN_EQUAL | GREATER_THAN
							| GREATER_THAN_EQUAL | TOUCHES | NEAR));
}

const void* ComparisonExpression::compute(bool left, bool right) const {
	switch (GetOperator()) {
	case EQUAL:
		return (void*) new bool(left == right);
		break;
	case NOT_EQUAL:
		return (void*) new bool(left != right);
		break;
	case LESS_THAN:
	case LESS_THAN_EQUAL:
	case GREATER_THAN:
	case GREATER_THAN_EQUAL:
	default:
		cerr << "Invalid boolean comparison operation '"
				<< operator_to_string(GetOperator()) << "'. Operands: " << left
				<< ", " << right << "\n";
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const void* ComparisonExpression::compute(int left, int right) const {
	switch (GetOperator()) {
	case EQUAL: {
		bool* result = new bool(left == right);
		//cout << "Integer comparison result: " << *result << "\n";
		return (void *) result;
		break;
	}
	case NOT_EQUAL:
		return (void*) new bool(left != right);
		break;
	case LESS_THAN:
		return (void*) new bool(left < right);
		break;
	case LESS_THAN_EQUAL:
		return (void*) new bool(left <= right);
		break;
	case GREATER_THAN:
		return (void*) new bool(left > right);
		break;
	case GREATER_THAN_EQUAL:
		return (void*) new bool(left >= right);
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const void* ComparisonExpression::compute(double left, double right) const {
	switch (GetOperator()) {
	case EQUAL:
		return (void*) new bool(left == right);
		break;
	case NOT_EQUAL:
		return (void*) new bool(left != right);
		break;
	case LESS_THAN:
		return (void*) new bool(left < right);
		break;
	case LESS_THAN_EQUAL:
		return (void*) new bool(left <= right);
		break;
	case GREATER_THAN:
		return (void*) new bool(left > right);
		break;
	case GREATER_THAN_EQUAL:
		return (void*) new bool(left >= right);
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const void* ComparisonExpression::compute(string* left, string* right) const {
	switch (GetOperator()) {
	case EQUAL:
		return (void*) new bool(*left == *right);
		break;
	case NOT_EQUAL:
		return (void*) new bool(*left != *right);
		break;
	case LESS_THAN:
		return (void*) new bool(*left < *right);
		break;
	case LESS_THAN_EQUAL:
		return (void*) new bool(*left <= *right);
		break;
	case GREATER_THAN:
		return (void*) new bool(*left > *right);
		break;
	case GREATER_THAN_EQUAL:
		return (void*) new bool(*left >= *right);
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

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

#include "comparison_expression.h"
#include "error.h"

using namespace std;

ComparisonExpression::ComparisonExpression(const YYLTYPE position,
		const OperatorType op, const Expression* left, const Expression* right) :
		BinaryExpression(position, op, left, right) {
	assert(
			op
					& (NOT_EQUAL | LESS_THAN | LESS_THAN_EQUAL | GREATER_THAN
							| GREATER_THAN_EQUAL));
}

const PrimitiveType ComparisonExpression::GetType(
		const ExecutionContext* execution_context) const {
	return BOOLEAN;
}

const LinkedList<const Error*>* ComparisonExpression::Validate(
		const ExecutionContext* execution_context) const {
	return BinaryExpression::Validate(execution_context,
			(BOOLEAN | INT | DOUBLE | STRING),
			(BOOLEAN | INT | DOUBLE | STRING));
}

const Result* ComparisonExpression::compute(bool left, bool right,
YYLTYPE left_position, YYLTYPE right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return new Result((void*) new bool(left == right),
				LinkedList<const Error*>::Terminator);
		break;
	case NOT_EQUAL:
		return new Result((void*) new bool(left != right),
				LinkedList<const Error*>::Terminator);
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

const Result* ComparisonExpression::compute(int left, int right,
YYLTYPE left_position, YYLTYPE right_position) const {
	switch (GetOperator()) {
	case EQUAL: {
		bool* result = new bool(left == right);
		//cout << "Integer comparison result: " << *result << "\n";
		return new Result((void *) result,
				LinkedList<const Error*>::Terminator);
		break;
	}
	case NOT_EQUAL:
		return new Result((void*) new bool(left != right),
				LinkedList<const Error*>::Terminator);
		break;
	case LESS_THAN:
		return new Result((void*) new bool(left < right),
				LinkedList<const Error*>::Terminator);
		break;
	case LESS_THAN_EQUAL:
		return new Result((void*) new bool(left <= right),
				LinkedList<const Error*>::Terminator);
		break;
	case GREATER_THAN:
		return new Result((void*) new bool(left > right),
				LinkedList<const Error*>::Terminator);
		break;
	case GREATER_THAN_EQUAL:
		return new Result((void*) new bool(left >= right),
				LinkedList<const Error*>::Terminator);
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const Result* ComparisonExpression::compute(double left, double right,
YYLTYPE left_position, YYLTYPE right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return new Result((void*) new bool(left == right),
				LinkedList<const Error*>::Terminator);
		break;
	case NOT_EQUAL:
		return new Result((void*) new bool(left != right),
				LinkedList<const Error*>::Terminator);
		break;
	case LESS_THAN:
		return new Result((void*) new bool(left < right),
				LinkedList<const Error*>::Terminator);
		break;
	case LESS_THAN_EQUAL:
		return new Result((void*) new bool(left <= right),
				LinkedList<const Error*>::Terminator);
		break;
	case GREATER_THAN:
		return new Result((void*) new bool(left > right),
				LinkedList<const Error*>::Terminator);
		break;
	case GREATER_THAN_EQUAL:
		return new Result((void*) new bool(left >= right),
				LinkedList<const Error*>::Terminator);
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const Result* ComparisonExpression::compute(string* left,
		string* right,
		YYLTYPE left_position, YYLTYPE right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return new Result((void*) new bool(*left == *right),
				LinkedList<const Error*>::Terminator);
		break;
	case NOT_EQUAL:
		return new Result((void*) new bool(*left != *right),
				LinkedList<const Error*>::Terminator);
		break;
	case LESS_THAN:
		return new Result((void*) new bool(*left < *right),
				LinkedList<const Error*>::Terminator);
		break;
	case LESS_THAN_EQUAL:
		return new Result((void*) new bool(*left <= *right),
				LinkedList<const Error*>::Terminator);
		break;
	case GREATER_THAN:
		return new Result((void*) new bool(*left > *right),
				LinkedList<const Error*>::Terminator);
		break;
	case GREATER_THAN_EQUAL:
		return new Result((void*) new bool(*left >= *right),
				LinkedList<const Error*>::Terminator);
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

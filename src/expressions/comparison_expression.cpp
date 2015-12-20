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
#include <memory>

using namespace std;

ComparisonExpression::ComparisonExpression(const yy::location position,
		const OperatorType op, const Expression* left, const Expression* right) :
		BinaryExpression(position, op, left, right) {
	assert(
			op
					& (NOT_EQUAL | LESS_THAN | LESS_THAN_EQUAL | GREATER_THAN
							| GREATER_THAN_EQUAL));
}

const_shared_ptr<TypeSpecifier> ComparisonExpression::GetType(
		const ExecutionContext* execution_context) const {
	return PrimitiveTypeSpecifier::GetBoolean();
}

const LinkedList<const Error*>* ComparisonExpression::Validate(
		const ExecutionContext* execution_context) const {
	return BinaryExpression::Validate(execution_context,
			PrimitiveTypeSpecifier::GetString(),
			PrimitiveTypeSpecifier::GetString());
}

const Result* ComparisonExpression::compute(bool& left, bool& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left == right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case NOT_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left != right)),
				LinkedList<const Error*>::GetTerminator());
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

const Result* ComparisonExpression::compute(int& left, int& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL: {
		return new Result(const_shared_ptr<const void>(new bool(left == right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	}
	case NOT_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left != right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case LESS_THAN:
		return new Result(const_shared_ptr<const void>(new bool(left < right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case LESS_THAN_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left <= right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case GREATER_THAN:
		return new Result(const_shared_ptr<const void>(new bool(left > right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case GREATER_THAN_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left >= right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const Result* ComparisonExpression::compute(double& left, double& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL: {
		return new Result(const_shared_ptr<const void>(new bool(left == right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	}
	case NOT_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left != right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case LESS_THAN:
		return new Result(const_shared_ptr<const void>(new bool(left < right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case LESS_THAN_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left <= right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case GREATER_THAN:
		return new Result(const_shared_ptr<const void>(new bool(left > right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case GREATER_THAN_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left >= right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const Result* ComparisonExpression::compute(string& left, string& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL: {
		return new Result(const_shared_ptr<const void>(new bool(left == right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	}
	case NOT_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left != right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case LESS_THAN:
		return new Result(const_shared_ptr<const void>(new bool(left < right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case LESS_THAN_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left <= right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case GREATER_THAN:
		return new Result(const_shared_ptr<const void>(new bool(left > right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	case GREATER_THAN_EQUAL:
		return new Result(const_shared_ptr<const void>(new bool(left >= right)),
				LinkedList<const Error*>::GetTerminator());
		break;
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

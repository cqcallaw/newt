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
		const OperatorType op, const_shared_ptr<Expression> left,
		const_shared_ptr<Expression> right) :
		BinaryExpression(position, op, left, right) {
	assert(
			op
					& (NOT_EQUAL | LESS_THAN | LESS_THAN_EQUAL | GREATER_THAN
							| GREATER_THAN_EQUAL));
}

const_shared_ptr<TypeSpecifier> ComparisonExpression::GetType(
		const_shared_ptr<ExecutionContext> execution_context) const {
	return PrimitiveTypeSpecifier::GetBoolean();
}

const ErrorListRef ComparisonExpression::Validate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	return BinaryExpression::Validate(execution_context,
			PrimitiveTypeSpecifier::GetString(),
			PrimitiveTypeSpecifier::GetString());
}

const_shared_ptr<Result> ComparisonExpression::compute(const bool& left,
		const bool& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left == right)),
				ErrorList::GetTerminator());
		break;
	case NOT_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left != right)),
				ErrorList::GetTerminator());
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

const_shared_ptr<Result> ComparisonExpression::compute(const int& left,
		const int& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL: {
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left == right)),
				ErrorList::GetTerminator());
	}
	case NOT_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left != right)),
				ErrorList::GetTerminator());
	case LESS_THAN:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left < right)),
				ErrorList::GetTerminator());
	case LESS_THAN_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left <= right)),
				ErrorList::GetTerminator());
	case GREATER_THAN:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left > right)),
				ErrorList::GetTerminator());
	case GREATER_THAN_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left >= right)),
				ErrorList::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const_shared_ptr<Result> ComparisonExpression::compute(const double& left,
		const double& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left == right)),
				ErrorList::GetTerminator());
	case NOT_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left != right)),
				ErrorList::GetTerminator());
	case LESS_THAN:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left < right)),
				ErrorList::GetTerminator());
	case LESS_THAN_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left <= right)),
				ErrorList::GetTerminator());
	case GREATER_THAN:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left > right)),
				ErrorList::GetTerminator());
	case GREATER_THAN_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left >= right)),
				ErrorList::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

const_shared_ptr<Result> ComparisonExpression::compute(const string& left,
		const string& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left == right)),
				ErrorList::GetTerminator());
	case NOT_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left != right)),
				ErrorList::GetTerminator());
	case LESS_THAN:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left < right)),
				ErrorList::GetTerminator());
	case LESS_THAN_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left <= right)),
				ErrorList::GetTerminator());
	case GREATER_THAN:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left > right)),
				ErrorList::GetTerminator());
	case GREATER_THAN_EQUAL:
		return make_shared<Result>(
				const_shared_ptr<const void>(new bool(left >= right)),
				ErrorList::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
	assert(false);
	return NULL;
}

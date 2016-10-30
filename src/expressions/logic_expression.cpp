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

#include "logic_expression.h"
#include "error.h"

LogicExpression::LogicExpression(const yy::location position,
		const OperatorType op, const_shared_ptr<Expression> left,
		const_shared_ptr<Expression> right) :
		BinaryExpression(position, op, left, right) {
	assert(op == OR || op == AND);
}

TypedResult<TypeSpecifier> LogicExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return TypedResult<TypeSpecifier>(PrimitiveTypeSpecifier::GetBoolean(),
			ErrorList::GetTerminator());
}

const ErrorListRef LogicExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	return BinaryExpression::Validate(execution_context,
			PrimitiveTypeSpecifier::GetDouble(),
			PrimitiveTypeSpecifier::GetDouble());
}

const_shared_ptr<Result> LogicExpression::compute(const bool& left,
		const bool& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case OR:
		return make_shared<Result>(
				const_shared_ptr<void>(new bool(left || right)),
				ErrorList::GetTerminator());
	case AND:
		return make_shared<Result>(
				const_shared_ptr<void>(new bool(left && right)),
				ErrorList::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
}

const_shared_ptr<Result> LogicExpression::compute(const int& left,
		const int& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case OR: {
		bool result = left || right;
		return make_shared<Result>(const_shared_ptr<void>(new bool(result)),
				ErrorList::GetTerminator());
	}
	case AND: {
		bool result = left && right;
		return make_shared<Result>(const_shared_ptr<void>(new bool(result)),
				ErrorList::GetTerminator());
	}
	default:
		assert(false);
		return NULL;
	}
}

const_shared_ptr<Result> LogicExpression::compute(const double& left,
		const double& right, yy::location left_position,
		yy::location right_position) const {
	switch (GetOperator()) {
	case OR:
		return make_shared<Result>(
				const_shared_ptr<void>(new bool(left || right)),
				ErrorList::GetTerminator());
	case AND:
		return make_shared<Result>(
				const_shared_ptr<void>(new bool(left && right)),
				ErrorList::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
}

const_shared_ptr<Result> LogicExpression::compute(const string& left,
		const string& right, yy::location left_position,
		yy::location right_position) const {
	assert(false);
	return nullptr;
}

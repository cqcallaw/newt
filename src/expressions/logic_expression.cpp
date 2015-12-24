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

const_shared_ptr<TypeSpecifier> LogicExpression::GetType(
		const_shared_ptr<ExecutionContext> execution_context) const {
	return PrimitiveTypeSpecifier::GetBoolean();
}

const ErrorList LogicExpression::Validate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	return BinaryExpression::Validate(execution_context,
			PrimitiveTypeSpecifier::GetDouble(),
			PrimitiveTypeSpecifier::GetDouble());
}

const Result* LogicExpression::compute(bool& left, bool& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case OR:
		return new Result(const_shared_ptr<void>(new bool(left || right)),
				ErrorListBase::GetTerminator());
	case AND:
		return new Result(const_shared_ptr<void>(new bool(left && right)),
				ErrorListBase::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
}

const Result* LogicExpression::compute(int& left, int& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case OR: {
		bool result = left || right;
		return new Result(const_shared_ptr<void>(new bool(result)),
				ErrorListBase::GetTerminator());
	}
	case AND: {
		bool result = left && right;
		return new Result(const_shared_ptr<void>(new bool(result)),
				ErrorListBase::GetTerminator());
	}
	default:
		assert(false);
		return NULL;
	}
}

const Result* LogicExpression::compute(double& left, double& right,
		yy::location left_position, yy::location right_position) const {
	switch (GetOperator()) {
	case OR:
		return new Result(const_shared_ptr<void>(new bool(left || right)),
				ErrorListBase::GetTerminator());
	case AND:
		return new Result(const_shared_ptr<void>(new bool(left && right)),
				ErrorListBase::GetTerminator());
	default:
		assert(false);
		return NULL;
	}
}

const Result* LogicExpression::compute(string& left, string& right,
		yy::location left_position, yy::location right_position) const {
	assert(false);
	return nullptr;
}

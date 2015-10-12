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

LogicExpression::LogicExpression(const YYLTYPE position, const OperatorType op,
		const Expression* left, const Expression* right) :
		BinaryExpression(position, op, left, right) {
	assert(op == OR || op == AND);
}

const TypeSpecifier* LogicExpression::GetType(
		const ExecutionContext* execution_context) const {
	return PrimitiveTypeSpecifier::GetBoolean();
}

const LinkedList<const Error*>* LogicExpression::Validate(
		const ExecutionContext* execution_context) const {
	return BinaryExpression::Validate(execution_context,
			PrimitiveTypeSpecifier::GetDouble(), PrimitiveTypeSpecifier::GetDouble());
}

const Result* LogicExpression::compute(bool left, bool right,
YYLTYPE left_position, YYLTYPE right_position) const {
	switch (GetOperator()) {
	case OR:
		return new Result((void *) new bool(left || right),
				LinkedList<const Error*>::Terminator);
	case AND:
		return new Result((void *) new bool(left && right),
				LinkedList<const Error*>::Terminator);
	default:
		assert(false);
		return NULL;
	}
}

const Result* LogicExpression::compute(int left, int right,
YYLTYPE left_position,
YYLTYPE right_position) const {
	switch (GetOperator()) {
	case OR: {
		bool result = left || right;
		return new Result((void *) new bool(result),
				LinkedList<const Error*>::Terminator);
	}
	case AND: {
		bool result = left && right;
		return new Result((void *) new bool(result),
				LinkedList<const Error*>::Terminator);
	}
	default:
		assert(false);
		return NULL;
	}
}

const Result* LogicExpression::compute(double left, double right,
YYLTYPE left_position, YYLTYPE right_position) const {
	switch (GetOperator()) {
	case OR:
		return new Result((void *) new bool(left || right),
				LinkedList<const Error*>::Terminator);
	case AND:
		return new Result((void *) new bool(left && right),
				LinkedList<const Error*>::Terminator);
	default:
		assert(false);
		return NULL;
	}
}

const Result* LogicExpression::compute(string* left, string* right,
YYLTYPE left_position, YYLTYPE right_position) const {
	assert(false);
	return NULL;
}

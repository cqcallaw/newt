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

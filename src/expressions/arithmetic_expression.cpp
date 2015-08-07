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

#include "arithmetic_expression.h"
#include <error.h>
#include <sstream>
#include <type.h>

ArithmeticExpression::ArithmeticExpression(const YYLTYPE position,
		const OperatorType op, const Expression* left, const Expression* right) :
		BinaryExpression(position, op, left, right) {
	assert(
			op == PLUS || op == MINUS || op == MULTIPLY || op == DIVIDE
					|| op == MOD);
}

const EvaluationResult* ArithmeticExpression::compute(bool left, bool right,
YYLTYPE left_position, YYLTYPE right_position) const {
	assert(false);
	return NULL;
}

const EvaluationResult* ArithmeticExpression::compute(int left, int right,
YYLTYPE left_position, YYLTYPE right_position) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;

	int* result = new int;
	switch (GetOperator()) {
	case PLUS:
		*result = left + right;
		break;
	case MULTIPLY:
		*result = left * right;
		break;
	case DIVIDE:
		if (right == 0) {
			errors = (LinkedList<const Error*>*) errors->With(
					new Error(Error::SEMANTIC, Error::DIVIDE_BY_ZERO,
							right_position.first_line,
							right_position.first_column));
			*result = 0;
		} else {
			*result = left / right;
		}
		break;
	case MINUS:
		*result = left - right;
		break;
	case MOD:
		if (right == 0) {
			errors = (LinkedList<const Error*>*) errors->With(
					new Error(Error::SEMANTIC, Error::MOD_BY_ZERO,
							right_position.first_line,
							right_position.first_column));
			*result = 0;
		} else {
			*result = left % right;
		}
		break;
	default:
		break;
	}

	return new EvaluationResult((void *) result, errors);
}

const EvaluationResult* ArithmeticExpression::compute(double left, double right,
YYLTYPE left_position, YYLTYPE right_position) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;

	double* result = new double;
	switch (GetOperator()) {
	case PLUS:
		*result = left + right;
		break;
	case MULTIPLY:
		*result = left * right;
		break;
	case DIVIDE:
		if (right == 0.0) {
			errors = (LinkedList<const Error*>*) errors->With(
					new Error(Error::SEMANTIC, Error::DIVIDE_BY_ZERO,
							right_position.first_line,
							right_position.first_column));
			*result = 0;
		} else {
			*result = left / right;
		}
		break;
	case MINUS:
		*result = left - right;
		break;
	default:
		break;
	}

	return new EvaluationResult((void *) result, errors);
}

const EvaluationResult* ArithmeticExpression::compute(string* left,
		string* right, YYLTYPE left_position, YYLTYPE right_position) const {
	//string concatenation isn't strictly an arithmetic operation, so this is a hack
	std::ostringstream buffer;
	buffer << *left;
	buffer << *right;
	string* result = new string(buffer.str());
	return new EvaluationResult((void *) result,
			LinkedList<const Error*>::Terminator);
}

const LinkedList<const Error*>* ArithmeticExpression::Validate(
		const ExecutionContext* execution_context) const {
	if (GetOperator() == PLUS) {
		//Allow STRING types because PLUS doubles as a concatenation operator
		return BinaryExpression::Validate(execution_context,
				(BOOLEAN | INT | DOUBLE | STRING),
				(BOOLEAN | INT | DOUBLE | STRING));
	} else if (GetOperator() == MOD) {
		return BinaryExpression::Validate(execution_context, (BOOLEAN | INT),
				(BOOLEAN | INT));
	} else {
		return BinaryExpression::Validate(execution_context,
				(BOOLEAN | INT | DOUBLE), (BOOLEAN | INT | DOUBLE));
	}
}

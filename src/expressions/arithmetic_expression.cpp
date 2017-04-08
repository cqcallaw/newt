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

ArithmeticExpression::ArithmeticExpression(const yy::location position,
		const OperatorType op, const_shared_ptr<Expression> left,
		const_shared_ptr<Expression> right) :
		BinaryExpression(position, op, left, right) {
	assert(
			op == PLUS || op == MINUS || op == MULTIPLY || op == DIVIDE
					|| op == MOD);
}

const_shared_ptr<Result> ArithmeticExpression::compute(const bool& left,
		const bool& right, yy::location left_position,
		yy::location right_position) const {
	assert(false);
	return NULL;
}

const_shared_ptr<Result> ArithmeticExpression::compute(const int& left,
		const int& right, yy::location left_position,
		yy::location right_position) const {
	ErrorListRef errors = ErrorList::GetTerminator();

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
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::DIVIDE_BY_ZERO,
							right_position.begin.line,
							right_position.begin.column), errors);
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
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::MOD_BY_ZERO,
							right_position.begin.line,
							right_position.begin.column), errors);
			*result = 0;
		} else {
			*result = left % right;
		}
		break;
	default:
		break;
	}

	return make_shared<Result>(const_shared_ptr<const void>(result), errors);
}

const_shared_ptr<Result> ArithmeticExpression::compute(const double& left,
		const double& right, yy::location left_position,
		yy::location right_position) const {
	ErrorListRef errors = ErrorList::GetTerminator();

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
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::DIVIDE_BY_ZERO,
							right_position.begin.line,
							right_position.begin.column), errors);
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

	return make_shared<Result>(const_shared_ptr<const void>(result), errors);
}

const_shared_ptr<Result> ArithmeticExpression::compute(const std::uint8_t& left,
		const std::uint8_t& right, yy::location left_position,
		yy::location right_position) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	uint8_t* result = new uint8_t;
	switch (GetOperator()) {
	case PLUS:
		*result = left + right;
		break;
	case MULTIPLY:
		*result = left * right;
		break;
	case DIVIDE:
		if (right == 0.0) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::DIVIDE_BY_ZERO,
							right_position.begin.line,
							right_position.begin.column), errors);
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

	return make_shared<Result>(const_shared_ptr<const void>(result), errors);
}

const_shared_ptr<Result> ArithmeticExpression::compute(const string& left,
		const string& right, yy::location left_position,
		yy::location right_position) const {
	//string concatenation isn't strictly an arithmetic operation, so this is a hack
	std::ostringstream buffer;
	buffer << left;
	buffer << right;
	const string buffer_string = buffer.str();
	const string* result = new string(buffer_string);
	return make_shared<Result>(const_shared_ptr<const void>(result),
			ErrorList::GetTerminator());
}

const ErrorListRef ArithmeticExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	if (GetOperator() == PLUS) {
		//Allow STRING types because PLUS doubles as a concatenation operator
		return BinaryExpression::Validate(execution_context,
				PrimitiveTypeSpecifier::GetString(),
				PrimitiveTypeSpecifier::GetString());
	} else if (GetOperator() == MOD) {
		return BinaryExpression::Validate(execution_context,
				PrimitiveTypeSpecifier::GetInt(),
				PrimitiveTypeSpecifier::GetInt());
	} else {
		return BinaryExpression::Validate(execution_context,
				PrimitiveTypeSpecifier::GetDouble(),
				PrimitiveTypeSpecifier::GetDouble());
	}
}

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

#ifndef LOGIC_EXPRESSION_H_
#define LOGIC_EXPRESSION_H_

#include "binary_expression.h"

class LogicExpression: public BinaryExpression {
public:
	LogicExpression(const yy::location position, const OperatorType op,
			const_shared_ptr<Expression> left, const_shared_ptr<Expression> right);

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorList Validate(
			const_shared_ptr<ExecutionContext> execution_context) const;

protected:
	virtual const Result* compute(bool& left, bool& right,
			yy::location left_position, yy::location right_position) const;
	virtual const Result* compute(int& left, int& right,
			yy::location left_position, yy::location right_position) const;
	virtual const Result* compute(double& left, double& right,
			yy::location left_position, yy::location right_position) const;
	virtual const Result* compute(string& left, string& right,
			yy::location left_position, yy::location right_position) const;
};
#endif /* LOGIC_EXPRESSION_H_ */

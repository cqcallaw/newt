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
	LogicExpression(const YYLTYPE position, const OperatorType op,
			const Expression* left, const Expression* right);

	virtual const Type GetType(const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context) const;

protected:
	virtual const EvaluationResult* compute(bool left, bool right,
			YYLTYPE left_position,
			YYLTYPE right_position) const;
	virtual const EvaluationResult* compute(int left, int right,
			YYLTYPE left_position,
			YYLTYPE right_position) const;
	virtual const EvaluationResult* compute(double left, double right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	virtual const EvaluationResult* compute(string* left, string* right,
	YYLTYPE left_position, YYLTYPE right_position) const;
};
#endif /* LOGIC_EXPRESSION_H_ */

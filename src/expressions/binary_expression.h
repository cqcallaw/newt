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

#ifndef BINARY_EXPRESSION_H_
#define BINARY_EXPRESSION_H_

#include "expression.h"

class Error;

class BinaryExpression: public Expression {
public:
	BinaryExpression(const YYLTYPE position, const OperatorType op,
			const Expression* left, const Expression* right);

	virtual const Type GetType(const ExecutionContext* execution_context) const;

	const EvaluationResult* Evaluate(
			const ExecutionContext* execution_context) const;

	const Expression* GetLeft() const {
		return m_left;
	}
	const OperatorType GetOperator() const {
		return m_operator;
	}
	const Expression* GetRight() const {
		return m_right;
	}

	const static Type ComputeResultType(const Expression* left,
			const Expression* right, const OperatorType op,
			const ExecutionContext* execution_context);

	virtual const bool IsConstant() const {
		return m_left->IsConstant() && m_right->IsConstant();
	}

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context, int valid_left,
			int valid_right) const;

protected:
	virtual const EvaluationResult* compute(bool left, bool right,
	YYLTYPE left_position, YYLTYPE right_position) const = 0;
	const EvaluationResult* compute(bool left, int right, YYLTYPE left_position,
	YYLTYPE right_position) const;
	const EvaluationResult* compute(bool left, double right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	const EvaluationResult* compute(bool left, string* right,
	YYLTYPE left_position, YYLTYPE right_position) const;

	const EvaluationResult* compute(int left, bool right, YYLTYPE left_position,
	YYLTYPE right_position) const;
	virtual const EvaluationResult* compute(int left, int right,
	YYLTYPE left_position, YYLTYPE right_position) const = 0;
	const EvaluationResult* compute(int left, double right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	const EvaluationResult* compute(int left, string* right,
	YYLTYPE left_position, YYLTYPE right_position) const;

	const EvaluationResult* compute(double left, bool right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	const EvaluationResult* compute(double left, int right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	virtual const EvaluationResult* compute(double left, double right,
	YYLTYPE left_position, YYLTYPE right_position) const = 0;
	const EvaluationResult* compute(double left, string* right,
	YYLTYPE left_position, YYLTYPE right_position) const;

	const EvaluationResult* compute(string* left, bool right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	const EvaluationResult* compute(string* left, int right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	const EvaluationResult* compute(string* left, double right,
	YYLTYPE left_position, YYLTYPE right_position) const;
	virtual const EvaluationResult* compute(string* left, string* right,
	YYLTYPE left_position, YYLTYPE right_position) const = 0;

private:
	const OperatorType m_operator;
	const Expression* m_left;
	const Expression* m_right;
}
;

#endif /* BINARY_EXPRESSION_H_ */

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

#ifndef UNARY_EXPRESSION_H_
#define UNARY_EXPRESSION_H_

#include "expression.h"

class UnaryExpression: public Expression {
public:
	UnaryExpression(const YYLTYPE position, const OperatorType op,
			const Expression* expression);

	virtual const Type GetType(const ExecutionContext* execution_context) const;

	virtual const Result* Evaluate(
			const ExecutionContext* execution_context) const;

	virtual const bool IsConstant() const {
		return m_operator == UNARY_MINUS && m_expression->IsConstant();
	}

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context) const;

private:
	const static Type compute_result_type(const Type input_type,
			const OperatorType op);
	const static void* compute(const Type input_type, const void* input,
			double (*compute_function)(double),
			double (*input_transform_function)(double) = NULL,
			double (*output_transform_function)(double) = NULL);
	static double degrees_to_radians(double angle);
	static double radians_to_degrees(double radians);
	const Expression* m_expression;
	const OperatorType m_operator;
};

#endif /* UNARY_EXPRESSION_H_ */

/*
 * unary_expression.h
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#ifndef UNARY_EXPRESSION_H_
#define UNARY_EXPRESSION_H_

#include "expression.h"

class UnaryExpression: public Expression {
public:
	UnaryExpression(const OperatorType op, const Expression* expression);

	virtual const void* Evaluate() const;

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

/*
 * constant_expression.h
 *
 *  Created on: May 16, 2015
 *      Author: caleb
 */

#ifndef EXPRESSIONS_CONSTANT_EXPRESSION_H_
#define EXPRESSIONS_CONSTANT_EXPRESSION_H_

#include <string>

#include "expression.h"

class ConstantExpression: public Expression {
public:
	ConstantExpression(const bool value);
	ConstantExpression(const int value);
	ConstantExpression(const double value);
	ConstantExpression(const std::string* value);

	virtual const void* Evaluate() const;

private:
	const void* m_value;
};

#endif /* EXPRESSIONS_CONSTANT_EXPRESSION_H_ */

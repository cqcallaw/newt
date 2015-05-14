/*
 * logic_expression.h
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#ifndef LOGIC_EXPRESSION_H_
#define LOGIC_EXPRESSION_H_

#include "binary_expression.h"

class LogicExpression: public BinaryExpression {
public:
	LogicExpression(const OperatorType op, const Expression* left,
			const Expression* right);

protected:
	virtual const void* compute(bool left, bool right) const;
	virtual const void* compute(int left, int right) const;
	virtual const void* compute(double left, double right) const;
	virtual const void* compute(string* left, string* right) const;
};
#endif /* LOGIC_EXPRESSION_H_ */

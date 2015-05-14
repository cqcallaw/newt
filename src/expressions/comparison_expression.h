/*
 * comparison_expression.h
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#ifndef COMPARISON_EXPRESSION_H_
#define COMPARISON_EXPRESSION_H_

#include "binary_expression.h"

using namespace std;

class ComparisonExpression: public BinaryExpression {
public:
	ComparisonExpression(const OperatorType op, const Expression* left,
			const Expression* right);

protected:
	virtual const void* compute(bool left, bool right) const;
	virtual const void* compute(int left, int right) const;
	virtual const void* compute(double left, double right) const;
	virtual const void* compute(string* left, string* right) const;
};

#endif /* COMPARISON_EXPRESSION_H_ */

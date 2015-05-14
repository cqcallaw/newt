/*
 * variable_expression.h
 *
 *  Created on: Apr 18, 2015
 *      Author: caleb
 */

#ifndef VARIABLE_EXPRESSION_H_
#define VARIABLE_EXPRESSION_H_

#include "expression.h"
class Variable;

class VariableExpression: public Expression {
public:
	VariableExpression(const Variable* variable);

	virtual const void* Evaluate() const;

private:
	const Variable* m_variable;
};

#endif /* VARIABLE_EXPRESSION_H_ */

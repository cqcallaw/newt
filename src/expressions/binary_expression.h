/*
 * binary_expression.h
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#ifndef BINARY_EXPRESSION_H_
#define BINARY_EXPRESSION_H_

#include "expression.h"

class BinaryExpression: public Expression {
public:
	BinaryExpression(const OperatorType op, const Expression* left,
			const Expression* right);

	const void* Evaluate() const;

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
			const Expression* right, const OperatorType op);

protected:
	virtual const void* compute(bool left, bool right) const = 0;
	const void* compute(bool left, int right) const;
	const void* compute(bool left, double right) const;
	const void* compute(bool left, string* right) const;

	const void* compute(int left, bool right) const;
	virtual const void* compute(int left, int right) const = 0;
	const void* compute(int left, double right) const;
	const void* compute(int left, string* right) const;

	const void* compute(double left, bool right) const;
	const void* compute(double left, int right) const;
	virtual const void* compute(double left, double right) const = 0;
	const void* compute(double left, string* right) const;

	const void* compute(string* left, bool right) const;
	const void* compute(string* left, int right) const;
	const void* compute(string* left, double right) const;
	virtual const void* compute(string* left, string* right) const = 0;

private:
	const OperatorType m_operator;
	const Expression* m_left;
	const Expression* m_right;
};

#endif /* BINARY_EXPRESSION_H_ */

/*
 * string_concatenation_expression.h
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#ifndef STRING_CONCATENATION_EXPRESSION_H_
#define STRING_CONCATENATION_EXPRESSION_H_

#include "binary_expression.h"

class StringConcatenationExpression: public BinaryExpression {
public:
	StringConcatenationExpression(const Expression* left,
			const Expression* right);

protected:
	virtual const void* compute(bool left, bool right) const {
		assert(false);
		return NULL;
	}
	virtual const void* compute(int left, int right) const {
		assert(false);
		return NULL;
	}
	virtual const void* compute(double left, double right) const {
		assert(false);
		return NULL;
	}
	virtual const void* compute(string* left, string* right) const {
		ostringstream buffer;
		buffer << *left;
		buffer << *right;
		string* result = new string(buffer.str());
		return (void *) result;
	}
};

#endif /* STRING_CONCATENATION_EXPRESSION_H_ */

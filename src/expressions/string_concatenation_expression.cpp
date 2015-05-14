/*
 * string_concatenation_expression.cpp
 *
 *  Created on: Feb 20, 2015
 *      Author: caleb
 */

#include <sstream>
#include "string_concatenation_expression.h"

StringConcatenationExpression::StringConcatenationExpression(
		const Expression* left, const Expression* right) :
		BinaryExpression(PLUS, left, right) {
	assert(left != NULL);
	if (left->GetType() != NONE) {
		assert(left->GetType() & (INT | DOUBLE | STRING));
	}

	assert(right != NULL);
	if (right->GetType() != NONE) {
		assert(right->GetType() & (INT | DOUBLE | STRING));
	}
}

/*
 * constant_expression.cpp
 *
 *  Created on: May 16, 2015
 *      Author: caleb
 */

#include "constant_expression.h"

ConstantExpression::ConstantExpression(const bool value) :
		Expression(BOOLEAN) {
	bool* heap_value = new bool;
	*heap_value = value;
	m_value = (void*) heap_value;
}

ConstantExpression::ConstantExpression(const int value) :
		Expression(INT) {
	int* heap_value = new int;
	*heap_value = value;
	m_value = (void*) heap_value;
}

ConstantExpression::ConstantExpression(const double value) :
		Expression(DOUBLE) {
	double* heap_value = new double;
	*heap_value = value;
	m_value = (void*) heap_value;
}

ConstantExpression::ConstantExpression(const std::string* value) :
		Expression(STRING) {
	m_value = (void*) value;
}

const void* ConstantExpression::Evaluate() const {
	return m_value;
}

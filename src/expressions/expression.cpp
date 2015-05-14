/*
 * expression.cpp
 *
 *  Created on: Feb 16, 2015
 *      Author: caleb
 */

#include <sstream>
#include "expression.h"
#include "error.h"
#include "variable.h"
#include "utils.h"

Expression::Expression(Type type) :
		m_type(type) {
}

Expression::~Expression() {
}

const void* Expression::Evaluate() const {
	return NULL;
}

const string* Expression::ToString() const {
	ostringstream buffer;
	switch (m_type) {
	case BOOLEAN:
		buffer << *((bool*) Evaluate());
		break;
	case INT:
		buffer << *((int*) Evaluate());
		break;
	case DOUBLE:
		buffer << *((double*) Evaluate());
		break;
	case STRING:
		buffer << *((string*) Evaluate());
		break;
	default:
		assert(false);
		return NULL;
	}

	return new string(buffer.str());
}

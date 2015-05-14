/*
 * parameter.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: caleb
 */

#include "parameter.h"

Parameter::Parameter(const string* name, const Expression* expression) {
	this->name = name;
	this->expression = expression;
}

Parameter::~Parameter() {
}

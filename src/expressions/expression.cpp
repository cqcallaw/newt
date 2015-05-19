/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
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

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

Expression::Expression(const YYLTYPE position) :
		m_position(position) {
}

Expression::~Expression() {
}

const void* Expression::Evaluate(
		const ExecutionContext* execution_context) const {
	return NULL;
}

const string* Expression::ToString(
		const ExecutionContext* execution_context) const {
	ostringstream buffer;
	switch (GetType(execution_context)) {
	case BOOLEAN:
		buffer << *((bool*) Evaluate(execution_context));
		break;
	case INT:
		buffer << *((int*) Evaluate(execution_context));
		break;
	case DOUBLE:
		buffer << *((double*) Evaluate(execution_context));
		break;
	case STRING:
		buffer << *((string*) Evaluate(execution_context));
		break;
	default:
		assert(false);
		return NULL;
	}

	return new string(buffer.str());
}

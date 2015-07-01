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

#include "constant_expression.h"

ConstantExpression::ConstantExpression(const YYLTYPE position, const bool value) :
		Expression(BOOLEAN, position) {
	bool* heap_value = new bool;
	*heap_value = value;
	m_value = (void*) heap_value;
}

ConstantExpression::ConstantExpression(const YYLTYPE position, const int value) :
		Expression(INT, position) {
	int* heap_value = new int;
	*heap_value = value;
	m_value = (void*) heap_value;
}

ConstantExpression::ConstantExpression(const YYLTYPE position,
		const double value) :
		Expression(DOUBLE, position) {
	double* heap_value = new double;
	*heap_value = value;
	m_value = (void*) heap_value;
}

ConstantExpression::ConstantExpression(const YYLTYPE position,
		const std::string* value) :
		Expression(STRING, position) {
	m_value = (void*) value;
}

const void* ConstantExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	return m_value;
}

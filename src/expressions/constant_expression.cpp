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
		ConstantExpression(position, BOOLEAN, (void*) new bool(value)) {
}

ConstantExpression::ConstantExpression(const YYLTYPE position, const int value) :
		ConstantExpression(position, INT, (void*) new int(value)) {
}

ConstantExpression::ConstantExpression(const YYLTYPE position,
		const double value) :
		ConstantExpression(position, DOUBLE, (void*) new double(value)) {
}

ConstantExpression::ConstantExpression(const YYLTYPE position,
		const std::string* value) :
		ConstantExpression(position, STRING, (void*) value) {
}

const BasicType ConstantExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_type;
}

const Result* ConstantExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	return new Result(m_value, LinkedList<const Error*>::Terminator);
}

ConstantExpression::ConstantExpression(const YYLTYPE position, const BasicType type,
		const void* value) :
		Expression(position), m_type(type), m_value(value) {
}

const LinkedList<const Error*>* ConstantExpression::Validate(
		const ExecutionContext* execution_context) const {
	return LinkedList<const Error*>::Terminator;
}

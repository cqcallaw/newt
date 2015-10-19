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

#include <invoke_expression.h>
#include <argument_list.h>

InvokeExpression::InvokeExpression(const YYLTYPE position,
		const Expression* expression, const ArgumentList* argument_list,
		const YYLTYPE argument_list_position) :
		Expression(position), m_expression(expression), m_argument_list(
				argument_list), m_argument_list_position(argument_list_position) {
}

InvokeExpression::~InvokeExpression() {
}

const TypeSpecifier* InvokeExpression::GetType(
		const ExecutionContext* execution_context) const {
}

const Result* InvokeExpression::Evaluate(
		const ExecutionContext* execution_context) const {
}

const Result* InvokeExpression::ToString(
		const ExecutionContext* execution_context) const {
}

const LinkedList<const Error*>* InvokeExpression::Validate(
		const ExecutionContext* execution_context) const {
}

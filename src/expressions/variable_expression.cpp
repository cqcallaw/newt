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

#include <variable_expression.h>
#include <variable.h>
#include <array_variable.h>
#include <compound_type_instance.h>
#include <member_variable.h>
#include <error.h>
#include <sstream>
#include <execution_context.h>
#include <type.h>

VariableExpression::VariableExpression(const YYLTYPE position,
		const Variable* variable) :
		Expression(position), m_variable(variable) {
}

const TypeSpecifier* VariableExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_variable->GetType(execution_context);
}

const Result* VariableExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	return m_variable->Evaluate(execution_context);
}

const LinkedList<const Error*>* VariableExpression::Validate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors = m_variable->Validate(
			execution_context);

	return errors;
}

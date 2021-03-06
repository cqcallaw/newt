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
#include <member_variable.h>
#include <error.h>
#include <sstream>
#include <execution_context.h>
#include <record.h>
#include <type.h>

VariableExpression::VariableExpression(const yy::location position,
		const_shared_ptr<Variable> variable) :
		Expression(position), m_variable(variable) {
}

TypedResult<TypeSpecifier> VariableExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return m_variable->GetTypeSpecifier(execution_context, resolution);
}

const_shared_ptr<Result> VariableExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	// it is somewhat astonishing that the lookup doesn't search the closure context,
	// but the best way semantics of multi-context require further thought
	return m_variable->Evaluate(context);
}

const ErrorListRef VariableExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = m_variable->Validate(execution_context);

	return errors;
}

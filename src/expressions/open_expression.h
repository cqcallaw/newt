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

#ifndef EXPRESSIONS_OPEN_EXPRESSION_H_
#define EXPRESSIONS_OPEN_EXPRESSION_H_

#include <invoke_expression.h>

class OpenExpression: public InvokeExpression {
public:
	OpenExpression(const yy::location location,
			const_shared_ptr<Expression> expression,
			const ArgumentListRef argument_list,
			const yy::location argument_list_location);
	virtual ~OpenExpression();

	virtual TypedResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	TypedResult<string> ToString(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const {
		return false;
	}

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const;
};

#endif /* EXPRESSIONS_OPEN_EXPRESSION_H_ */

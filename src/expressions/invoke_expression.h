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

#ifndef EXPRESSIONS_INVOKE_EXPRESSION_H_
#define EXPRESSIONS_INVOKE_EXPRESSION_H_

#include <expression.h>

class InvokeExpression: public Expression {
public:
	InvokeExpression(const yy::location position, const_shared_ptr<Expression> expression,
			ArgumentListRef argument_list,
			const yy::location argument_list_position);
	virtual ~InvokeExpression();

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const_shared_ptr<ExecutionContext> execution_context) const;

	const_shared_ptr<Result> ToString(const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const {
		return false;
	}

	virtual const ErrorListRef Validate(
			const_shared_ptr<ExecutionContext> execution_context) const;

	const_shared_ptr<Expression> GetExpression() const {
		return m_expression;
	}

	ArgumentListRef GetArgumentListRef() const {
		return m_argument_list;
	}

	const yy::location GetArgumentListRefPosition() const {
		return m_argument_list_position;
	}

private:
	const_shared_ptr<Expression> m_expression;
	ArgumentListRef m_argument_list;
	const yy::location m_argument_list_position;
};

#endif /* STATEMENTS_INVOKE_STATEMENT_H_ */

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

#ifndef UNARY_EXPRESSION_H_
#define UNARY_EXPRESSION_H_

#include "expression.h"

class UnaryExpression: public Expression {
public:
	UnaryExpression(const yy::location position, const OperatorType op,
			const_shared_ptr<Expression> expression);

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution =
					AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const {
		return m_operator == UNARY_MINUS && m_expression->IsConstant();
	}

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const;

private:
	static const_shared_ptr<TypeSpecifier> compute_result_type(
			const_shared_ptr<TypeSpecifier> input_type, const OperatorType op);
	const_shared_ptr<Expression> m_expression;
	const OperatorType m_operator;
};

#endif /* UNARY_EXPRESSION_H_ */

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

#ifndef EXPRESSIONS_FUNCTION_EXPRESSION_H_
#define EXPRESSIONS_FUNCTION_EXPRESSION_H_

#include <expression.h>

class FunctionDeclaration;
class StatementBlock;

class FunctionExpression: public Expression {
public:
	FunctionExpression(const yy::location position,
			const_shared_ptr<FunctionDeclaration> type,
			const_shared_ptr<StatementBlock> body);
	virtual ~FunctionExpression();

	virtual TypedResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const bool IsConstant() const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const;

private:
	const_shared_ptr<FunctionDeclaration> m_declaration;
	const_shared_ptr<StatementBlock> m_body;
};

#endif /* EXPRESSIONS_FUNCTION_EXPRESSION_H_ */

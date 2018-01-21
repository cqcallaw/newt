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
	InvokeExpression(const yy::location position,
			const_shared_ptr<Expression> expression,
			const ArgumentListRef argument_list,
			const yy::location argument_list_position,
			const TypeSpecifierListRef type_argument_list,
			const yy::location type_argument_list_location);
	virtual ~InvokeExpression();

	static const_shared_ptr<InvokeExpression> BuildInvokeExpression(
			const yy::location location,
			const_shared_ptr<Expression> expression,
			const ArgumentListRef argument_list,
			const yy::location argument_list_location,
			const TypeSpecifierListRef type_argument_list,
			const yy::location type_argument_list_location);

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

	const_shared_ptr<Expression> GetExpression() const {
		return m_expression;
	}

	ArgumentListRef GetArgumentListRef() const {
		return m_argument_list;
	}

	const yy::location GetArgumentListRefLocation() const {
		return m_argument_list_location;
	}

	const TypeSpecifierListRef GetTypeArgumentList() const {
		return m_type_argument_list;
	}

	const yy::location GetTypeArgumentListPosition() const {
		return m_type_argument_list_location;
	}

	static const vector<string> BuiltinFunctionList;

private:
	const_shared_ptr<Expression> m_expression;
	const ArgumentListRef m_argument_list;
	const yy::location m_argument_list_location;
	const TypeSpecifierListRef m_type_argument_list;
	const yy::location m_type_argument_list_location;
};

#endif /* STATEMENTS_INVOKE_STATEMENT_H_ */

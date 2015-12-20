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
	InvokeExpression(const yy::location position, const Expression* expression,
			const ArgumentList* argument_list,
			const yy::location argument_list_position);
	virtual ~InvokeExpression();

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const ExecutionContext* execution_context) const;

	virtual const Result* Evaluate(
			const ExecutionContext* execution_context) const;

	const Result* ToString(const ExecutionContext* execution_context) const;

	virtual const bool IsConstant() const {
		return false;
	}

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context) const;

	const Expression* GetExpression() const {
		return m_expression;
	}

	const ArgumentList* GetArgumentList() const {
		return m_argument_list;
	}

	const yy::location GetArgumentListPosition() const {
		return m_argument_list_position;
	}

private:
	const Expression* m_expression;
	const ArgumentList* m_argument_list;
	const yy::location m_argument_list_position;
};

#endif /* STATEMENTS_INVOKE_STATEMENT_H_ */

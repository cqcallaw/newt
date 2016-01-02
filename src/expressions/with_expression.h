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

#ifndef EXPRESSIONS_WITH_EXPRESSION_H_
#define EXPRESSIONS_WITH_EXPRESSION_H_

#include <expression.h>
#include <member_instantiation.h>

class WithExpression: public Expression {
public:
	WithExpression(const yy::location position, const_shared_ptr<Expression> source_expression,
			MemberInstantiationListRef member_instantiation_list,
			const yy::location member_instantiation_list_position);
	virtual ~WithExpression();

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const;

	virtual const ErrorListRef Validate(
			const_shared_ptr<ExecutionContext> execution_context) const;

	MemberInstantiationListRef GetMemberInstantiationListRef() const {
		return m_member_instantiation_list;
	}

	const yy::location GetMemberInstantiationListRefPosition() const {
		return m_member_instantiation_list_position;
	}

	const_shared_ptr<Expression> GetSourceExpression() const {
		return m_source_expression;
	}

private:
	const_shared_ptr<Expression> m_source_expression;
	MemberInstantiationListRef m_member_instantiation_list;
	const yy::location m_member_instantiation_list_position;
};

#endif /* EXPRESSIONS_WITH_EXPRESSION_H_ */

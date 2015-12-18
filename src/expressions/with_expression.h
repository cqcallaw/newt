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
	WithExpression(const YYLTYPE position, const Expression* source_expression,
			const MemberInstantiationList* member_instantiation_list,
			const YYLTYPE member_instantiation_list_position);
	virtual ~WithExpression();

	virtual const TypeSpecifier* GetType(
			const ExecutionContext* execution_context) const;

	virtual const Result* Evaluate(
			const ExecutionContext* execution_context) const;

	virtual const bool IsConstant() const;

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context) const;

	const MemberInstantiationList* GetMemberInstantiationList() const {
		return m_member_instantiation_list;
	}

	const YYLTYPE GetMemberInstantiationListPosition() const {
		return m_member_instantiation_list_position;
	}

	const Expression* GetSourceExpression() const {
		return m_source_expression;
	}

private:
	const Expression* m_source_expression;
	const MemberInstantiationList* m_member_instantiation_list;
	const YYLTYPE m_member_instantiation_list_position;
};

#endif /* EXPRESSIONS_WITH_EXPRESSION_H_ */

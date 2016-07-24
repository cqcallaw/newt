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

#ifndef MEMBER_VARIABLE_H_
#define MEMBER_VARIABLE_H_

#include <variable.h>
#include <assignment_type.h>

class MemberVariable: public Variable {
public:
	MemberVariable(const_shared_ptr<Variable> container_variable,
			const_shared_ptr<Variable> member_variable);
	virtual ~MemberVariable();

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<std::string> ToString(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef AssignValue(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<Expression> expression,
			const AssignmentType op) const;

	const_shared_ptr<Variable> GetContainer() const {
		return m_container;
	}

	const_shared_ptr<Variable> GetMemberVariable() const {
		return m_member_variable;
	}

protected:
	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<bool> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<int> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<double> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<std::string> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Array> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<ComplexTypeSpecifier> type,
			const_shared_ptr<Record> value) const;

private:
	const_shared_ptr<Variable> m_container;
	const_shared_ptr<Variable> m_member_variable;
};

#endif /* MEMBER_VARIABLE_H_ */

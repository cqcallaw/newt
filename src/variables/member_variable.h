/*
 * member_variable.h
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
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

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const_shared_ptr<ExecutionContext> context) const;

	virtual const string* ToString(
			const_shared_ptr<ExecutionContext> context) const;

	virtual const ErrorList Validate(
			const_shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const_shared_ptr<ExecutionContext> context) const;

	virtual const ErrorList AssignValue(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<Expression> expression,
			const AssignmentType op) const;

	const_shared_ptr<Variable> GetContainer() const {
		return m_container;
	}

	const_shared_ptr<Variable> GetMemberVariable() const {
		return m_member_variable;
	}

protected:
	virtual const ErrorList SetSymbol(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<bool> value) const;

	virtual const ErrorList SetSymbol(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<int> value) const;

	virtual const ErrorList SetSymbol(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<double> value) const;

	virtual const ErrorList SetSymbol(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<string> value) const;

	virtual const ErrorList SetSymbol(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<Array> value) const;

	virtual const ErrorList SetSymbol(
			const_shared_ptr<ExecutionContext> context,
			const_shared_ptr<CompoundTypeInstance> value) const;

private:
	const_shared_ptr<Variable> m_container;
	const_shared_ptr<Variable> m_member_variable;
};

#endif /* MEMBER_VARIABLE_H_ */

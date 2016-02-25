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
			const shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<std::string> ToString(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef AssignValue(
			const shared_ptr<ExecutionContext> context,
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
			const_shared_ptr<Record> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const;

private:
	const_shared_ptr<Variable> m_container;
	const_shared_ptr<Variable> m_member_variable;
};

#endif /* MEMBER_VARIABLE_H_ */

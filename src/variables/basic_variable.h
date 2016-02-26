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

#ifndef VARIABLES_BASIC_VARIABLE_H_
#define VARIABLES_BASIC_VARIABLE_H_

#include <variable.h>
#include <assignment_type.h>

class BasicVariable: public Variable {
public:
	BasicVariable(const_shared_ptr<string> name, const yy::location location);
	BasicVariable(const_shared_ptr<string> name, const yy::location location,
			const NamespaceQualifierListRef space);
	virtual ~BasicVariable();

	virtual const_shared_ptr<string> ToString(
			const shared_ptr<ExecutionContext> context) const;

	static const_shared_ptr<Variable> GetDefaultVariable();

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef AssignValue(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Expression> expression,
			const AssignmentType op) const;

	const ErrorListRef AssignValue(const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Expression> expression,
			const shared_ptr<ExecutionContext> output_context,
			const AssignmentType op) const;

	const NamespaceQualifierListRef GetSpace() const {
		return m_space;
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
			const_shared_ptr<string> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Array> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Record> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Function> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Sum> sum) const;

private:
	const NamespaceQualifierListRef m_space;
};

#endif /* VARIABLES_BASIC_VARIABLE_H_ */

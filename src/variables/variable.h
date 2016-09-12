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

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <location.hh>
#include <symbol.h>
#include <string>
#include <symbol_context.h>
#include <assignment_type.h>

class Expression;
class ExecutionContext;
class Result;

class Variable {
public:
	Variable(const_shared_ptr<std::string> name, const yy::location location);
	virtual ~Variable();

	virtual const_shared_ptr<std::string> ToString(
			const shared_ptr<ExecutionContext> context) const = 0;

	virtual TypedResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> context,
			AliasResolution resolution = AliasResolution::RESOLVE) const = 0;

	const_shared_ptr<std::string> GetName() const {
		return m_name;
	}

	const yy::location GetLocation() const {
		return m_location;
	}

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> context) const = 0;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context) const = 0;

	virtual const ErrorListRef AssignValue(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<Expression> expression,
			const AssignmentType op) const = 0;

protected:
	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<bool> value) const = 0;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<int> value) const = 0;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<double> value) const = 0;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<std::string> value) const = 0;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Array> value) const = 0;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<ComplexTypeSpecifier> type,
			const_shared_ptr<Record> value) const = 0;

	const ErrorListRef ToErrorListRef(SetResult result,
			const_shared_ptr<TypeSpecifier> symbol_type,
			const_shared_ptr<TypeSpecifier> expression_type) const;

private:
	const_shared_ptr<std::string> m_name;
	const yy::location m_location;
};

#endif /* VARIABLE_H_ */

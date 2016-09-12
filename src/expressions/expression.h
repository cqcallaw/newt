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

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <array.h>
#include <symbol_table.h>
#include <utils.h>
#include <assert.h>
#include <type.h>
#include <linked_list.h>
#include <result.h>
#include <symbol.h>
#include <error.h>

class ExecutionContext;

class Expression {
public:
	Expression(const yy::location position);
	virtual ~Expression();

	const yy::location GetPosition() const {
		return m_position;
	}

	virtual TResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution = AliasResolution::RESOLVE) const = 0;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const = 0;

	TResult<string> ToString(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const bool IsConstant() const = 0;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const = 0;

private:
	const yy::location m_position;
};

typedef const LinkedList<const Expression, NO_DUPLICATES> ArgumentList;
typedef shared_ptr<ArgumentList> ArgumentListRef;

#endif /* EXPRESSION_H_ */

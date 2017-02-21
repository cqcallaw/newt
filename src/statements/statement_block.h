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

#ifndef STATEMENT_BLOCK_H
#define STATEMENT_BLOCK_H

#include "linked_list.h"

#include "statement.h"

class Statement;

class StatementBlock {
public:
	StatementBlock(StatementListRef statements, const yy::location location);
	virtual ~StatementBlock();

	const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<TypeSpecifier> return_type_specifier) const;

	const PreprocessResult Preprocess(const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier) const;

	const ErrorListRef Execute(
			shared_ptr<ExecutionContext> execution_context) const;

	const ErrorListRef Execute(const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	const yy::location GetLocation() const {
		return m_location;
	}

private:
	StatementListRef m_statements;
	const yy::location m_location;
};

#endif // #ifndef STATEMENT_BLOCK_H

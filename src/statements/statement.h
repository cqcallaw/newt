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

#ifndef STATEMENT_H_
#define STATEMENT_H_
#include "linked_list.h"
#include "analysis_result.h"
#include <defaults.h>
#include <error.h>

class ExecutionContext;
class Result;
class TypeSpecifier;

class Statement {
public:
	Statement() {
	}
	virtual ~Statement() {
	}

	/**
	 * Pre-process the statement. Here the symbol table is populated, and semantic verification is carried out.
	 */
	virtual const ErrorListRef preprocess(
			const_shared_ptr<ExecutionContext> execution_context) const = 0;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const = 0;

	virtual const AnalysisResult Returns(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<ExecutionContext> execution_context) const = 0;
};

typedef const LinkedList<const Statement, NO_DUPLICATES> StatementList;
typedef shared_ptr<StatementList> StatementListRef;

#endif /* STATEMENT_H_ */

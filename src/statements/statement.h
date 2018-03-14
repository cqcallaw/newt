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
#include <result.h>
#include <type_specifier.h>

class ExecutionContext;
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
	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const TypeSpecifierListRef type_parameter_list,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const = 0;

	/**
	 * Execute the statement. Here the interpreter carries out the rest of the statement instructions.
	 */
	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const = 0;
};

typedef const LinkedList<const Statement, NO_DUPLICATES> StatementList;
typedef shared_ptr<StatementList> StatementListRef;

#endif /* STATEMENT_H_ */

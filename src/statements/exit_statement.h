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

#ifndef EXIT_STATEMENT_H_
#define EXIT_STATEMENT_H_

#include "statement.h"

class Expression;
class Error;

class ExitStatement: public Statement {
public:
	ExitStatement();
	ExitStatement(const_shared_ptr<Expression> exit_expression);
	virtual ~ExitStatement();

	virtual const ErrorListRef Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const ErrorListRef Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const ErrorListRef GetReturnStatementErrors(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const shared_ptr<ExecutionContext> execution_context) const {
		return ErrorList::GetTerminator();
	}

private:
	const_shared_ptr<Expression> m_exit_expression;
};

#endif /* EXIT_STATEMENT_H_ */

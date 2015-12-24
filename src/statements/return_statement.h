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

#ifndef STATEMENTS_RETURN_STATEMENT_H_
#define STATEMENTS_RETURN_STATEMENT_H_

#include <statement.h>

class Expression;

class ReturnStatement: public Statement {
public:
	ReturnStatement(const_shared_ptr<Expression> expression);
	virtual ~ReturnStatement();

	virtual const ErrorList preprocess(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorList execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const AnalysisResult Returns(const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<ExecutionContext> execution_context) const;

private:
	const_shared_ptr<Expression> m_expression;
};

#endif /* STATEMENTS_RETURN_STATEMENT_H_ */

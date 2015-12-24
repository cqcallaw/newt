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

#ifndef STATEMENTS_INVOKE_STATEMENT_H_
#define STATEMENTS_INVOKE_STATEMENT_H_

#include <expression.h>
#include <statement.h>

class Variable;

class InvokeStatement: public Statement {
public:
	InvokeStatement(const_shared_ptr<Variable> variable, ArgumentList argument_list,
			const yy::location argument_list_position);
	virtual ~InvokeStatement();

	virtual const ErrorList preprocess(
			const_shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorList execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const AnalysisResult Returns(const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<ExecutionContext> execution_context) const {
		return AnalysisResult::NO;
	}
private:
	const_shared_ptr<Variable> m_variable;
	ArgumentList m_argument_list;
	const yy::location m_argument_list_position;
};

#endif /* STATEMENTS_INVOKE_STATEMENT_H_ */

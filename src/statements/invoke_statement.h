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

#include <statement.h>
#include <yyltype.h>

class ArgumentList;
class Variable;

class InvokeStatement: public Statement {
public:
	InvokeStatement(const Variable* variable, const ArgumentList* argument_list,
			const YYLTYPE argument_list_position);
	virtual ~InvokeStatement();

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;

	virtual const AnalysisResult Returns(
			const TypeSpecifier* type_specifier) const {
		return AnalysisResult::NO;
	}
private:
	const Variable* m_variable;
	const ArgumentList* m_argument_list;
	const YYLTYPE m_argument_list_position;
};

#endif /* STATEMENTS_INVOKE_STATEMENT_H_ */

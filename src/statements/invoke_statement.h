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
	InvokeStatement(const_shared_ptr<Variable> variable,
			ArgumentListRef argument_list,
			const yy::location argument_list_position,
			const TypeSpecifierListRef type_argument_list,
			const yy::location type_argument_list_location);
	virtual ~InvokeStatement();

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

private:
	const_shared_ptr<Variable> m_variable;
	ArgumentListRef m_argument_list;
	const yy::location m_argument_list_position;
	const_shared_ptr<Expression> m_expression;
};

#endif /* STATEMENTS_INVOKE_STATEMENT_H_ */

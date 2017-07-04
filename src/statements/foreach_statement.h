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

#ifndef STATEMENTS_FOREACH_STATEMENT_H_
#define STATEMENTS_FOREACH_STATEMENT_H_

#include <statement.h>

class Expression;
class StatementBlock;

class ForeachStatement: public Statement {
public:
	ForeachStatement(const_shared_ptr<string> name,
			const_shared_ptr<Expression> expression,
			const_shared_ptr<StatementBlock> statement_block);
	virtual ~ForeachStatement();

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ErrorListRef Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	static const_shared_ptr<std::string> NEXT_NAME;

private:
	const_shared_ptr<string> m_name;
	const_shared_ptr<Expression> m_expression;
	const_shared_ptr<StatementBlock> m_statement_block;
	shared_ptr<ExecutionContext> m_block_context;
};

#endif /* STATEMENTS_FOREACH_STATEMENT_H_ */

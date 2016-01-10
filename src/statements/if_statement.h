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

#ifndef IF_STATEMENT_H_
#define IF_STATEMENT_H_

#include "statement.h"

class Expression;
class StatementBlock;
class Error;
class SymbolTable;

class IfStatement: public Statement {
public:
	IfStatement(const_shared_ptr<Expression> expression,
			const_shared_ptr<StatementBlock> block);
	IfStatement(const_shared_ptr<Expression> expression,
			const_shared_ptr<StatementBlock> block,
			const_shared_ptr<StatementBlock> else_block);
	virtual ~IfStatement();

	const_shared_ptr<Expression> GetExpression() const {
		return m_expression;
	}

	virtual const ErrorListRef preprocess(
			const shared_ptr<ExecutionContext> execution_context) const;

	virtual const ErrorListRef execute(
			shared_ptr<ExecutionContext> execution_context) const;

	virtual const AnalysisResult Returns(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const shared_ptr<ExecutionContext> execution_context) const;

private:
	const_shared_ptr<Expression> m_expression;
	const_shared_ptr<StatementBlock> m_block;
	const_shared_ptr<StatementBlock> m_else_block;
	shared_ptr<SymbolTable> m_block_table;
	shared_ptr<SymbolTable> m_else_block_table;
};

#endif /* IF_STATEMENT_H_ */

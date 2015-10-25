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
	IfStatement(const Expression* expression, const StatementBlock* block);
	IfStatement(const Expression* expression, const StatementBlock* block,
			const StatementBlock* else_block);
	virtual ~IfStatement();

	const Expression* GetExpression() const {
		return m_expression;
	}

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			ExecutionContext* execution_context) const;

	virtual const AnalysisResult Returns(const TypeSpecifier* type_specifier,
			const ExecutionContext* execution_context) const;

private:
	const Expression* m_expression;
	const StatementBlock* m_block;
	const StatementBlock* m_else_block;
	SymbolTable* m_block_table;
	SymbolTable* m_else_block_table;
};

#endif /* IF_STATEMENT_H_ */

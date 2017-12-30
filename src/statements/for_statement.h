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

#ifndef FOR_STATEMENT_H_
#define FOR_STATEMENT_H_

#include "statement.h"

class AssignmentStatement;
class DeclarationStatement;
class Expression;
class StatementBlock;
class SymbolTable;
class TypeSpecifier;

class ForStatement: public Statement {
public:
	ForStatement(const_shared_ptr<AssignmentStatement> initial,
			const_shared_ptr<Expression> loop_expression,
			const_shared_ptr<AssignmentStatement> loop_assignment,
			const_shared_ptr<StatementBlock> statement_block);
	ForStatement(const_shared_ptr<DeclarationStatement> initial,
			const_shared_ptr<Expression> loop_expression,
			const_shared_ptr<AssignmentStatement> loop_assignment,
			const_shared_ptr<StatementBlock> statement_block);
	virtual ~ForStatement();

	virtual const PreprocessResult Preprocess(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure,
			const_shared_ptr<TypeSpecifier> return_type_specifier = nullptr) const;

	virtual const ExecutionResult Execute(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

private:
	ForStatement(const_shared_ptr<Statement> initial,
			const_shared_ptr<Expression> loop_expression,
			const_shared_ptr<AssignmentStatement> loop_assignment,
			const_shared_ptr<StatementBlock> statement_block);

	const_shared_ptr<Statement> m_initial;
	const_shared_ptr<Expression> m_loop_expression;
	const_shared_ptr<AssignmentStatement> m_loop_assignment;
	const_shared_ptr<StatementBlock> m_statement_block;
	shared_ptr<ExecutionContext> m_block_context;
};

#endif /* FOR_STATEMENT_H_ */

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
class Expression;
class StatementBlock;
class SymbolTable;
class TypeSpecifier;

class ForStatement: public Statement {
public:
	ForStatement(const AssignmentStatement* initial,
			const Expression* loop_expression,
			const AssignmentStatement* loop_assignment,
			const StatementBlock* statement_block);
	virtual ~ForStatement();

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			ExecutionContext* execution_context) const;

	virtual const AnalysisResult Returns(const_shared_ptr<TypeSpecifier> type_specifier,
			const ExecutionContext* execution_context) const;

private:
	const AssignmentStatement* m_initial;
	const Expression* m_loop_expression;
	const AssignmentStatement* m_loop_assignment;
	const StatementBlock* m_statement_block;
	SymbolTable* m_block_table;
};

#endif /* FOR_STATEMENT_H_ */

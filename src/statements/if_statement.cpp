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

#include "if_statement.h"

#include <expression.h>
#include <statement_block.h>
#include <defaults.h>
#include <assert.h>
#include <type.h>
#include <error.h>

IfStatement::IfStatement(const Expression* expression,
		const StatementBlock* block) :
		IfStatement(expression, block, nullptr) {
}

IfStatement::IfStatement(const Expression* expression,
		const StatementBlock* block, const StatementBlock* else_block) :
		m_expression(expression), m_block(block), m_else_block(else_block) {
}

IfStatement::~IfStatement() {
}

LinkedList<const Error*>* IfStatement::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;

	if (m_expression != nullptr) {
		if (!(m_expression->GetType(execution_context) & (BOOLEAN | INT))) {
			YYLTYPE position = m_expression->GetPosition();
			result = (LinkedList<const Error*>*) result->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_TYPE_FOR_IF_STMT_EXPRESSION,
							position.first_line, position.first_column));
		}
	} else {
		assert(false);
	}

	return result;
}

void IfStatement::execute(const ExecutionContext* execution_context) const {
	const Result* evaluation = m_expression->Evaluate(
			execution_context);
	bool test = *((bool*) evaluation->GetData());
	if (test) {
		m_block->execute(execution_context);
	} else if (m_else_block != nullptr) {
		m_else_block->execute(execution_context);
	}
	delete (evaluation);
}

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

#include "for_statement.h"
#include <expression.h>
#include <defaults.h>
#include <assert.h>
#include <assignment_statement.h>
#include "statement_block.h"
#include <error.h>

ForStatement::ForStatement(const AssignmentStatement* initial,
		const Expression* loop_expression,
		const AssignmentStatement* loop_assignment,
		const StatementBlock* statement_block) :
		m_initial(initial), m_loop_expression(loop_expression), m_loop_assignment(
				loop_assignment), m_statement_block(statement_block) {
	assert(loop_expression != nullptr);
	assert(loop_assignment != nullptr);
}

ForStatement::~ForStatement() {
}

LinkedList<const Error*>* ForStatement::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;

	if (m_loop_expression != nullptr) {
		if (!(m_loop_expression->GetType(execution_context) & (BOOLEAN | INT))) {
			YYLTYPE position = m_loop_expression->GetPosition();
			result = (LinkedList<const Error*>*) result->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
							position.first_line, position.first_column));
		}
	}

	return result;
}

void ForStatement::execute(const ExecutionContext* execution_context) const {
	if (m_initial != nullptr) {
		m_initial->execute(execution_context);
	}
	Result* evaluation =
			(Result*) m_loop_expression->Evaluate(execution_context);

	//TODO: handle evaluation errors
	while (*((bool*) evaluation->GetData())) {
		if (m_statement_block != nullptr) {
			m_statement_block->execute(execution_context);
		}

		m_loop_assignment->execute(execution_context);

		delete (evaluation);
		evaluation = (Result*) m_loop_expression->Evaluate(
				execution_context);
	}
	delete (evaluation);
}

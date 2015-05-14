/*
 * for_statement.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: caleb
 */

#include "for_statement.h"
#include <expression.h>
#include <defaults.h>
#include <assert.h>
#include "assignment_statement.h"
#include "statement_block.h"

ForStatement::ForStatement(const AssignmentStatement* initial,
		const Expression* loop_expression,
		const AssignmentStatement* loop_assignment,
		const StatementBlock* statement_block) :
		m_initial(initial), m_loop_expression(loop_expression), m_loop_assignment(
				loop_assignment), m_statement_block(statement_block) {
	assert(loop_expression != nullptr);
	if (loop_expression != DefaultExpression) {
		assert(loop_expression->GetType() & (BOOLEAN | INT));
	}
	assert(loop_assignment != nullptr);
}

ForStatement::~ForStatement() {
}

void ForStatement::execute() const {
	if (m_initial != nullptr) {
		m_initial->execute();
	}

	for (; *((bool*) m_loop_expression->Evaluate());
			m_loop_assignment->execute()) {
		if (m_statement_block != nullptr) {
			m_statement_block->execute();
		}
	}
}

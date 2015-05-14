/*
 * if_statement.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: caleb
 */

#include "if_statement.h"

#include <expression.h>
#include <statement_block.h>
#include <defaults.h>
#include <assert.h>
#include <type.h>

IfStatement::IfStatement(const Expression* expression,
		const StatementBlock* block) :
		IfStatement(expression, block, nullptr) {
}

IfStatement::IfStatement(const Expression* expression,
		const StatementBlock* block, const StatementBlock* else_block) :
		m_expression(expression), m_block(block), m_else_block(else_block) {
	if (expression != DefaultExpression) {
		//not our default statement
		assert(expression->GetType() == BOOLEAN);
	}
}

IfStatement::~IfStatement() {
}

void IfStatement::execute() const {
	if (m_expression == nullptr || m_expression == DefaultExpression) {
		assert(false);
		return;
	}

	if (m_block == nullptr || m_block == DefaultStatementBlock) {
		assert(false);
		return;
	}

	bool test = *((bool*) m_expression->Evaluate());
	if (test) {
		m_block->execute();
	} else if (m_else_block != nullptr) {
		m_else_block->execute();
	}
}

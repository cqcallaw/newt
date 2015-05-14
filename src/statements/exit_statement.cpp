/*
 * exit_statement.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: caleb
 */

#include "exit_statement.h"
#include <expression.h>
#include <assert.h>

ExitStatement::ExitStatement(const int line_number,
		const Expression* exit_expression) :
		m_line_number(line_number), m_exit_expression(exit_expression) {
	if (exit_expression != nullptr) {
		assert(exit_expression->GetType() <= INT);
	}
}

ExitStatement::~ExitStatement() {
}

void ExitStatement::execute() const {
	if (m_exit_expression == nullptr) {
		return;
	}

	int exit_code = *((int*) m_exit_expression->Evaluate());
	std::cout << "[" << m_line_number << "]: exit(" << exit_code << ")\n";
	exit(exit_code);
}

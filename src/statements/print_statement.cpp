/*
 * print_statement.cpp
 *
 *  Created on: Mar 20, 2015
 *      Author: caleb
 */

#include <expression.h>
#include "print_statement.h"
#include <defaults.h>

PrintStatement::PrintStatement(const int line_number,
		const Expression* expression) :
		m_line_number(line_number), m_expression(expression) {
}

PrintStatement::~PrintStatement() {
}

void PrintStatement::execute() const {
	std::cout << "[" << m_line_number << "]: " << *(m_expression->ToString())
			<< "\n";
}

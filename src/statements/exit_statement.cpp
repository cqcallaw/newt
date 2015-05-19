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

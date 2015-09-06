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
#include <error.h>

ExitStatement::ExitStatement() :
		m_exit_expression(nullptr) {
}

ExitStatement::ExitStatement(const Expression* exit_expression) :
		m_exit_expression(exit_expression) {
}

ExitStatement::~ExitStatement() {
}

const LinkedList<const Error*>* ExitStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* result =
			LinkedList<const Error*>::Terminator;

	if ((m_exit_expression != nullptr)
			&& m_exit_expression->GetType(execution_context) > INT) {
		YYLTYPE position = m_exit_expression->GetPosition();
		result = result->With(
				new Error(Error::SEMANTIC,
						Error::EXIT_STATUS_MUST_BE_AN_INTEGER,
						position.first_line, position.first_column,
						type_to_string(
								m_exit_expression->GetType(
										execution_context))));
	}

	return result;
}

const LinkedList<const Error*>* ExitStatement::execute(
		const ExecutionContext* execution_context) const {
	int exit_code = 0;

	if (m_exit_expression == nullptr) {
		const Result* evaluation = m_exit_expression->Evaluate(
				execution_context);
//TODO: handle evaluation errors
		exit_code = *((int*) evaluation->GetData());
		delete (evaluation);
	}

	exit(exit_code);
}

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

#include <expression.h>
#include "print_statement.h"
#include <defaults.h>

PrintStatement::PrintStatement(const int line_number,
		const_shared_ptr<Expression> expression) :
		m_line_number(line_number), m_expression(expression) {
}

PrintStatement::~PrintStatement() {
}

const ErrorList PrintStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors(ErrorListBase::GetTerminator());

	const_shared_ptr<Result> string_result = m_expression->ToString(
			execution_context);
	errors = string_result->GetErrors();

	if (ErrorListBase::IsTerminator(errors)) {
		std::cout
				<< *(static_pointer_cast<const string>(string_result->GetData()))
				<< "\n";
	}

	return errors;
}

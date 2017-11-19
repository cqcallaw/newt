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

const PreprocessResult PrintStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE,
			m_expression->Validate(context));
}

const ExecutionResult PrintStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto string_result = m_expression->ToString(context);
	auto errors = string_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		std::cout << *(string_result.GetData()) << "\n";
	}

	return ExecutionResult(errors);
}

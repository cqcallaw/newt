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
#include <execution_context.h>

ExitStatement::ExitStatement() :
		m_exit_expression(nullptr) {
}

ExitStatement::ExitStatement(const_shared_ptr<Expression> exit_expression) :
		m_exit_expression(exit_expression) {
}

ExitStatement::~ExitStatement() {
}

const ErrorListRef ExitStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (m_exit_expression) {
		const_shared_ptr<TypeSpecifier> expression_type_specifier =
				m_exit_expression->GetTypeSpecifier(execution_context);
		const_shared_ptr<PrimitiveTypeSpecifier> expression_as_primitive =
				std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
						expression_type_specifier);

		if (expression_as_primitive == nullptr
				|| !(expression_as_primitive->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetInt(),
						execution_context->GetTypeTable()))) {
			yy::location position = m_exit_expression->GetPosition();
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::EXIT_STATUS_MUST_BE_AN_INTEGER,
							position.begin.line, position.begin.column,
							m_exit_expression->GetTypeSpecifier(
									execution_context)->ToString()), errors);
		}
	}

	return errors;
}

const ErrorListRef ExitStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	plain_shared_ptr<int> exit_code = make_shared<int>(0);
	if (m_exit_expression) {
		const_shared_ptr<Result> evaluation = m_exit_expression->Evaluate(
				execution_context);

		if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
			return evaluation->GetErrors();
		} else {
			exit_code = evaluation->GetData<int>();
		}
	}

	execution_context->SetExitCode(exit_code);
	return ErrorList::GetTerminator();
}

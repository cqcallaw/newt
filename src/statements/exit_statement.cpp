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

const PreprocessResult ExitStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (m_exit_expression) {
		auto expression_type_specifier_result =
				m_exit_expression->GetTypeSpecifier(context);

		errors = expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();

			const_shared_ptr<PrimitiveTypeSpecifier> expression_as_primitive =
					std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
							expression_type_specifier);

			if (expression_as_primitive == nullptr
					|| !(expression_as_primitive->AnalyzeAssignmentTo(
							PrimitiveTypeSpecifier::GetInt(),
							context->GetTypeTable()))) {
				yy::location position = m_exit_expression->GetPosition();
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::EXIT_STATUS_MUST_BE_AN_INTEGER,
								position.begin.line, position.begin.column,
								expression_type_specifier->ToString()), errors);
			}
		}
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ErrorListRef ExitStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	plain_shared_ptr<int> exit_code = make_shared<int>(0);
	if (m_exit_expression) {
		const_shared_ptr<Result> evaluation = m_exit_expression->Evaluate(
				context, closure);

		if (!ErrorList::IsTerminator(evaluation->GetErrors())) {
			return evaluation->GetErrors();
		} else {
			exit_code = evaluation->GetData<int>();
		}
	}

	context->SetExitCode(exit_code);
	return ErrorList::GetTerminator();
}

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

#include <return_statement.h>
#include <expression.h>
#include <execution_context.h>
#include <sum_type.h>
#include <sum_type_specifier.h>
#include <unit_type.h>

ReturnStatement::ReturnStatement(const_shared_ptr<Expression> expression) :
		m_expression(expression) {
}

ReturnStatement::~ReturnStatement() {
}

const PreprocessResult ReturnStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = m_expression->Validate(context);

	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
				context);
		errors = expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();

			auto assignment_analysis =
					expression_type_specifier->AnalyzeAssignmentTo(
							return_type_specifier, context->GetTypeTable());
			if (assignment_analysis == AnalysisResult::AMBIGUOUS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::AMBIGUOUS_WIDENING_CONVERSION,
								m_expression->GetLocation().begin.line,
								m_expression->GetLocation().begin.column,
								return_type_specifier->ToString(),
								expression_type_specifier->ToString()), errors);
			} else if (assignment_analysis == AnalysisResult::INCOMPATIBLE) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::FUNCTION_RETURN_MISMATCH,
								m_expression->GetLocation().begin.line,
								m_expression->GetLocation().begin.column),
						errors);
			}
		}
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::FULL, errors);
}

const ExecutionResult ReturnStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto result = m_expression->Evaluate(context, closure);

	auto errors = result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
				context);

		errors = expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();
			return ExecutionResult(
					const_shared_ptr<Symbol>(
							new Symbol(expression_type_specifier,
									result->GetRawData())));
		}
	}

	return ExecutionResult(errors);
}

const PreprocessResult::ReturnCoverage ReturnStatement::CoverageTransition(
		PreprocessResult::ReturnCoverage current,
		PreprocessResult::ReturnCoverage input, bool is_start) {
	if (is_start) {
		return input;
	} else {
		switch (current) {
		case PreprocessResult::ReturnCoverage::FULL: {
			switch (input) {
			case PreprocessResult::ReturnCoverage::FULL: {
				return PreprocessResult::ReturnCoverage::FULL;
			}
			case PreprocessResult::ReturnCoverage::PARTIAL: {
				return PreprocessResult::ReturnCoverage::PARTIAL;
			}
			case PreprocessResult::ReturnCoverage::NONE:
			default: {
				return PreprocessResult::ReturnCoverage::PARTIAL;
			}
			}
			break;
		}
		case PreprocessResult::ReturnCoverage::PARTIAL: {
			return PreprocessResult::ReturnCoverage::PARTIAL;
		}
		case PreprocessResult::ReturnCoverage::NONE:
		default: {
			switch (input) {
			case PreprocessResult::ReturnCoverage::FULL: {
				return PreprocessResult::ReturnCoverage::PARTIAL;
			}
			case PreprocessResult::ReturnCoverage::PARTIAL: {
				return PreprocessResult::ReturnCoverage::PARTIAL;
			}
			case PreprocessResult::ReturnCoverage::NONE:
			default: {
				return PreprocessResult::ReturnCoverage::NONE;
			}
			}
		}
		}
	}
}

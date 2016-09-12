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

#include <math.h>
#include <sstream>
#include <defaults.h>
#include "unary_expression.h"
#include "error.h"
#include <execution_context.h>

UnaryExpression::UnaryExpression(const yy::location position,
		const OperatorType op, const_shared_ptr<Expression> expression) :
		Expression(position), m_expression(expression), m_operator(op) {
	assert(expression != NULL);
}

TypedResult<TypeSpecifier> UnaryExpression::compute_result_type(
		const_shared_ptr<TypeSpecifier> input_type, const OperatorType op) {
	switch (op) {
	case UNARY_MINUS:
		return TypedResult<TypeSpecifier>(input_type, ErrorList::GetTerminator());
	case NOT:
		return TypedResult<TypeSpecifier>(PrimitiveTypeSpecifier::GetBoolean(),
				ErrorList::GetTerminator());
	default:
		assert(false);
		return TypedResult<TypeSpecifier>(PrimitiveTypeSpecifier::GetNone(),
				ErrorList::GetTerminator());
	}
}

TypedResult<TypeSpecifier> UnaryExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	auto type_specifier_result = m_expression->GetTypeSpecifier(
			execution_context, resolution);

	if (ErrorList::IsTerminator(type_specifier_result.GetErrors())) {
		auto type_specifier = type_specifier_result.GetData();
		return compute_result_type(type_specifier, m_operator);
	} else {
		return type_specifier_result;
	}
}

const ErrorListRef UnaryExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const OperatorType op = m_operator;

	ErrorListRef expression_errors = m_expression->Validate(execution_context);
	if (!ErrorList::IsTerminator(expression_errors)) {
		errors = ErrorList::Concatenate(errors, expression_errors);
		return errors;
	}

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			execution_context);
	errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();
		auto operand_analysis = expression_type_specifier->AnalyzeAssignmentTo(
				PrimitiveTypeSpecifier::GetDouble(),
				execution_context->GetTypeTable());
		if (operand_analysis != EQUIVALENT && operand_analysis != UNAMBIGUOUS) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_expression->GetPosition().begin.line,
							m_expression->GetPosition().begin.column,
							OperatorToString(op)), errors);
		}
	}

	return errors;
}

const_shared_ptr<Result> UnaryExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	shared_ptr<void> result;

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			context);
	auto errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();
		const_shared_ptr<Result> evaluation = m_expression->Evaluate(context,
				closure);
		ErrorListRef evaluation_errors = evaluation->GetErrors();

		if (!ErrorList::IsTerminator(evaluation_errors)) {
			errors = evaluation_errors;
		} else {
			switch (m_operator) {
			case UNARY_MINUS: {
				auto expression_analysis =
						expression_type_specifier->AnalyzeAssignmentTo(
								PrimitiveTypeSpecifier::GetInt(),
								context->GetTypeTable());
				if (expression_analysis == EQUIVALENT
						|| expression_analysis == UNAMBIGUOUS) {
					int value = -(*(evaluation->GetData<int>()));
					result = make_shared<int>(value);
				} else if (expression_type_specifier->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetDouble(),
						context->GetTypeTable())) {
					double value = -(*(evaluation->GetData<double>()));
					result = make_shared<double>(value);
				} else {
					assert(false);
				}
				break;
			}
			case NOT: {
				if (expression_type_specifier->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetBoolean(),
						context->GetTypeTable()) == EQUIVALENT) {
					bool old_value = *(evaluation->GetData<bool>());
					bool value = !old_value;
					result = make_shared<bool>(value);
				} else if (expression_type_specifier->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetInt(),
						context->GetTypeTable()) == EQUIVALENT) {
					int old_value = *(evaluation->GetData<int>());
					bool value = !(old_value != 0);
					result = make_shared<bool>(value);
					break;
				} else if (expression_type_specifier->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetDouble(),
						context->GetTypeTable()) == EQUIVALENT) {
					double old_value = *(evaluation->GetData<double>());
					bool value = !(old_value != 0);
					result = make_shared<bool>(value);
					break;
				} else {
					assert(false);
				}
				break;

			}
			default:
				assert(false);
			}
		}
	}

	return make_shared<Result>(result, errors);
}


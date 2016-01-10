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

UnaryExpression::UnaryExpression(const yy::location position,
		const OperatorType op, const_shared_ptr<Expression> expression) :
		Expression(position), m_expression(expression), m_operator(op) {
	assert(expression != NULL);
}

const_shared_ptr<TypeSpecifier> UnaryExpression::compute_result_type(
		const_shared_ptr<TypeSpecifier> input_type, const OperatorType op) {
	switch (op) {
	case UNARY_MINUS:
		return input_type;
	case NOT:
		return PrimitiveTypeSpecifier::GetBoolean();
	default:
		assert(false);
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const_shared_ptr<TypeSpecifier> UnaryExpression::GetType(
		const shared_ptr<ExecutionContext> execution_context) const {
	return compute_result_type(m_expression->GetType(execution_context),
			m_operator);
}

const ErrorListRef UnaryExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef result = ErrorList::GetTerminator();

	const OperatorType op = m_operator;
	const_shared_ptr<Expression> expression = m_expression;

	ErrorListRef expression_errors = expression->Validate(execution_context);
	if (!ErrorList::IsTerminator(expression_errors)) {
		result = ErrorList::Concatenate(result, expression_errors);
		return result;
	}

	const_shared_ptr<TypeSpecifier> expression_type = expression->GetType(
			execution_context);
	if (!(expression_type->IsAssignableTo(PrimitiveTypeSpecifier::GetDouble()))) {
		result = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_RIGHT_OPERAND_TYPE,
						expression->GetPosition().begin.line,
						expression->GetPosition().begin.column,
						operator_to_string(op)), result);
	}

	return result;
}

const_shared_ptr<Result> UnaryExpression::Evaluate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	void* result = nullptr;

	const_shared_ptr<TypeSpecifier> expression_type = m_expression->GetType(
			execution_context);
	const_shared_ptr<Result> evaluation = m_expression->Evaluate(
			execution_context);
	ErrorListRef evaluation_errors = evaluation->GetErrors();

	if (!ErrorList::IsTerminator(evaluation_errors)) {
		errors = evaluation_errors;
	} else {
		auto data = evaluation->GetData();
		switch (m_operator) {
		case UNARY_MINUS: {
			if (expression_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				int* value = new int;
				*value = -(*(static_pointer_cast<const int>(data)));
				result = (void *) value;
			} else if (expression_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetDouble())) {
				double* value = new double;
				*value = -(*(static_pointer_cast<const double>(data)));
				result = (void *) value;
			} else {
				assert(false);
			}
			break;
		}
		case NOT: {
			if (expression_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetBoolean())) {
				bool old_value = *(static_pointer_cast<const bool>(data));
				bool* value = new bool(!old_value);
				result = (void *) value;
			} else if (expression_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				int old_value = *(static_pointer_cast<const int>(data));
				bool* value = new bool(!(old_value != 0));
				result = (void *) value;
				break;
			} else if (expression_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetDouble())) {
				double old_value = *(static_pointer_cast<const double>(data));
				bool* value = new bool(!(old_value != 0));
				result = (void *) value;
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

	return make_shared<Result>(const_shared_ptr<void>(result), errors);
}


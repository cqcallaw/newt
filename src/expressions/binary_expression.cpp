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

#include "binary_expression.h"
#include "error.h"
#include <execution_context.h>

BinaryExpression::BinaryExpression(const yy::location position,
		const OperatorType op, const_shared_ptr<Expression> left,
		const_shared_ptr<Expression> right) :
		Expression(position), m_operator(op), m_left(left), m_right(right) {
	assert(left != NULL);
	assert(right != NULL);
}

TypedResult<TypeSpecifier> BinaryExpression::ComputeResultType(
		const_shared_ptr<Expression> left, const_shared_ptr<Expression> right,
		const OperatorType op,
		const shared_ptr<ExecutionContext> execution_context) {
	auto left_type_specifier_result = left->GetTypeSpecifier(execution_context,
			AliasResolution::RESOLVE);
	auto right_type_specifier_result = right->GetTypeSpecifier(
			execution_context, AliasResolution::RESOLVE);

	auto errors = left_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		errors = right_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			const_shared_ptr<PrimitiveTypeSpecifier> left_as_primitive =
					std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
							left_type_specifier_result.GetData());
			const_shared_ptr<PrimitiveTypeSpecifier> right_as_primitive =
					std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
							right_type_specifier_result.GetData());

			if (left_as_primitive
					&& left_as_primitive != PrimitiveTypeSpecifier::GetNone()
					&& right_as_primitive
					&& right_as_primitive
							!= PrimitiveTypeSpecifier::GetNone()) {

				const BasicType left_basic_type =
						left_as_primitive->GetBasicType();
				const BasicType right_basic_type =
						right_as_primitive->GetBasicType();

				if (op == EQUAL || op == NOT_EQUAL || op == LESS_THAN
						|| op == LESS_THAN_EQUAL || op == GREATER_THAN
						|| op == GREATER_THAN_EQUAL || op == AND || op == OR) {
					return TypedResult<TypeSpecifier>(
							PrimitiveTypeSpecifier::GetBoolean(), errors);
				}

				if (op == MOD) {
					return TypedResult<TypeSpecifier>(
							PrimitiveTypeSpecifier::GetInt(), errors);
				}

				if (right_basic_type >= left_basic_type) {
					return TypedResult<TypeSpecifier>(right_as_primitive, errors);
				} else {
					return TypedResult<TypeSpecifier>(left_as_primitive, errors);
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::INVALID_TYPE_COMBINATION,
								right->GetLocation().begin.line,
								right->GetLocation().begin.column), errors);
			}
		}
	}

	return TypedResult<TypeSpecifier>(nullptr, errors);
}

const_shared_ptr<Result> BinaryExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	const_shared_ptr<Expression> left = GetLeft();
	const_shared_ptr<Expression> right = GetRight();

	const_shared_ptr<Result> left_result = left->Evaluate(context, closure);
	if (!ErrorList::IsTerminator(left_result->GetErrors())) {
		return left_result;
	}

	const_shared_ptr<Result> right_result = right->Evaluate(context, closure);
	if (!ErrorList::IsTerminator(right_result->GetErrors())) {
		return right_result;
	}

	auto left_type_specifier_result = left->GetTypeSpecifier(context);
	auto right_type_specifier_result = right->GetTypeSpecifier(context);

	errors = left_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		errors = right_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			yy::location left_position = left->GetLocation();
			yy::location right_position = right->GetLocation();

			auto type_table = context->GetTypeTable();

			auto left_type = left_type_specifier_result.GetData();
			auto right_type = right_type_specifier_result.GetData();

			if (left_type->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetBoolean(), type_table)
					== EQUIVALENT) {
				bool left_value = *(left_result->GetData<bool>());

				if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetBoolean(), type_table)
						== EQUIVALENT) {
					bool right_value = *(right_result->GetData<bool>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetInt(), type_table)
						== EQUIVALENT) {
					int right_value = *(right_result->GetData<int>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetDouble(), type_table)
						== EQUIVALENT) {
					double right_value = *(right_result->GetData<double>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetString(), type_table)
						== EQUIVALENT) {
					string right_value = *(right_result->GetData<string>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else {
					assert(false);
				}
			} else if (left_type->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetInt(), type_table)
					== EQUIVALENT) {
				int left_value = *(left_result->GetData<int>());

				if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetBoolean(), type_table)
						== EQUIVALENT) {
					bool right_value = *(right_result->GetData<bool>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetInt(), type_table)
						== EQUIVALENT) {
					int right_value = *(right_result->GetData<int>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetDouble(), type_table)
						== EQUIVALENT) {
					double right_value = *(right_result->GetData<double>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetString(), type_table)
						== EQUIVALENT) {
					string right_value = *(right_result->GetData<string>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else {
					assert(false);
				}
			} else if (left_type->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetDouble(), type_table)
					== EQUIVALENT) {
				double left_value = *(left_result->GetData<double>());

				if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetBoolean(), type_table)
						== EQUIVALENT) {
					bool right_value = *(right_result->GetData<bool>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetInt(), type_table)
						== EQUIVALENT) {
					int right_value = *(right_result->GetData<int>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetDouble(), type_table)
						== EQUIVALENT) {
					double right_value = *(right_result->GetData<double>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetString(), type_table)
						== EQUIVALENT) {
					string right_value = *(right_result->GetData<string>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else {
					assert(false);
				}
			} else if (left_type->AnalyzeAssignmentTo(
					PrimitiveTypeSpecifier::GetString(), type_table)
					== EQUIVALENT) {
				string left_value = *(left_result->GetData<string>());

				if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetBoolean(), type_table)
						== EQUIVALENT) {
					bool right_value = *(right_result->GetData<bool>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetInt(), type_table)
						== EQUIVALENT) {
					int right_value = *(right_result->GetData<int>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetDouble(), type_table)
						== EQUIVALENT) {
					double right_value = *(right_result->GetData<double>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else if (right_type->AnalyzeAssignmentTo(
						PrimitiveTypeSpecifier::GetString(), type_table)
						== EQUIVALENT) {
					string right_value = *(right_result->GetData<string>());
					return compute(left_value, right_value, left_position,
							right_position);
				} else {
					assert(false);
				}
			} else {
				assert(false);
			}
		}
	}

	return make_shared<Result>(nullptr, errors);
}

TypedResult<TypeSpecifier> BinaryExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return ComputeResultType(m_left, m_right, m_operator, execution_context);
}

const ErrorListRef BinaryExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<TypeSpecifier> valid_left,
		const_shared_ptr<TypeSpecifier> valid_right) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const OperatorType op = GetOperator();

	auto left = GetLeft();
	auto left_errors = left->Validate(execution_context);
	if (ErrorList::IsTerminator(left_errors)) {
		auto left_type_specifier_result = left->GetTypeSpecifier(
				execution_context);
		auto left_type_specifier_errors =
				left_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(left_type_specifier_errors)) {
			auto left_type_specifier = left_type_specifier_result.GetData();
			auto left_analysis = left_type_specifier->AnalyzeAssignmentTo(
					valid_left, execution_context->GetTypeTable());
			if (left_analysis != EQUIVALENT && left_analysis != UNAMBIGUOUS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::INVALID_LEFT_OPERAND_TYPE,
								left->GetLocation().begin.line,
								left->GetLocation().begin.column,
								OperatorToString(op)), errors);
			}
		} else {
			errors = ErrorList::Concatenate(errors, left_type_specifier_errors);
		}
	} else {
		errors = ErrorList::Concatenate(errors, left_errors);
	}

	auto right = GetRight();
	auto right_errors = right->Validate(execution_context);
	if (ErrorList::IsTerminator(right_errors)) {
		auto right_type_specifier_result = GetRight()->GetTypeSpecifier(
				execution_context);
		auto right_type_specifier_errors =
				right_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(right_type_specifier_errors)) {
			auto right_type_specifier = right_type_specifier_result.GetData();
			auto right_analysis = right_type_specifier->AnalyzeAssignmentTo(
					valid_right, execution_context->GetTypeTable());
			if (right_analysis != EQUIVALENT && right_analysis != UNAMBIGUOUS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::INVALID_RIGHT_OPERAND_TYPE,
								right->GetLocation().begin.line,
								right->GetLocation().begin.column,
								OperatorToString(op)), errors);
			}
		} else {
			errors = ErrorList::Concatenate(errors,
					right_type_specifier_errors);
		}
	} else {
		errors = ErrorList::Concatenate(errors, right_errors);
	}

	return errors;
}

const_shared_ptr<Result> BinaryExpression::compute(const bool& left,
		const int& right, yy::location left_position,
		yy::location right_position) const {
	int converted_left = left;
	return compute(converted_left, right, left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const bool& left,
		const double& right, yy::location left_position,
		yy::location right_position) const {
	double converted_left = left;
	return compute(converted_left, right, left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const bool& left,
		const string& right, yy::location left_position,
		yy::location right_position) const {
	return compute(*AsString(left), right, left_position, right_position);
}

const_shared_ptr<Result> BinaryExpression::compute(const int& left,
		const bool& right, yy::location left_position,
		yy::location right_position) const {
	int converted_right = right;
	return compute(left, converted_right, left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const int& left,
		const double& right, yy::location left_position,
		yy::location right_position) const {
	double converted_left = left;
	return compute(converted_left, right, left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const int& left,
		const string& right, yy::location left_position,
		yy::location right_position) const {
	return compute(*AsString(left), right, left_position, right_position);
}

const_shared_ptr<Result> BinaryExpression::compute(const double& left,
		const bool& right, yy::location left_position,
		yy::location right_position) const {
	double converted_right = right;
	return compute(left, converted_right, left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const double& left,
		const int& right, yy::location left_position,
		yy::location right_position) const {
	double converted_right = right;
	return compute(left, converted_right, left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const double& left,
		const string& right, yy::location left_position,
		yy::location right_position) const {
	return compute(*AsString(left), right, left_position, right_position);
}

const_shared_ptr<Result> BinaryExpression::compute(const string& left,
		const bool& right, yy::location left_position,
		yy::location right_position) const {
	return compute(left, *AsString(right), left_position, right_position);
}
const_shared_ptr<Result> BinaryExpression::compute(const string& left,
		const int& right, yy::location left_position,
		yy::location right_position) const {
	return compute(left, *AsString(right), left_position, right_position);
}

const_shared_ptr<Result> BinaryExpression::compute(const string& left,
		const double& right, yy::location left_position,
		yy::location right_position) const {
	return compute(left, *AsString(right), left_position, right_position);
}

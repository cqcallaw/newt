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

const_shared_ptr<TypeSpecifier> BinaryExpression::ComputeResultType(
		const_shared_ptr<Expression> left, const_shared_ptr<Expression> right,
		const OperatorType op,
		const shared_ptr<ExecutionContext> execution_context) {
	const_shared_ptr<TypeSpecifier> left_type = left->GetTypeSpecifier(execution_context,
			AliasResolution::RESOLVE);
	const_shared_ptr<TypeSpecifier> right_type = right->GetTypeSpecifier(
			execution_context, AliasResolution::RESOLVE);

	const_shared_ptr<PrimitiveTypeSpecifier> left_as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(left_type);
	const_shared_ptr<PrimitiveTypeSpecifier> right_as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(right_type);

	if (left_as_primitive == nullptr || right_as_primitive == nullptr
			|| left_as_primitive == PrimitiveTypeSpecifier::GetNone()
			|| right_as_primitive == PrimitiveTypeSpecifier::GetNone()) {
		return PrimitiveTypeSpecifier::GetNone();
	}

	const BasicType left_basic_type = left_as_primitive->GetBasicType();
	const BasicType right_basic_type = right_as_primitive->GetBasicType();

	if (op == EQUAL || op == NOT_EQUAL || op == LESS_THAN
			|| op == LESS_THAN_EQUAL || op == GREATER_THAN
			|| op == GREATER_THAN_EQUAL || op == AND || op == OR) {
		return PrimitiveTypeSpecifier::GetBoolean();
	}

	if (op == MOD) {
		return PrimitiveTypeSpecifier::GetInt();
	}

	if (right_basic_type >= left_basic_type)
		return right_type;
	else
		return left_type;

	cerr << "Invalid type combination <" << left_type << "> and <" << right_type
			<< ">\n";
	assert(false);
	return PrimitiveTypeSpecifier::GetNone();
}

const_shared_ptr<Result> BinaryExpression::Evaluate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	const_shared_ptr<Expression> left = GetLeft();
	const_shared_ptr<Expression> right = GetRight();

	const_shared_ptr<Result> left_result = left->Evaluate(execution_context);
	if (!ErrorList::IsTerminator(left_result->GetErrors())) {
		return left_result;
	}

	const_shared_ptr<Result> right_result = right->Evaluate(execution_context);
	if (!ErrorList::IsTerminator(right_result->GetErrors())) {
		return right_result;
	}

	const_shared_ptr<TypeSpecifier> left_type = left->GetTypeSpecifier(
			execution_context);
	const_shared_ptr<TypeSpecifier> right_type = right->GetTypeSpecifier(
			execution_context);

	yy::location left_position = left->GetPosition();
	yy::location right_position = right->GetPosition();

	auto type_table = execution_context->GetTypeTable();

	if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean(),
			type_table)) {
		bool left_value = *(left_result->GetData<bool>());

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean(),
				type_table)) {
			bool right_value = *(right_result->GetData<bool>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt(),
				type_table)) {
			int right_value = *(right_result->GetData<int>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble(), type_table)) {
			double right_value = *(right_result->GetData<double>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString(), type_table)) {
			string right_value = *(right_result->GetData<string>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt(),
			type_table)) {
		int left_value = *(left_result->GetData<int>());

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean(),
				type_table)) {
			bool right_value = *(right_result->GetData<bool>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt(),
				type_table)) {
			int right_value = *(right_result->GetData<int>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble(), type_table)) {
			double right_value = *(right_result->GetData<double>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString(), type_table)) {
			string right_value = *(right_result->GetData<string>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetDouble(),
			type_table)) {
		double left_value = *(left_result->GetData<double>());

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean(),
				type_table)) {
			bool right_value = *(right_result->GetData<bool>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt(),
				type_table)) {
			int right_value = *(right_result->GetData<int>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble(), type_table)) {
			double right_value = *(right_result->GetData<double>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString(), type_table)) {
			string right_value = *(right_result->GetData<string>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetString(),
			type_table)) {
		string left_value = *(left_result->GetData<string>());

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean(),
				type_table)) {
			bool right_value = *(right_result->GetData<bool>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt(),
				type_table)) {
			int right_value = *(right_result->GetData<int>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble(), type_table)) {
			double right_value = *(right_result->GetData<double>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString(), type_table)) {
			string right_value = *(right_result->GetData<string>());
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else {
		assert(false);
	}

	return make_shared<Result>(const_shared_ptr<const void>(), errors);
}

const_shared_ptr<TypeSpecifier> BinaryExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return ComputeResultType(m_left, m_right, m_operator, execution_context);
}

const ErrorListRef BinaryExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<TypeSpecifier> valid_left,
		const_shared_ptr<TypeSpecifier> valid_right) const {
	ErrorListRef result = ErrorList::GetTerminator();

	const OperatorType op = GetOperator();
	const_shared_ptr<Expression> left = GetLeft();

	ErrorListRef left_errors = left->Validate(execution_context);
	if (!ErrorList::IsTerminator(left_errors)) {
		result = ErrorList::Concatenate(result, left_errors);
		return result;
	}

	const_shared_ptr<TypeSpecifier> left_type = left->GetTypeSpecifier(
			execution_context);
	if (!(left_type->IsAssignableTo(valid_left,
			execution_context->GetTypeTable()))) {
		result = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_LEFT_OPERAND_TYPE,
						left->GetPosition().begin.line,
						left->GetPosition().begin.column,
						operator_to_string(op)), result);
	}
	const_shared_ptr<Expression> right = GetRight();
	const_shared_ptr<TypeSpecifier> right_type = GetRight()->GetTypeSpecifier(
			execution_context);

	ErrorListRef right_errors = right->Validate(execution_context);
	if (!ErrorList::IsTerminator(right_errors)) {
		result = ErrorList::Concatenate(result, right_errors);
		return result;
	}

	result = ErrorList::Concatenate(result, right->Validate(execution_context));
	if (!(right_type->IsAssignableTo(valid_right,
			execution_context->GetTypeTable()))) {
		result = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_RIGHT_OPERAND_TYPE,
						right->GetPosition().begin.line,
						right->GetPosition().begin.column,
						operator_to_string(op)), result);
	}

	return result;
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

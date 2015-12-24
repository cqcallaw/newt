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
		const_shared_ptr<ExecutionContext> execution_context) {
	const_shared_ptr<TypeSpecifier> left_type = left->GetType(
			execution_context);
	const_shared_ptr<TypeSpecifier> right_type = right->GetType(
			execution_context);

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

const Result* BinaryExpression::Evaluate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();
	const_shared_ptr<Expression> left = GetLeft();
	const_shared_ptr<Expression> right = GetRight();

	const Result* left_result = left->Evaluate(execution_context);
	if (!ErrorListBase::IsTerminator(left_result->GetErrors())) {
		return left_result;
	}

	const Result* right_result = right->Evaluate(execution_context);
	if (!ErrorListBase::IsTerminator(right_result->GetErrors())) {
		return right_result;
	}

	const_shared_ptr<TypeSpecifier> left_type = left->GetType(
			execution_context);
	const_shared_ptr<TypeSpecifier> right_type = right->GetType(
			execution_context);

	yy::location left_position = left->GetPosition();
	yy::location right_position = right->GetPosition();

	if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean())) {
		bool left_value = *(static_pointer_cast<const bool>(
				left_result->GetData()));

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean())) {
			bool right_value = *(static_pointer_cast<const bool>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			int right_value = *(static_pointer_cast<const int>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			double right_value = *(static_pointer_cast<const double>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			string right_value = *(static_pointer_cast<const string>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetInt())) {
		int left_value = *(static_pointer_cast<const int>(
				left_result->GetData()));

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean())) {
			bool right_value = *(static_pointer_cast<const bool>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			int right_value = *(static_pointer_cast<const int>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			double right_value = *(static_pointer_cast<const double>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			string right_value = *(static_pointer_cast<const string>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetDouble())) {
		double left_value = *(static_pointer_cast<const double>(
				left_result->GetData()));

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean())) {
			bool right_value = *(static_pointer_cast<const bool>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			int right_value = *(static_pointer_cast<const int>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			double right_value = *(static_pointer_cast<const double>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			string right_value = *(static_pointer_cast<const string>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else if (left_type->IsAssignableTo(PrimitiveTypeSpecifier::GetString())) {
		string left_value = *(static_pointer_cast<const string>(
				left_result->GetData()));

		if (right_type->IsAssignableTo(PrimitiveTypeSpecifier::GetBoolean())) {
			bool right_value = *(static_pointer_cast<const bool>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {
			int right_value = *(static_pointer_cast<const int>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetDouble())) {
			double right_value = *(static_pointer_cast<const double>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else if (right_type->IsAssignableTo(
				PrimitiveTypeSpecifier::GetString())) {
			string right_value = *(static_pointer_cast<const string>(
					right_result->GetData()));
			return compute(left_value, right_value, left_position,
					right_position);
		} else {
			assert(false);
		}
	} else {
		assert(false);
	}

	delete (left_result);
	delete (right_result);

	return new Result(const_shared_ptr<const void>(), errors);
}

const_shared_ptr<TypeSpecifier> BinaryExpression::GetType(
		const_shared_ptr<ExecutionContext> execution_context) const {
	return ComputeResultType(m_left, m_right, m_operator, execution_context);
}

const ErrorList BinaryExpression::Validate(
		const_shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<TypeSpecifier> valid_left,
		const_shared_ptr<TypeSpecifier> valid_right) const {
	ErrorList result = ErrorListBase::GetTerminator();

	const OperatorType op = GetOperator();
	const_shared_ptr<Expression> left = GetLeft();

	ErrorList left_errors = left->Validate(execution_context);
	if (!ErrorListBase::IsTerminator(left_errors)) {
		result = ErrorListBase::Concatenate(result, left_errors);
		return result;
	}

	const_shared_ptr<TypeSpecifier> left_type = left->GetType(
			execution_context);
	if (!(left_type->IsAssignableTo(valid_left))) {
		result = ErrorListBase::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_LEFT_OPERAND_TYPE,
						left->GetPosition().begin.line,
						left->GetPosition().begin.column,
						operator_to_string(op)), result);
	}
	const_shared_ptr<Expression> right = GetRight();
	const_shared_ptr<TypeSpecifier> right_type = GetRight()->GetType(
			execution_context);

	ErrorList right_errors = right->Validate(execution_context);
	if (!ErrorListBase::IsTerminator(right_errors)) {
		result = ErrorListBase::Concatenate(result, right_errors);
		return result;
	}

	result = ErrorListBase::Concatenate(result,
			right->Validate(execution_context));
	if (!(right_type->IsAssignableTo(valid_right))) {
		result = ErrorListBase::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_RIGHT_OPERAND_TYPE,
						right->GetPosition().begin.line,
						right->GetPosition().begin.column,
						operator_to_string(op)), result);
	}

	return result;
}

const Result* BinaryExpression::compute(bool& left, int& right,
		yy::location left_position, yy::location right_position) const {
	int converted_left = left;
	return compute(converted_left, right, left_position, right_position);
}
const Result* BinaryExpression::compute(bool& left, double& right,
		yy::location left_position, yy::location right_position) const {
	double converted_left = left;
	return compute(converted_left, right, left_position, right_position);
}
const Result* BinaryExpression::compute(bool& left, string& right,
		yy::location left_position, yy::location right_position) const {
	return compute(*AsString(left), right, left_position, right_position);
}

const Result* BinaryExpression::compute(int& left, bool& right,
		yy::location left_position, yy::location right_position) const {
	int converted_right = right;
	return compute(left, converted_right, left_position, right_position);
}
const Result* BinaryExpression::compute(int& left, double& right,
		yy::location left_position, yy::location right_position) const {
	double converted_left = left;
	return compute(converted_left, right, left_position, right_position);
}
const Result* BinaryExpression::compute(int& left, string& right,
		yy::location left_position, yy::location right_position) const {
	return compute(*AsString(left), right, left_position, right_position);
}

const Result* BinaryExpression::compute(double& left, bool& right,
		yy::location left_position, yy::location right_position) const {
	double converted_right = right;
	return compute(left, converted_right, left_position, right_position);
}
const Result* BinaryExpression::compute(double& left, int& right,
		yy::location left_position, yy::location right_position) const {
	double converted_right = right;
	return compute(left, converted_right, left_position, right_position);
}
const Result* BinaryExpression::compute(double& left, string& right,
		yy::location left_position, yy::location right_position) const {
	return compute(*AsString(left), right, left_position, right_position);
}

const Result* BinaryExpression::compute(string& left, bool& right,
		yy::location left_position, yy::location right_position) const {
	return compute(left, *AsString(right), left_position, right_position);
}
const Result* BinaryExpression::compute(string& left, int& right,
		yy::location left_position, yy::location right_position) const {
	return compute(left, *AsString(right), left_position, right_position);
}

const Result* BinaryExpression::compute(string& left, double& right,
		yy::location left_position, yy::location right_position) const {
	return compute(left, *AsString(right), left_position, right_position);
}

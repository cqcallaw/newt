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

BinaryExpression::BinaryExpression(const YYLTYPE position,
		const OperatorType op, const Expression* left, const Expression* right) :
		Expression(position), m_operator(op), m_left(left), m_right(right) {
	assert(left != NULL);
	assert(right != NULL);
}

const Type BinaryExpression::ComputeResultType(const Expression* left,
		const Expression* right, const OperatorType op,
		const ExecutionContext* execution_context) {
	Type left_type = left->GetType(execution_context);
	Type right_type = right->GetType(execution_context);

	if (left_type == NONE || right_type == NONE) {
		return NONE;
	}

	if (op == EQUAL || op == NOT_EQUAL || op == LESS_THAN
			|| op == LESS_THAN_EQUAL || op == GREATER_THAN
			|| op == GREATER_THAN_EQUAL || op == AND || op == OR
			|| op == TOUCHES || op == NEAR) {
		return BOOLEAN;
	}

	if (right_type >= left_type)
		return right_type;
	else
		return left_type;

	cerr << "Invalid type combination <" << left_type << "> and <" << right_type
			<< ">\n";
	assert(false);
	return NONE;
}

const void* BinaryExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	Type left_type = GetLeft()->GetType(execution_context);
	Type right_type = GetRight()->GetType(execution_context);

	const void* left_void_value = GetLeft()->Evaluate(execution_context);
	const void* right_void_value = GetRight()->Evaluate(execution_context);

	switch (left_type) {
	case BOOLEAN: {
		bool left_value = *((bool*) left_void_value);

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_void_value);
			return compute(left_value, right_value);
		}
		case INT: {
			int right_value = *((int*) right_void_value);
			return compute(left_value, right_value);
		}
		case DOUBLE: {
			double right_value = *((double*) right_void_value);
			return compute(left_value, right_value);
		}
		case STRING: {
			string* right_value = (string*) right_void_value;
			return compute(left_value, right_value);
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	}
	case INT: {
		int left_value = *((int*) left_void_value);

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_void_value);
			return compute(left_value, right_value);
		}
		case INT: {
			int right_value = *((int*) right_void_value);
			return compute(left_value, right_value);
		}
		case DOUBLE: {
			double right_value = *((double*) right_void_value);
			return compute(left_value, right_value);
		}
		case STRING: {
			string* right_value = (string*) right_void_value;
			return compute(left_value, right_value);
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	}
	case DOUBLE: {
		double left_value = *((double*) left_void_value);

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_void_value);
			return compute(left_value, right_value);
		}
		case INT: {
			int right_value = *((int*) right_void_value);
			return compute(left_value, right_value);
		}
		case DOUBLE: {
			double right_value = *((double*) right_void_value);
			return compute(left_value, right_value);
		}
		case STRING: {
			string* right_value = (string*) right_void_value;
			return compute(left_value, right_value);
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	}
	case STRING: {
		string* left_value = (string*) left_void_value;

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_void_value);
			return compute(left_value, right_value);
		}
		case INT: {
			int right_value = *((int*) right_void_value);
			return compute(left_value, right_value);
		}
		case DOUBLE: {
			double right_value = *((double*) right_void_value);
			return compute(left_value, right_value);
		}
		case STRING: {
			string* right_value = (string*) right_void_value;
			return compute(left_value, right_value);
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	}
	default:
		assert(false);
		return NULL;
	}

	assert(false);
	return NULL;
}

const Type BinaryExpression::GetType(
		const ExecutionContext* execution_context) const {
	return ComputeResultType(m_left, m_right, m_operator, execution_context);
}

const LinkedList<const Error*>* BinaryExpression::Validate(
		const ExecutionContext* execution_context, int valid_left,
		int valid_right) const {
	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;

	const OperatorType op = GetOperator();
	const Expression* left = GetLeft();

	const LinkedList<const Error*>* left_errors = left->Validate(
			execution_context);
	if (left_errors != LinkedList<const Error*>::Terminator) {
		result = (LinkedList<const Error*>*) result->Concatenate(left_errors,
				true);
		return result;
	}

	Type left_type = left->GetType(execution_context);
	if (!(left_type & (valid_left))) {
		result = (LinkedList<const Error*>*) result->With(
				new Error(Error::SEMANTIC, Error::INVALID_LEFT_OPERAND_TYPE,
						left->GetPosition().first_line,
						left->GetPosition().first_column,
						operator_to_string(op)));
	}
	const Expression* right = GetRight();
	Type right_type = GetRight()->GetType(execution_context);

	const LinkedList<const Error*>* right_errors = right->Validate(
			execution_context);
	if (right_errors != LinkedList<const Error*>::Terminator) {
		result = (LinkedList<const Error*>*) result->Concatenate(right_errors,
				true);
		return result;
	}

	result = (LinkedList<const Error*>*) result->Concatenate(
			right->Validate(execution_context), true);
	if (!(right_type & (valid_right))) {
		result = (LinkedList<const Error*>*) result->With(
				new Error(Error::SEMANTIC, Error::INVALID_RIGHT_OPERAND_TYPE,
						right->GetPosition().first_line,
						right->GetPosition().first_column,
						operator_to_string(op)));
	}

	return result;
}

const void* BinaryExpression::compute(bool left, int right) const {
	return compute((int) left, right);
}
const void* BinaryExpression::compute(bool left, double right) const {
	return compute((double) left, right);
}
const void* BinaryExpression::compute(bool left, string* right) const {
	return compute(AsString(left), right);
}

const void* BinaryExpression::compute(int left, bool right) const {
	return compute(left, (int) right);
}
const void* BinaryExpression::compute(int left, double right) const {
	return compute((double) left, right);
}
const void* BinaryExpression::compute(int left, string* right) const {
	return compute(AsString(left), right);
}

const void* BinaryExpression::compute(double left, bool right) const {
	return compute(left, (double) right);
}
const void* BinaryExpression::compute(double left, int right) const {
	return compute(left, (double) right);
}
const void* BinaryExpression::compute(double left, string* right) const {
	return compute(AsString(left), right);
}

const void* BinaryExpression::compute(string* left, bool right) const {
	return compute(left, AsString(right));
}
const void* BinaryExpression::compute(string* left, int right) const {
	return compute(left, AsString(right));
}

const void* BinaryExpression::compute(string* left, double right) const {
	return compute(left, AsString(right));
}

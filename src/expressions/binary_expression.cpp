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

const PrimitiveType BinaryExpression::ComputeResultType(const Expression* left,
		const Expression* right, const OperatorType op,
		const ExecutionContext* execution_context) {
	PrimitiveType left_type = left->GetType(execution_context);
	PrimitiveType right_type = right->GetType(execution_context);

	if (left_type == NONE || right_type == NONE) {
		return NONE;
	}

	if (op == EQUAL || op == NOT_EQUAL || op == LESS_THAN
			|| op == LESS_THAN_EQUAL || op == GREATER_THAN
			|| op == GREATER_THAN_EQUAL || op == AND || op == OR) {
		return BOOLEAN;
	}

	if (op == MOD) {
		return INT;
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

const Result* BinaryExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;
	void* result = nullptr;

	const Expression* left = GetLeft();
	const Expression* right = GetRight();

	PrimitiveType left_type = left->GetType(execution_context);
	PrimitiveType right_type = right->GetType(execution_context);

	YYLTYPE left_position = left->GetPosition();
	YYLTYPE right_position = right->GetPosition();

	const Result* left_result = left->Evaluate(execution_context);

	if (left_result->GetErrors() != LinkedList<const Error*>::Terminator) {
		return left_result;
	}

	const Result* right_result = right->Evaluate(execution_context);

	if (right_result->GetErrors() != LinkedList<const Error*>::Terminator) {
		return right_result;
	}

	switch (left_type) {
	case BOOLEAN: {
		bool left_value = *((bool*) left_result->GetData());

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case INT: {
			int right_value = *((int*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case DOUBLE: {
			double right_value = *((double*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case STRING: {
			string* right_value = (string*) right_result->GetData();
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		default:
			assert(false);
		}
		break;
	}
	case INT: {
		int left_value = *((int*) left_result->GetData());

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case INT: {
			int right_value = *((int*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case DOUBLE: {
			double right_value = *((double*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case STRING: {
			string* right_value = (string*) right_result->GetData();
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		default:
			assert(false);
		}
		break;
	}
	case DOUBLE: {
		double left_value = *((double*) left_result->GetData());

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case INT: {
			int right_value = *((int*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case DOUBLE: {
			double right_value = *((double*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case STRING: {
			string* right_value = (string*) right_result->GetData();
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	}
	case STRING: {
		string* left_value = (string*) left_result->GetData();

		switch (right_type) {
		case BOOLEAN: {
			bool right_value = *((bool*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case INT: {
			int right_value = *((int*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case DOUBLE: {
			double right_value = *((double*) right_result->GetData());
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		case STRING: {
			string* right_value = (string*) right_result->GetData();
			return compute(left_value, right_value, left_position,
					right_position);
			break;
		}
		default:
			assert(false);
		}
		break;
	}
	default:
		assert(false);
	}

	delete (left_result);
	delete (right_result);

	return new Result(result, errors);
}

const PrimitiveType BinaryExpression::GetType(
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

	PrimitiveType left_type = left->GetType(execution_context);
	if (!(left_type & (valid_left))) {
		result = (LinkedList<const Error*>*) result->With(
				new Error(Error::SEMANTIC, Error::INVALID_LEFT_OPERAND_TYPE,
						left->GetPosition().first_line,
						left->GetPosition().first_column,
						operator_to_string(op)));
	}
	const Expression* right = GetRight();
	PrimitiveType right_type = GetRight()->GetType(execution_context);

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

const Result* BinaryExpression::compute(bool left, int right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute((int) left, right, left_position, right_position);
}
const Result* BinaryExpression::compute(bool left, double right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute((double) left, right, left_position, right_position);
}
const Result* BinaryExpression::compute(bool left, string* right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(AsString(left), right, left_position, right_position);
}

const Result* BinaryExpression::compute(int left, bool right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(left, (int) right, left_position, right_position);
}
const Result* BinaryExpression::compute(int left, double right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute((double) left, right, left_position, right_position);
}
const Result* BinaryExpression::compute(int left, string* right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(AsString(left), right, left_position, right_position);
}

const Result* BinaryExpression::compute(double left, bool right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(left, (double) right, left_position, right_position);
}
const Result* BinaryExpression::compute(double left, int right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(left, (double) right, left_position, right_position);
}
const Result* BinaryExpression::compute(double left, string* right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(AsString(left), right, left_position, right_position);
}

const Result* BinaryExpression::compute(string* left, bool right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(left, AsString(right), left_position, right_position);
}
const Result* BinaryExpression::compute(string* left, int right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(left, AsString(right), left_position, right_position);
}

const Result* BinaryExpression::compute(string* left, double right,
YYLTYPE left_position, YYLTYPE right_position) const {
	return compute(left, AsString(right), left_position, right_position);
}

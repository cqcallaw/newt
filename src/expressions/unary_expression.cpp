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

UnaryExpression::UnaryExpression(const YYLTYPE position, const OperatorType op,
		const Expression* expression) :
		Expression(position), m_expression(expression), m_operator(op) {
	assert(expression != NULL);
}

const Type UnaryExpression::compute_result_type(const Type input_type,
		const OperatorType op) {
	switch (op) {
	case ABS:
	case UNARY_MINUS:
		return input_type;
	case SIN:
	case COS:
	case TAN:
	case ASIN:
	case ACOS:
	case ATAN:
	case SQRT:
		return DOUBLE;
	case FLOOR:
		return INT;
	case RANDOM:
		return INT;
	case NOT:
		return BOOLEAN;
	default:
		assert(false);
		return NONE;
	}
}

const void* UnaryExpression::compute(const Type input_type, const void* input,
		double (*compute_function)(double),
		double (*input_transform_function)(double),
		double (*output_transform_function)(double)) {
	switch (input_type) {
	case INT: {
		int as_int = *((int*) input);
		double* result = new double;

		*result =
				input_transform_function != NULL ?
						compute_function(input_transform_function(as_int)) :
						compute_function(as_int);

		if (output_transform_function != NULL) {
			*result = output_transform_function(*result);
		}

		return (void *) result;
	}
	case DOUBLE: {
		double casted = *((double*) input);
		double* result = new double;
		*result =
				input_transform_function != NULL ?
						compute_function(input_transform_function(casted)) :
						compute_function(casted);

		if (output_transform_function != NULL) {
			*result = output_transform_function(*result);
		}

		return (void *) result;
	}
	default:
		assert(false);
		return NULL;
	}
}

double UnaryExpression::degrees_to_radians(double angle) {
	return angle * (M_PI / 180.0);
}

const Type UnaryExpression::GetType(
		const ExecutionContext* execution_context) const {
	return compute_result_type(m_expression->GetType(execution_context),
			m_operator);
}

const LinkedList<const Error*>* UnaryExpression::Validate(
		const ExecutionContext* execution_context) const {
	/*switch (expression->GetType()) {
	 case INT:
	 case DOUBLE:
	 assert(
	 op == NOT || op == UNARY_MINUS || op == SIN || op == COS
	 || op == TAN || op == ASIN || op == ACOS || op == ATAN
	 || op == SQRT || op == FLOOR || op == ABS
	 || op == RANDOM);
	 break;
	 case BOOLEAN: {
	 assert(op == NOT);
	 break;
	 }
	 default:
	 assert(false);
	 }*/
	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;
	return result;
}

double UnaryExpression::radians_to_degrees(double radians) {
	return radians * (180.0 / M_PI);
}

const void* UnaryExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const Type expression_type = m_expression->GetType(execution_context);
	const void* evaluated = m_expression->Evaluate(execution_context);

	switch (m_operator) {
	case UNARY_MINUS: {
		switch (expression_type) {
		case INT: {
			int* value = new int;
			*value = -(*((int*) evaluated));
			//cout << "Integer negation result: " << *value << "\n";
			return (void *) value;
		}
		case DOUBLE: {
			double* value = new double;
			*value = -(*((double*) evaluated));
			//cout << "Floating point negation result: " << *value << "\n";
			return (void *) value;
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	}
	case SIN:
		return compute(expression_type, evaluated, sin, degrees_to_radians);
		break;
	case COS:
		return compute(expression_type, evaluated, cos, degrees_to_radians);
		break;
	case TAN:
		return compute(expression_type, evaluated, tan, degrees_to_radians);
		break;
	case ASIN:
		return compute(expression_type, evaluated, asin, NULL,
				radians_to_degrees);
		break;
	case ACOS:
		return compute(expression_type, evaluated, acos, NULL,
				radians_to_degrees);
		break;
	case ATAN:
		return compute(expression_type, evaluated, atan, NULL,
				radians_to_degrees);
		break;
	case SQRT:
		return compute(expression_type, evaluated, sqrt);
		break;
	case FLOOR:
		switch (expression_type) {
		case INT: {
			int* value = new int(floor(*((int*) evaluated)));
			return (void *) value;
		}
		case DOUBLE: {
			int* value = new int(floor(*((double*) evaluated)));
			return (void *) value;
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	case ABS:
		switch (expression_type) {
		case INT: {
			int* value = new int;
			*value = abs(*((int*) evaluated));
			return (void *) value;
		}
		case DOUBLE: {
			double old_value = *((double*) evaluated);
			double* value = new double;
			*value = old_value < 0 ? -old_value : old_value;
			return (void *) value;
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	case RANDOM:
		switch (expression_type) {
		case INT: {
			int* value = new int;
			*value = rand() % (*((int*) evaluated));
			return (void *) value;
		}
		case DOUBLE: {
			double* value = new double;
			*value = rand() % int(floor(*((double*) evaluated)));
			return (void *) value;
		}
		default:
			assert(false);
			return NULL;
		}
		break;
	case NOT: {
		switch (expression_type) {
		case BOOLEAN: {
			bool old_value = *((bool*) evaluated);
			bool* value = new bool(!old_value);
			//cout << "Boolean NOT result: " << *value << "\n";
			return (void *) value;
		}
		case INT: {
			int old_value = *((int*) evaluated);
			bool* value = new bool(!(old_value != 0));
			//cout << "Integer NOT result: " << *value << "\n";
			return (void *) value;
		}
		case DOUBLE: {
			double old_value = *((double*) evaluated);
			bool* value = new bool(!(old_value != 0));
			//cout << "Floating point NOT result: " << *value << "\n";
			return (void *) value;
		}
		default:
			assert(false);
			return NULL;
		}
		break;

	}
	default:
		assert(false);
		return nullptr;
	}
}


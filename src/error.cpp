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

#include "error.h"

#include <iostream>
#include <sstream>
#include <assert.h>

Error::Error(ErrorClass error_class, ErrorCode code, int line_number,
		int column_number, string s1, string s2, string s3) :
		m_error_class(error_class), m_code(code), m_line_number(line_number), m_column_number(
				column_number), m_s1(s1), m_s2(s2), m_s3(s3) {
}

const string Error::ToString() const {
	ostringstream os;

	switch (m_error_class) {
	case SEMANTIC: {
		os << "Semantic error on line " << m_line_number << ", column "
				<< m_column_number << ": " << get_error_message();
		break;
	}
	case RUNTIME: {
		os << "Runtime error on line " << m_line_number << ", column "
				<< m_column_number << ": " << get_error_message();
		break;
	}
	default:
		assert(false);
	}

	return os.str();
}

const string Error::get_error_message() const {
	ostringstream os;

	switch (m_code) {
	case ARRAY_INDEX_MUST_BE_AN_INTEGER:
		// m_s2 is expected to be one of the following strings
		//     "A double expression"
		//     "A string expression"
		os << m_s2 << " is not a legal array index.  The array is '" << m_s1
				<< "'.";
		break;
	case ARRAY_INDEX_OUT_OF_BOUNDS:
		os << "Index value '" << m_s2 << "' is out of bounds for array '"
				<< m_s1 << "'.";
		break;
	case ASSIGNMENT_TYPE_ERROR:
		os << "Cannot assign an expression of type '" << m_s2
				<< "' to a variable of type '" << m_s1 << "'.";
		break;
	case AMBIGUOUS_WIDENING_CONVERSION:
		os << "Cannot unambiguously widen an expression of type '" << m_s2
				<< "' to a variable of type '" << m_s1 << "'.";
		break;
	case EXIT_STATUS_MUST_BE_AN_INTEGER:
		os << "Value passed to exit() must be an integer.  "
				<< "Value passed was of type '" << m_s1 << "'.";
		break;
	case ARRAY_SIZE_MISMATCH:
		os << "The size of array '" << m_s1
				<< "' must match the size of the right-hand side of the assignment.";
		break;
	case INVALID_LHS_OF_ASSIGNMENT:
		os
				<< "Left-hand side of assignment must be of type INT, DOUBLE, or STRING."
				<< "  Variable '" << m_s1 << "' is of type '" << m_s2 << "'.";
		break;
	case INVALID_LHS_OF_MINUS_ASSIGNMENT:
		os << "LHS of minus-assignment must be (INT || DOUBLE)."
				<< "  Variable '" << m_s1 << "' is of type '" << m_s2 << "'.";
		break;
	case INVALID_LHS_OF_PLUS_ASSIGNMENT:
		os << "LHS of plus-assignment must be (INT || DOUBLE || STRING)."
				<< "  Variable '" << m_s1 << "' is of type '" << m_s2 << "'.";
		break;
	case INVALID_LEFT_OPERAND_TYPE:
		os << "Invalid left operand for operator '" << m_s1 << "'.";
		break;
	case INVALID_RIGHT_OPERAND_TYPE:
		os << "Invalid right operand for operator '" << m_s1 << "'.";
		break;
	case INVALID_INITIALIZER_TYPE:
		os << "Variable '" << m_s1 << "' of type '" << m_s2
				<< "' cannot be assigned to an expression of type '" << m_s3
				<< "'.";
		break;
	case INVALID_TYPE_FOR_FOR_STMT_EXPRESSION:
		os << "Incorrect type for expression in for statement."
				<< "  Expressions in for statements must be of type INT.";
		break;
	case INVALID_TYPE_FOR_IF_STMT_EXPRESSION:
		os << "Incorrect type for expression in an if statement."
				<< "  Expressions in if statements must be of type INT.";
		break;
	case INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION:
		os << "Incorrect type for expression in a print statement."
				<< "  Expressions in print statements must be"
				<< " of type INT, DOUBLE, or STRING.";
		break;
	case INVALID_ARRAY_TYPE:
		os << "Invalid array type '" << m_s1 << "' for variable '" << m_s2
				<< "'.";
		break;
	case LHS_OF_PERIOD_MUST_BE_OBJECT:
		os << "Variable '" << m_s1 << "' is not an object."
				<< "  Only objects may be on the left of a period.";
		break;
	case MINUS_ASSIGNMENT_TYPE_ERROR:
		os << "Cannot -= an expression of type '" << m_s2
				<< "' from a variable of type '" << m_s1 << "'.";
		break;
	case PLUS_ASSIGNMENT_TYPE_ERROR:
		os << "Cannot += an expression of type '" << m_s2
				<< "' to a variable of type '" << m_s1 << "'.";
		break;
	case PREVIOUS_DECLARATION:
		os << "'" << m_s1 << "'"
				<< " has been already been declared in the current scope.";
		break;
	case UNDECLARED_VARIABLE:
		os << "Undeclared variable '" << m_s1 << "'";
		break;
	case VARIABLE_NOT_AN_ARRAY:
		os << "Variable '" << m_s1 << "' is not an array.";
		break;
	case NOT_A_COMPOUND_TYPE:
		os << "'" << m_s1 << "' does not reference a compound type.";
		break;
	case DIVIDE_BY_ZERO:
		os << "Arithmetic divide by zero.";
		break;
	case MOD_BY_ZERO:
		os << "Arithmetic mod by zero.";
		break;
	case MEMBER_DEFAULTS_MUST_BE_CONSTANT:
		os << "Default member values must be constant.";
		break;
	case UNDECLARED_TYPE:
		os << "Invalid type \"" << m_s1 << "\".";
		break;
	case PARTIALLY_DECLARED_TYPE:
		os << "Type \"" << m_s1 << "\" has not been fully defined.";
		break;
	case UNDECLARED_MEMBER:
		os << "Type \"" << m_s2 << "\" does not contain a member named \""
				<< m_s1 << "\".";
		break;
	case READONLY:
		os << "\"" << m_s1 << "\" is read-only.";
		break;
	case FUNCTION_RETURN_MISMATCH:
		os << "Return type does not match function return type.";
		break;
	case FUNCTION_PARAMETER_TYPE_MISMATCH_AMBIGUOUS:
		os << "Parameter type mismatch: can't unambiguous assign '" << m_s1
				<< "' to '" << m_s2 << "'";
		break;
	case FUNCTION_PARAMETER_TYPE_MISMATCH_INCOMPATIBLE:
		os << "Parameter type mismatch: can't assign '" << m_s1 << "' to '"
				<< m_s2 << "'";
		break;
	case FUNCTION_PARAMETER_DEFAULT_MUST_BE_CONSTANT:
		os << "Parameter default must be constant.";
		break;
	case TOO_MANY_ARGUMENTS:
		os << "Too many arguments for function of type '" << m_s1 << "'.";
		break;
	case NO_PARAMETER_DEFAULT:
		os << "No value specified for non-default parameter '" << m_s1 << "'.";
		break;
	case NOT_A_FUNCTION:
		os << "The given expression does not reference a valid function.";
		break;
	case INFERRED_DECLARATION_FAILED:
		os << "Inferred declaration failure.";
		break;
	case INCOMPLETE_MATCH:
		os << "Incomplete match. Unmatched variants: " << m_s1 << ".";
		break;
	case MATCH_FAILURE:
		os << "Match failure.";
		break;
	case MATCH_REQUIRES_SUM:
		os << "Match statement requires a sum type expression.";
		break;
	case DUPLICATE_MATCH_BLOCK:
		os << "Match block for variant '" << m_s1 << "' already exists.";
		break;
	case EXTRANEOUS_DEFAULT_MATCH:
		os << "Complete match; default match block well never execute.";
		break;
	case RAW_RECURSIVE_DECLARATION:
		os << "Specification of containing type requires Maybe type specifier.";
		break;
	case MAX_INVOCATION_DEPTH:
		os << "Max invocation depth " << m_s1 << " exceeded.";
		break;
	case INVALID_WITH_OPERAND:
		os << "Invalid source expression type '" << m_s1 << "' for 'with' operation: source must be of a record type.";
		break;
	default:
		os << "Unknown error passed to Error::error_core.";
		break;
	}

	return os.str();
}

ostream &operator<<(ostream &os, const Error &error) {
	os << error.ToString();
	return os;
}

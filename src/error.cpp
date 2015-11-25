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

int Error::m_num_errors = 0;

void Error::lex_error(int line_number, string s1) {
	cerr << "Syntax error on line " << line_number << " '" << s1 << "'"
			<< " is not a legal token." << endl;
}

void Error::parse_error(int line_number, int column_number, string s1) {
	cerr << "Parse error on line " << line_number << ", column "
			<< column_number << " reported by parser: " << s1 << "." << endl;
	m_num_errors++;
}

void Error::error_core(ostream &os, ErrorCode code, string s1, string s2,
		string s3) {
	switch (code) {
	case ARRAY_INDEX_MUST_BE_AN_INTEGER:
		// s2 is expected to be one of the following strings
		//     "A double expression"
		//     "A string expression"
		os << s2 << " is not a legal array index.  The array is '" << s1
				<< "'.";
		break;
	case ARRAY_INDEX_OUT_OF_BOUNDS:
		os << "Index value '" << s2 << "' is out of bounds for array '" << s1
				<< "'.";
		break;
	case ASSIGNMENT_TYPE_ERROR:
		os << "Cannot assign an expression of type '" << s2
				<< "' to a variable of type '" << s1 << "'.";
		break;
	case EXIT_STATUS_MUST_BE_AN_INTEGER:
		os << "Value passed to exit() must be an integer.  "
				<< "Value passed was of type '" << s1 << "'.";
		break;
	case ARRAY_SIZE_MISMATCH:
		os << "The size of array '" << s1
				<< "' must match the size of the right-hand side of the assignment.";
		break;
	case INVALID_LHS_OF_ASSIGNMENT:
		os
				<< "Left-hand side of assignment must be of type INT, DOUBLE, or STRING."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'.";
		break;
	case INVALID_LHS_OF_MINUS_ASSIGNMENT:
		os << "LHS of minus-assignment must be (INT || DOUBLE)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'.";
		break;
	case INVALID_LHS_OF_PLUS_ASSIGNMENT:
		os << "LHS of plus-assignment must be (INT || DOUBLE || STRING)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'.";
		break;
	case INVALID_LEFT_OPERAND_TYPE:
		os << "Invalid left operand for operator '" << s1 << "'.";
		break;
	case INVALID_RIGHT_OPERAND_TYPE:
		os << "Invalid right operand for operator '" << s1 << "'.";
		break;
	case INVALID_TYPE_FOR_INITIAL_VALUE:
		os << "Incorrect type for initial value of variable '" << s1 << "'.";
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
		os << "Invalid array type '" << s1 << "' for variable '" << s2 << "'.";
		break;
	case LHS_OF_PERIOD_MUST_BE_OBJECT:
		os << "Variable '" << s1 << "' is not an object."
				<< "  Only objects may be on the left of a period.";
		break;
	case MINUS_ASSIGNMENT_TYPE_ERROR:
		os << "Cannot -= an expression of type '" << s2
				<< "' from a variable of type '" << s1 << "'.";
		break;
	case PLUS_ASSIGNMENT_TYPE_ERROR:
		os << "Cannot += an expression of type '" << s2
				<< "' to a variable of type '" << s1 << "'.";
		break;
	case PREVIOUSLY_DECLARED_VARIABLE:
		os << "Variable '" << s1 << "'" << " previously declared.";
		break;
	case UNDECLARED_VARIABLE:
		os << "Undeclared variable '" << s1 << "'";
		break;
	case VARIABLE_NOT_AN_ARRAY:
		os << "Variable '" << s1 << "' is not an array.";
		break;
	case VARIABLE_NOT_A_COMPOUND_TYPE:
		os << "Variable '" << s1 << "' does not reference a compound type.";
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
		os << "Invalid type \"" << s1 << "\".";
		break;
	case UNDECLARED_MEMBER:
		os << "Type \"" << s2 << "\" does not contain a member named \"" << s1
				<< "\".";
		break;
	case READONLY:
		os << "\"" << s1 << "\" is read-only.";
		break;
	case FUNCTION_RETURN_MISMATCH:
		os << "Function does not return specified type.";
		break;
	case FUNCTION_PARAMETER_TYPE_MISMATCH:
		os << "Parameter type mismatch: can't assign '" << s1 << "' to '"  << s2 << "'";
		break;
	case TOO_MANY_ARGUMENTS:
		os << "Too many arguments for function of type '" << s1 << "'.";
		break;
	case NO_PARAMETER_DEFAULT:
		os << "No value specified for non-default parameter '" << s1 << "'.";
		break;
	case NOT_A_FUNCTION:
		os << "The given expression does not reference a valid function.";
		break;
	default:
		os << "Unknown error passed to Error::error_core.";
		break;
	}

	m_num_errors++;
}

Error::Error() :
		m_error_class(DEFAULT_ERROR_CLASS), m_code(DEFAULT_ERROR_CODE), m_line_number(
				-1), m_column_number(-1) {
}

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
	error_core(os, m_code, m_s1, m_s2, m_s3);
	return os.str();
}

ostream &operator<<(ostream &os, const Error &error) {
	os << error.ToString();
	return os;
}


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

int Error::m_num_errors = 0;

void Error::lex_error(int line_number, string s1) {
	cerr << "Syntax error on line " << line_number << " '" << s1 << "'"
			<< " is not a legal token." << endl;
}

void Error::parse_error(int line_number, string s1) {
	cerr << "Parse error on line " << line_number << " reported by parser: "
			<< s1 << "." << endl;
	m_num_errors++;
}

void Error::semantic_error(Error_type type, int line_number, int column_number,
		string s1, string s2, string s3) {
	cerr << "Semantic error on line " << line_number << ", column "
			<< column_number << ": ";
	error_core(type, s1, s2, s3);
}

void Error::error_core(Error_type type, string s1, string s2, string s3) {
	switch (type) {
	case ARRAY_INDEX_MUST_BE_AN_INTEGER:
		// s2 is expected to be one of the following strings
		//     "A double expression"
		//     "A string expression"
		//     "A animation_block expression"
		cerr << s2 << " is not a legal array index.  The array is '" << s1
				<< "'." << endl;
		break;
	case ARRAY_INDEX_OUT_OF_BOUNDS:
		cerr << "Index value '" << s2 << "' is out of bounds for array '" << s1
				<< "'." << endl;
		break;
	case ASSIGNMENT_TYPE_ERROR:
		cerr << "Cannot assign an expression of type '" << s2
				<< "' to a variable of type '" << s1 << "'." << endl;
		break;
	case EXIT_STATUS_MUST_BE_AN_INTEGER:
		cerr << "Value passed to exit() must be an integer.  "
				<< "Value passed was of type '" << s1 << "'." << endl;
		break;
	case INCORRECT_CONSTRUCTOR_PARAMETER_TYPE:
		cerr << "Incorrect type for parameter '" << s2 << "' of object " << s1
				<< "." << endl;
		break;
	case INVALID_ARRAY_SIZE:
		cerr << "The array '" << s1 << "' was declared with illegal size '"
				<< s2 << "'.  Arrays sizes must be integers of 1 or larger."
				<< endl;
		break;
		// everything but a game object is a legal LHS of assignment
	case INVALID_LHS_OF_ASSIGNMENT:
		cerr << "LHS of assignment must be "
				<< "(INT || DOUBLE || STRING || ANIMATION_BLOCK)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'."
				<< endl;
		break;
	case INVALID_LHS_OF_MINUS_ASSIGNMENT:
		cerr << "LHS of minus-assignment must be (INT || DOUBLE)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'."
				<< endl;
		break;
	case INVALID_LHS_OF_PLUS_ASSIGNMENT:
		cerr << "LHS of plus-assignment must be (INT || DOUBLE || STRING)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'."
				<< endl;
		break;
	case INVALID_LEFT_OPERAND_TYPE:
		cerr << "Invalid left operand for operator '" << s1 << "'." << endl;
		break;
	case INVALID_RIGHT_OPERAND_TYPE:
		cerr << "Invalid right operand for operator '" << s1 << "'." << endl;
		break;
	case INVALID_TYPE_FOR_INITIAL_VALUE:
		cerr << "Incorrect type for initial value of variable '" << s1 << "'."
				<< endl;
		break;
	case INVALID_TYPE_FOR_FOR_STMT_EXPRESSION:
		cerr << "Incorrect type for expression in for statement."
				<< "  Expressions in for statements must be of type INT."
				<< endl;
		break;
	case INVALID_TYPE_FOR_IF_STMT_EXPRESSION:
		cerr << "Incorrect type for expression in an if statement."
				<< "  Expressions in if statements must be of type INT."
				<< endl;
		break;
	case INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION:
		cerr << "Incorrect type for expression in a print statement."
				<< "  Expressions in print statements must be"
				<< " of type INT, DOUBLE, or STRING." << endl;
		break;
	case INVALID_TYPE_FOR_RESERVED_VARIABLE:
		cerr << "Incorrect type for reserved variable '" << s1
				<< "'  It was declared with type '" << s2
				<< "'.  It must be of type '" << s3 << "'." << endl;
		break;
	case LHS_OF_PERIOD_MUST_BE_OBJECT:
		cerr << "Variable '" << s1 << "' is not an object."
				<< "  Only objects may be on the left of a period." << endl;
		break;
	case MINUS_ASSIGNMENT_TYPE_ERROR:
		cerr << "Cannot -= an expression of type '" << s2
				<< "' from a variable of type '" << s1 << "'." << endl;
		break;
	case PLUS_ASSIGNMENT_TYPE_ERROR:
		cerr << "Cannot += an expression of type '" << s2
				<< "' to a variable of type '" << s1 << "'." << endl;
		break;
	case PREVIOUSLY_DECLARED_VARIABLE:
		cerr << "Variable '" << s1 << "'" << " previously declared." << endl;
		break;
	case UNDECLARED_VARIABLE:
		cerr << "Variable '" << s1 << "'"
				<< " was not declared before it was used." << endl;
		break;
	case UNKNOWN_CONSTRUCTOR_PARAMETER:
		cerr << "Class '" << s1 << "' does not have a parameter called '" << s2
				<< "'." << endl;
		break;
	case VARIABLE_NOT_AN_ARRAY:
		cerr << "Variable '" << s1 << "' is not an array." << endl;
		break;
	case DIVIDE_BY_ZERO_AT_PARSE_TIME:
		cerr << "Arithmetic divide by zero at parse time.  "
				<< "Using zero as the result so parse can continue." << endl;
		break;
	case MOD_BY_ZERO_AT_PARSE_TIME:
		cerr << "Arithmetic mod by zero at parse time.  "
				<< "Using zero as the result so parse can continue." << endl;
		break;
	default:
		cerr << "Unknown error passed to Error::error_core." << endl;
		break;
	}

	m_num_errors++;
}

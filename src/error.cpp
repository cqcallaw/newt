#include "error.h"

#include <iostream>
using namespace std;

// The following is a global variable defined by the scanner (the .l file)
extern int line_count; // the line number of current token

/* static */bool Error::m_runtime = false;
/* static */int Error::m_num_errors = 0;

/* static */void Error::error_header() {
	if (m_runtime)
		cerr << "Runtime error: ";
	else
		cerr << "Semantic error on line " << line_count << ": ";
}

/* static */void Error::error(Error_type type, string s1 /* = "" */,
		string s2 /* = "" */, string s3 /* = "" */
		) {
	switch (type) {
	case ARRAY_INDEX_MUST_BE_AN_INTEGER:
		error_header();
		// s2 is expected to be one of the following strings
		//     "A double expression"
		//     "A string expression"
		//     "A animation_block expression"
		cerr << s2 << " is not a legal array index.  The array is '" << s1
				<< "'.";
		if (m_runtime)
			cerr << "  Element '" << s1 << "[0]' will be used instead.";
		cerr << endl;
		break;
	case ARRAY_INDEX_OUT_OF_BOUNDS:
		error_header();
		cerr << "Index value '" << s2 << "' is out of bounds for array '" << s1
				<< "'.";
		if (m_runtime)
			cerr << "  Element '" << s1 << "[0]' will be used instead.";
		cerr << endl;
		break;
	case ASSIGNMENT_TYPE_ERROR:
		error_header();
		cerr << "Cannot assign an expression of type '" << s2
				<< "' to a variable of type '" << s1 << "'." << endl;
		break;

		// some attributes (such as h & w in a circle) cannot be changed
	case CANNOT_CHANGE_DERIVED_ATTRIBUTE:
		error_header();
		cerr << "Cannot changed derived field '" << s1 << "' of a '" << s2
				<< "' object.  This field is derived from other fields.  ";
		if (m_runtime)
			cerr << "The change will be ignored.";
		cerr << endl;
		break;
	case EXIT_STATUS_MUST_BE_AN_INTEGER:
		error_header();
		cerr << "Value passed to exit() must be an integer.  "
				<< "Value passed was of type '" << s1 << "'." << endl;
		break;

		// this error originates from the Bison file when it finds an illegal token
	case ILLEGAL_TOKEN:
		cerr << "Syntax error on line " << line_count << " '" << s1 << "'"
				<< " is not a legal token." << endl;
		break;
	case INCORRECT_CONSTRUCTOR_PARAMETER_TYPE:
		error_header();
		cerr << "Incorrect type for parameter '" << s2 << "' of object " << s1
				<< "." << endl;
		break;
	case INVALID_ARRAY_SIZE:
		error_header();
		cerr << "The array '" << s1 << "' was declared with illegal size '"
				<< s2 << "'.  Arrays sizes must be integers of 1 or larger."
				<< endl;
		break;
		// everything but a game object is a legal LHS of assignment
	case INVALID_LHS_OF_ASSIGNMENT:
		error_header();
		cerr << "LHS of assignment must be "
				<< "(INT || DOUBLE || STRING || ANIMATION_BLOCK)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'."
				<< endl;
		break;
	case INVALID_LHS_OF_MINUS_ASSIGNMENT:
		error_header();
		cerr << "LHS of minus-assignment must be (INT || DOUBLE)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'."
				<< endl;
		break;
	case INVALID_LHS_OF_PLUS_ASSIGNMENT:
		error_header();
		cerr << "LHS of plus-assignment must be (INT || DOUBLE || STRING)."
				<< "  Variable '" << s1 << "' is of type '" << s2 << "'."
				<< endl;
		break;
	case INVALID_LEFT_OPERAND_TYPE:
		error_header();
		cerr << "Invalid left operand for operator '" << s1 << "'." << endl;
		break;
	case INVALID_RIGHT_OPERAND_TYPE:
		error_header();
		cerr << "Invalid right operand for operator '" << s1 << "'." << endl;
		break;
	case INVALID_TYPE_FOR_INITIAL_VALUE:
		error_header();
		cerr << "Incorrect type for initial value of variable '" << s1 << "'."
				<< endl;
		break;
	case INVALID_TYPE_FOR_FOR_STMT_EXPRESSION:
		error_header();
		cerr << "Incorrect type for expression in for statement."
				<< "  Expressions in for statements must be of type INT."
				<< endl;
		break;
	case INVALID_TYPE_FOR_IF_STMT_EXPRESSION:
		error_header();
		cerr << "Incorrect type for expression in an if statement."
				<< "  Expressions in if statements must be of type INT."
				<< endl;
		break;
	case INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION:
		error_header();
		cerr << "Incorrect type for expression in a print statement."
				<< "  Expressions in print statements must be"
				<< " of type INT, DOUBLE, or STRING." << endl;
		break;
	case INVALID_TYPE_FOR_RESERVED_VARIABLE:
		error_header();
		cerr << "Incorrect type for reserved variable '" << s1
				<< "'  It was declared with type '" << s2
				<< "'.  It must be of type '" << s3 << "'." << endl;
		break;
	case LHS_OF_PERIOD_MUST_BE_OBJECT:
		error_header();
		cerr << "Variable '" << s1 << "' is not an object."
				<< "  Only objects may be on the left of a period." << endl;
		break;
	case MINUS_ASSIGNMENT_TYPE_ERROR:
		error_header();
		cerr << "Cannot -= an expression of type '" << s2
				<< "' from a variable of type '" << s1 << "'." << endl;
		break;
		// only called in when the parser finds an error
		// called from yyerror()
	case PARSE_ERROR:
		cerr << "Parse error on line " << line_count << " reported by parser: "
				<< s1 << "." << endl;
		break;
	case PLUS_ASSIGNMENT_TYPE_ERROR:
		error_header();
		cerr << "Cannot += an expression of type '" << s2
				<< "' to a variable of type '" << s1 << "'." << endl;
		break;
	case PREVIOUSLY_DECLARED_VARIABLE:
		error_header();
		cerr << "Variable '" << s1 << "'" << " previously declared." << endl;
		break;
	case UNDECLARED_VARIABLE:
		error_header();
		cerr << "Variable '" << s1 << "'"
				<< " was not declared before it was used." << endl;
		break;
	case UNKNOWN_CONSTRUCTOR_PARAMETER:
		error_header();
		cerr << "Class '" << s1 << "' does not have a parameter called '" << s2
				<< "'." << endl;
		break;
	case VARIABLE_NOT_AN_ARRAY:
		error_header();
		cerr << "Variable '" << s1 << "' is not an array." << endl;
		break;
	case DIVIDE_BY_ZERO_AT_PARSE_TIME:
		error_header();
		cerr << "Arithmetic divide by zero at parse time.  "
				<< "Using zero as the result so parse can continue." << endl;
		break;
	case MOD_BY_ZERO_AT_PARSE_TIME:
		error_header();
		cerr << "Arithmetic mod by zero at parse time.  "
				<< "Using zero as the result so parse can continue." << endl;
		break;
	case UNDEFINED_ERROR:
		error_header();
		cerr << "Undefined error passed to Error::error().  "
				<< "This is probably because error.cpp was not updated "
				<< "when a new error was added to error.h." << endl;
		break;
	default:
		cerr << "Unknown error sent to class Error::error()." << endl;
		break;
	}
	m_num_errors++;
}

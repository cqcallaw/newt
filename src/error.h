/*
 class Error provides a means to unify error messages

 To issue an error message, call the following static function

 static void error(Error_type type,
 string s1 = "",
 string s2 = "",
 string s3 = "");

 Static functions can be called without instantiating an Error object:

 Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, name);

 */

#ifndef ERROR_H
#define ERROR_H

#include <string>

class Error {
public:
	enum Error_type {
		ARRAY_INDEX_MUST_BE_AN_INTEGER,
		ARRAY_INDEX_OUT_OF_BOUNDS,
		ASSIGNMENT_TYPE_ERROR,
		CANNOT_CHANGE_DERIVED_ATTRIBUTE,
		EXIT_STATUS_MUST_BE_AN_INTEGER,
		ILLEGAL_TOKEN,
		INCORRECT_CONSTRUCTOR_PARAMETER_TYPE,
		INVALID_ARRAY_SIZE,
		INVALID_LHS_OF_ASSIGNMENT,
		INVALID_LHS_OF_MINUS_ASSIGNMENT,
		INVALID_LHS_OF_PLUS_ASSIGNMENT,
		INVALID_RIGHT_OPERAND_TYPE,
		INVALID_LEFT_OPERAND_TYPE,
		INVALID_TYPE_FOR_INITIAL_VALUE,
		INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
		INVALID_TYPE_FOR_IF_STMT_EXPRESSION,
		INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION,
		INVALID_TYPE_FOR_RESERVED_VARIABLE,
		LHS_OF_PERIOD_MUST_BE_OBJECT,
		MINUS_ASSIGNMENT_TYPE_ERROR,
		NO_BODY_PROVIDED_FOR_FORWARD,
		PARSE_ERROR,
		PLUS_ASSIGNMENT_TYPE_ERROR,
		PREVIOUSLY_DECLARED_VARIABLE,
		UNDECLARED_VARIABLE,
		UNKNOWN_CONSTRUCTOR_PARAMETER,
		VARIABLE_NOT_AN_ARRAY,
		DIVIDE_BY_ZERO_AT_PARSE_TIME,
		MOD_BY_ZERO_AT_PARSE_TIME,
		UNDEFINED_ERROR
	};

	static void error(Error_type type, std::string s1 = "", std::string s2 = "",
			std::string s3 = "");

	static void starting_execution() {
		m_runtime = true;
	}

	static int num_errors() {
		return m_num_errors;
	}
	static bool runtime() {
		return m_runtime;
	}

protected:
	static bool m_runtime;
	static int m_num_errors;
	static void error_header();
};

#endif // #ifndef ERROR_H

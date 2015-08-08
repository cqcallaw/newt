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

using namespace std;

class Error {
public:
	enum ErrorClass {
		DEFAULT_ERROR_CLASS, PARSE, SEMANTIC, RUNTIME
	};
	enum ErrorCode {
		DEFAULT_ERROR_CODE,
		ARRAY_INDEX_MUST_BE_AN_INTEGER,
		ARRAY_INDEX_OUT_OF_BOUNDS,
		ASSIGNMENT_TYPE_ERROR,
		EXIT_STATUS_MUST_BE_AN_INTEGER,
		ILLEGAL_TOKEN,
		INCORRECT_CONSTRUCTOR_PARAMETER_TYPE,
		INVALID_ARRAY_TYPE,
		INVALID_ARRAY_SIZE,
		INVALID_ARRAY_SIZE_TYPE,
		INVALID_LHS_OF_ASSIGNMENT,
		INVALID_LHS_OF_MINUS_ASSIGNMENT,
		INVALID_LHS_OF_PLUS_ASSIGNMENT,
		INVALID_RIGHT_OPERAND_TYPE,
		INVALID_LEFT_OPERAND_TYPE,
		INVALID_TYPE_FOR_INITIAL_VALUE,
		INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
		INVALID_TYPE_FOR_IF_STMT_EXPRESSION,
		INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION,
		LHS_OF_PERIOD_MUST_BE_OBJECT,
		MINUS_ASSIGNMENT_TYPE_ERROR,
		NO_BODY_PROVIDED_FOR_FORWARD,
		PLUS_ASSIGNMENT_TYPE_ERROR,
		PREVIOUSLY_DECLARED_VARIABLE,
		UNDECLARED_VARIABLE,
		UNKNOWN_CONSTRUCTOR_PARAMETER,
		VARIABLE_NOT_AN_ARRAY,
		DIVIDE_BY_ZERO,
		MOD_BY_ZERO,
		PARSE_ERROR
	};

	Error();
	Error(ErrorClass error_class, ErrorCode code, int line_number,
			int column_number, string s1 = "", string s2 = "", string s3 = "");

	static void lex_error(int line_number, string s1);

	static void parse_error(int line_number, string s1);

	static void semantic_error(ErrorCode code, int line_number,
			int column_number, string s1 = "", string s2 = "", string s3 = "");

	static void runtime_error(ErrorCode code, string s1 = "", string s2 = "",
			string s3 = "");

	static int num_errors() {
		return m_num_errors;
	}

	const string ToString() const;

protected:
	static void error_core(ostream &os, ErrorCode code, string s1 = "",
			string s2 = "", string s3 = "");

	static int m_num_errors;

	const string get_error_message() const;

private:
	const ErrorClass m_error_class;
	const ErrorCode m_code;
	const int m_line_number;
	const int m_column_number;
	const string m_s1;
	const string m_s2;
	const string m_s3;
};

std::ostream &operator<<(std::ostream &os, const Error &error);

#endif // #ifndef ERROR_H

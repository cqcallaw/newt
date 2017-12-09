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
#include <linked_list.h>

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
		AMBIGUOUS_WIDENING_CONVERSION,
		ARRAY_SIZE_MISMATCH,
		EXIT_STATUS_MUST_BE_AN_INTEGER,
		ILLEGAL_TOKEN,
		INVALID_TYPE_COMBINATION,
		INVALID_ARRAY_TYPE,
		INVALID_LHS_OF_ASSIGNMENT,
		INVALID_LHS_OF_MINUS_ASSIGNMENT,
		INVALID_LHS_OF_PLUS_ASSIGNMENT,
		INVALID_RIGHT_OPERAND_TYPE,
		INVALID_LEFT_OPERAND_TYPE,
		INVALID_INITIALIZER_TYPE,
		INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
		INVALID_TYPE_FOR_IF_STMT_EXPRESSION,
		INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION,
		MINUS_ASSIGNMENT_TYPE_ERROR,
		PLUS_ASSIGNMENT_TYPE_ERROR,
		PREVIOUS_DECLARATION,
		BUILTIN_REDECLARATION,
		UNDECLARED_VARIABLE,
		VARIABLE_NOT_AN_ARRAY,
		NOT_A_COMPOUND_TYPE,
		DIVIDE_BY_ZERO,
		MOD_BY_ZERO,
		PARSE_ERROR,
		MEMBER_DEFAULTS_MUST_BE_CONSTANT,
		READONLY,
		UNDECLARED_TYPE,
		PARTIALLY_DECLARED_TYPE,
		UNDECLARED_MEMBER,
		FUNCTION_RETURN_MISMATCH,
		FUNCTION_PARAMETER_TYPE_MISMATCH_AMBIGUOUS,
		FUNCTION_PARAMETER_TYPE_MISMATCH_INCOMPATIBLE,
		FUNCTION_PARAMETER_DEFAULT_MUST_BE_CONSTANT,
		INFERRED_DECLARATION_FAILED,
		TOO_MANY_ARGUMENTS,
		NO_PARAMETER_DEFAULT,
		NOT_A_FUNCTION,
		EXPRESSION_IS_NOT_A_FUNCTION,
		MATCH_REQUIRES_SUM,
		INCOMPLETE_MATCH,
		MATCH_FAILURE,
		DUPLICATE_MATCH_BLOCK,
		EXTRANEOUS_DEFAULT_MATCH,
		RAW_RECURSIVE_DECLARATION,
		MAX_INVOCATION_DEPTH,
		INVALID_WITH_OPERAND,
		FUNCTION_VARIANT_WITH_DUPLICATE_SIGNATURE,
		NO_FUNCTION_VARIANT_MATCH,
		MULTIPLE_FUNCTION_VARIANT_MATCHES,
		CANNOT_REFERENCE_SUM_VARIANT_AS_DATA,
		MISSING_RETURN_COVERAGE,
		STMT_SOURCE_MUST_BE_RECORD,
		FOREACH_STMT_REQUIRES_NEXT,
		RETURN_STMT_MUST_BE_MAYBE,
		FOREACH_NEXT_MUST_EVALUATE_TO_BASE_TYPE,
		USING_ASSIGNMENT_TYPE_ERROR,
		USING_AMBIGUOUS_WIDENING_CONVERSION
	};

	Error(ErrorClass error_class, ErrorCode code, int line_number,
			int column_number, string s1 = "", string s2 = "", string s3 = "");

	const string ToString() const;

	const ErrorCode GetCode() const {
		return m_code;
	}

	const ErrorClass GetErrorClass() const {
		return m_error_class;
	}

protected:
	static void error_core(ostream &os, ErrorCode code, string s1 = "",
			string s2 = "", string s3 = "");

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

typedef const LinkedList<Error, NO_DUPLICATES> ErrorList;
typedef shared_ptr<ErrorList> ErrorListRef;

#endif // #ifndef ERROR_H

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

#ifndef BUILTINS_H_
#define BUILTINS_H_

#include <mutex>

#include "error.h"
#include "symbol_table.h"
#include "type_table.h"

class UnitType;
class SumType;
class MaybeTypeSpecifier;

typedef map<const int, volatile_shared_ptr<fstream>> file_handle_map;

class Builtins {
public:
	/**
	 * Get a context pre-loaded with built-ins, such as errors
	 */
	static const shared_ptr<ExecutionContext> GetBuiltinContext();

	static const_shared_ptr<std::string> ERROR_TYPE_NAME;
	static const_shared_ptr<std::string> ERROR_CODE_NAME;
	static const_shared_ptr<std::string> ERROR_MESSAGE_NAME;
	static const_shared_ptr<ComplexTypeSpecifier> get_error_type_specifier();
	static const_shared_ptr<MaybeTypeSpecifier> get_error_maybe_type_specifier();

	static const_shared_ptr<std::string> ERROR_LIST_TYPE_NAME;
	static const_shared_ptr<std::string> ERROR_LIST_DATA_NAME;
	static const_shared_ptr<std::string> ERROR_LIST_NEXT_NAME;
	static const_shared_ptr<ComplexTypeSpecifier> get_error_list_type_specifier();
	static const_shared_ptr<MaybeTypeSpecifier> get_error_list_maybe_type_specifier();

	static const_shared_ptr<std::string> STREAM_MODE_TYPE_NAME;
	static const_shared_ptr<std::string> STREAM_MODE_IN_NAME;
	static const_shared_ptr<std::string> STREAM_MODE_OUT_NAME;
	static const_shared_ptr<std::string> STREAM_MODE_BINARY_NAME;
	static const_shared_ptr<std::string> STREAM_MODE_ATE_NAME;
	static const_shared_ptr<std::string> STREAM_MODE_APP_NAME;
	static const_shared_ptr<std::string> STREAM_MODE_TRUNC_NAME;
	static const_shared_ptr<ComplexTypeSpecifier> get_stream_mode_type_specifier();

	static const_shared_ptr<std::string> BYTE_RESULT_TYPE_NAME;
	static const_shared_ptr<std::string> BYTE_RESULT_DATA_NAME;
	static const_shared_ptr<std::string> BYTE_RESULT_ERRORS_NAME;
	static const_shared_ptr<ComplexTypeSpecifier> get_byte_result_type_specifier();

	static const_shared_ptr<std::string> INT_RESULT_TYPE_NAME;
	static const_shared_ptr<std::string> INT_RESULT_DATA_NAME;
	static const_shared_ptr<std::string> INT_RESULT_ERRORS_NAME;
	static const_shared_ptr<ComplexTypeSpecifier> get_int_result_type_specifier();

	static const_shared_ptr<std::string> BYTE_READ_RESULT_TYPE_NAME;
	static const_shared_ptr<std::string> BYTE_READ_RESULT_DATA_NAME;
	static const_shared_ptr<std::string> BYTE_READ_RESULT_EOF_NAME;
	static const_shared_ptr<std::string> BYTE_READ_RESULT_ERRORS_NAME;
	static const_shared_ptr<ComplexTypeSpecifier> get_byte_read_result_type_specifier();
	static const_shared_ptr<ComplexTypeSpecifier> get_byte_read_result_eof_type_specifier();

	static volatile_shared_ptr<file_handle_map> get_file_handle_map();
	static volatile_shared_ptr<mutex> get_file_handle_map_mutex();
};

#endif /* BUILTINS_H_ */

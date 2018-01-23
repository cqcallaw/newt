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

#include <builtins.h>
#include <record_type.h>
#include <maybe_type.h>
#include <unit_type.h>
#include <type_alias_declaration_statement.h>
#include <maybe_type_specifier.h>

const_shared_ptr<std::string> Builtins::ERROR_TYPE_NAME = make_shared<
		std::string>("error");

const_shared_ptr<std::string> Builtins::ERROR_CODE_NAME = make_shared<
		std::string>("id");

const_shared_ptr<std::string> Builtins::ERROR_MESSAGE_NAME = make_shared<
		std::string>("message");

const_shared_ptr<ComplexTypeSpecifier> Builtins::get_error_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::ERROR_TYPE_NAME,
			TypeSpecifierList::GetTerminator());
	return instance;
}

const_shared_ptr<MaybeTypeSpecifier> Builtins::get_error_maybe_type_specifier() {
	static const_shared_ptr<MaybeTypeSpecifier> instance = make_shared<
			MaybeTypeSpecifier>(get_error_type_specifier(),
			GetDefaultLocation());
	return instance;
}
const_shared_ptr<std::string> Builtins::ERROR_LIST_TYPE_NAME = make_shared<
		std::string>("error_list");

const_shared_ptr<std::string> Builtins::ERROR_LIST_DATA_NAME = make_shared<
		std::string>("data");

const_shared_ptr<std::string> Builtins::ERROR_LIST_NEXT_NAME = make_shared<
		std::string>("next");

const_shared_ptr<ComplexTypeSpecifier> Builtins::get_error_list_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::ERROR_LIST_TYPE_NAME,
			TypeSpecifierList::GetTerminator());
	return instance;
}

const_shared_ptr<MaybeTypeSpecifier> Builtins::get_error_list_maybe_type_specifier() {
	static const_shared_ptr<MaybeTypeSpecifier> instance = make_shared<
			MaybeTypeSpecifier>(get_error_list_type_specifier(),
			GetDefaultLocation());
	return instance;
}

const_shared_ptr<std::string> Builtins::STREAM_MODE_TYPE_NAME = make_shared<
		std::string>("stream_mode");
const_shared_ptr<std::string> Builtins::STREAM_MODE_IN_NAME = make_shared<
		std::string>("read");
const_shared_ptr<std::string> Builtins::STREAM_MODE_OUT_NAME = make_shared<
		std::string>("write");
const_shared_ptr<std::string> Builtins::STREAM_MODE_BINARY_NAME = make_shared<
		std::string>("binary");
const_shared_ptr<std::string> Builtins::STREAM_MODE_ATE_NAME = make_shared<
		std::string>("ate");
const_shared_ptr<std::string> Builtins::STREAM_MODE_APP_NAME = make_shared<
		std::string>("app");
const_shared_ptr<std::string> Builtins::STREAM_MODE_TRUNC_NAME = make_shared<
		std::string>("trunc");
const_shared_ptr<ComplexTypeSpecifier> Builtins::get_stream_mode_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::STREAM_MODE_TYPE_NAME,
			TypeSpecifierList::GetTerminator());
	return instance;
}

const_shared_ptr<std::string> Builtins::BYTE_RESULT_TYPE_NAME = make_shared<
		std::string>("byte_result");

const_shared_ptr<std::string> Builtins::BYTE_RESULT_DATA_NAME = make_shared<
		std::string>("data");

const_shared_ptr<std::string> Builtins::BYTE_RESULT_ERRORS_NAME = make_shared<
		std::string>("errors");
const_shared_ptr<ComplexTypeSpecifier> Builtins::get_byte_result_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::BYTE_RESULT_TYPE_NAME,
			TypeSpecifierList::GetTerminator());
	return instance;
}

const_shared_ptr<std::string> Builtins::INT_RESULT_TYPE_NAME = make_shared<
		std::string>("int_result");
const_shared_ptr<std::string> Builtins::INT_RESULT_DATA_NAME = make_shared<
		std::string>("data");
const_shared_ptr<std::string> Builtins::INT_RESULT_ERRORS_NAME = make_shared<
		std::string>("errors");
const_shared_ptr<ComplexTypeSpecifier> Builtins::get_int_result_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::INT_RESULT_TYPE_NAME,
			TypeSpecifierList::GetTerminator());
	return instance;
}

const_shared_ptr<std::string> Builtins::BYTE_READ_RESULT_TYPE_NAME =
		make_shared<std::string>("byte_read_result");
const_shared_ptr<std::string> Builtins::BYTE_READ_RESULT_DATA_NAME =
		make_shared<std::string>("data");
const_shared_ptr<std::string> Builtins::BYTE_READ_RESULT_EOF_NAME = make_shared<
		std::string>("eof");
const_shared_ptr<std::string> Builtins::BYTE_READ_RESULT_ERRORS_NAME =
		make_shared<std::string>("errors");
const_shared_ptr<ComplexTypeSpecifier> Builtins::get_byte_read_result_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::BYTE_READ_RESULT_TYPE_NAME,
			TypeSpecifierList::GetTerminator());
	return instance;
}
const_shared_ptr<ComplexTypeSpecifier> Builtins::get_byte_read_result_eof_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::BYTE_READ_RESULT_EOF_NAME,
			TypeSpecifierList::GetTerminator(),
			get_byte_read_result_type_specifier(),
			NamespaceQualifierList::GetTerminator());
	return instance;
}

volatile_shared_ptr<file_handle_map> Builtins::get_file_handle_map() {
	static volatile_shared_ptr<file_handle_map> instance = make_shared<
			file_handle_map>();
	return instance;
}

const_shared_ptr<string> Builtins::get_builtin_definition() {
	ostringstream ss;
	ss << *Builtins::LANGUAGE_VERSION_NAME << ":string = \""
			<< *LANGUAGE_VERSION_STRING << "\"" << endl;
	ss << *Builtins::ERROR_TYPE_NAME << " { " << *Builtins::ERROR_CODE_NAME
			<< ":int, " << *Builtins::ERROR_MESSAGE_NAME << ":string }" << endl;
	ss << *Builtins::ERROR_LIST_TYPE_NAME << " { "
			<< *Builtins::ERROR_LIST_NEXT_NAME << ":"
			<< *Builtins::ERROR_LIST_TYPE_NAME << "?, "
			<< *Builtins::ERROR_LIST_DATA_NAME << ":"
			<< *Builtins::ERROR_TYPE_NAME << " }" << endl;
	ss << *Builtins::STREAM_MODE_TYPE_NAME << " { "
			<< *Builtins::STREAM_MODE_IN_NAME << ":bool, "
			<< *Builtins::STREAM_MODE_OUT_NAME << ":bool, "
			<< *Builtins::STREAM_MODE_BINARY_NAME << ":bool, "
			<< *Builtins::STREAM_MODE_ATE_NAME << ":bool, "
			<< *Builtins::STREAM_MODE_APP_NAME << ":bool, "
			<< *Builtins::STREAM_MODE_TRUNC_NAME << ":bool }" << endl;
	ss << *Builtins::BYTE_RESULT_TYPE_NAME << " { "
			<< *Builtins::BYTE_RESULT_DATA_NAME << ":byte | "
			<< *Builtins::BYTE_RESULT_ERRORS_NAME << ":"
			<< *Builtins::ERROR_LIST_TYPE_NAME << " }" << endl;
	ss << *Builtins::BYTE_READ_RESULT_TYPE_NAME << "{ "
			<< *Builtins::BYTE_READ_RESULT_EOF_NAME << " | "
			<< *Builtins::BYTE_READ_RESULT_DATA_NAME << ":byte |"
			<< *Builtins::BYTE_READ_RESULT_ERRORS_NAME << ":"
			<< *Builtins::ERROR_LIST_TYPE_NAME << " }" << endl;
	ss << *Builtins::INT_RESULT_TYPE_NAME << " { "
			<< *Builtins::INT_RESULT_DATA_NAME << ":int | "
			<< *Builtins::INT_RESULT_ERRORS_NAME << ":"
			<< *Builtins::ERROR_LIST_TYPE_NAME << " }" << endl;
	ss << *Builtins::PATH_SEPARATOR_NAME << ":string = \""
			<< get_path_separator() << "\"" << endl;

	auto result = make_shared<string>(ss.str());
	return result;
}

char Builtins::get_path_separator() {
	// ref: https://stackoverflow.com/a/12971535/577298
	// ref: https://stackoverflow.com/a/32439392/577298
	static const char path_separator =
#if defined _WIN32 || defined __CYGWIN__
			'\\';
#else
			'/';
#endif
	return path_separator;
}

const_shared_ptr<std::string> Builtins::PATH_SEPARATOR_NAME = make_shared<
		std::string>("path_separator");

const_shared_ptr<std::string> Builtins::LANGUAGE_VERSION_NAME = make_shared<
		std::string>("language_version");

#ifndef LANGUAGE_VERSION
#define LANGUAGE_VERSION none
#endif
const_shared_ptr<std::string> Builtins::LANGUAGE_VERSION_STRING = make_shared<
		std::string>(STRINGIZE(LANGUAGE_VERSION));

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

const shared_ptr<ExecutionContext> Builtins::GetBuiltinContext() {
	auto type_table = make_shared<TypeTable>();
	auto builtin_symbols = make_shared<SymbolContext>(Modifier::NONE);
	auto result = make_shared<ExecutionContext>(builtin_symbols, type_table,
			LifeTime::PERSISTENT);

	// error type
	auto error_code_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetInt(), DIRECT);
	auto error_message_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetString(), DIRECT);
	auto error_type_mapping = make_shared<type_map>(type_map { {
			*Builtins::ERROR_CODE_NAME, error_code_alias }, {
			*Builtins::ERROR_MESSAGE_NAME, error_message_alias } });
	auto error_type_table = make_shared<const TypeTable>(error_type_mapping,
			type_table);
	auto error_maybe =
			MaybeType::Build(type_table, get_error_type_specifier())->GetData<
					MaybeType>();
	auto error_type = make_shared<RecordType>(error_type_table, Modifier::NONE,
			error_maybe);
	type_table->AddType(*Builtins::ERROR_TYPE_NAME, error_type);

	// error list type
	auto error_alias = make_shared<AliasDefinition>(type_table,
			get_error_type_specifier(), DIRECT);
	auto error_list_maybe = MaybeType::Build(type_table,
			get_error_list_type_specifier())->GetData<MaybeType>();
	auto error_list_type_mapping = make_shared<type_map>(type_map { {
			*Builtins::ERROR_LIST_NEXT_NAME, error_list_maybe }, {
			*Builtins::ERROR_LIST_DATA_NAME, error_alias } });
	auto error_list_type_table = make_shared<const TypeTable>(
			error_list_type_mapping, type_table);
	auto error_list_type = make_shared<RecordType>(error_list_type_table,
			Modifier::NONE, error_list_maybe);
	type_table->AddType(*Builtins::ERROR_LIST_TYPE_NAME, error_list_type);

	// stream mode type
	auto in_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetBoolean(), DIRECT);
	auto out_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetBoolean(), DIRECT);
	auto binary_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetBoolean(), DIRECT);
	auto ate_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetBoolean(), DIRECT);
	auto app_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetBoolean(), DIRECT);
	auto trunc_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetBoolean(), DIRECT);
	auto stream_mode_maybe = MaybeType::Build(type_table,
			get_stream_mode_type_specifier())->GetData<MaybeType>();
	auto stream_mode_type_mapping = make_shared<type_map>(type_map { {
			*Builtins::STREAM_MODE_IN_NAME, in_alias }, {
			*Builtins::STREAM_MODE_OUT_NAME, out_alias }, {
			*Builtins::STREAM_MODE_BINARY_NAME, binary_alias }, {
			*Builtins::STREAM_MODE_ATE_NAME, ate_alias }, {
			*Builtins::STREAM_MODE_APP_NAME, app_alias }, {
			*Builtins::STREAM_MODE_TRUNC_NAME, trunc_alias } });
	auto stream_mode_type_table = make_shared<const TypeTable>(
			stream_mode_type_mapping, type_table);
	auto stream_mode_type = make_shared<RecordType>(stream_mode_type_table,
			Modifier::NONE, stream_mode_maybe);
	type_table->AddType(*Builtins::STREAM_MODE_TYPE_NAME, stream_mode_type);

	// byte result type
	auto byte_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetByte(), DIRECT);
	auto byte_error_list_alias = make_shared<AliasDefinition>(type_table,
			get_error_list_type_specifier(), DIRECT);
	auto byte_result_type_mapping = make_shared<type_map>(type_map { {
			*Builtins::BYTE_RESULT_DATA_NAME, byte_alias }, {
			*Builtins::BYTE_RESULT_ERRORS_NAME, byte_error_list_alias } });
	auto byte_result_type_table = make_shared<const TypeTable>(
			byte_result_type_mapping, type_table);
	auto byte_result_maybe = MaybeType::Build(type_table,
			get_byte_result_type_specifier())->GetData<MaybeType>();
	auto byte_type_constructor_result =
			TypeAliasDeclarationStatement::GetTypeConstructor(
					GetDefaultLocation(), Builtins::BYTE_RESULT_DATA_NAME,
					GetDefaultLocation(), GetDefaultLocation(),
					PrimitiveTypeSpecifier::GetInt(),
					get_byte_result_type_specifier(), byte_alias, result,
					result);
	assert(ErrorList::IsTerminator(byte_type_constructor_result.GetErrors()));
	auto byte_result_constructor_symbols = make_shared<SymbolTable>();
	byte_result_constructor_symbols->InsertSymbol(
			*Builtins::ERROR_LIST_NEXT_NAME,
			byte_type_constructor_result.GetData());
	auto byte_result_type = make_shared<SumType>(byte_result_type_table,
			Builtins::BYTE_RESULT_DATA_NAME, byte_result_constructor_symbols,
			byte_result_maybe);
	type_table->AddType(*Builtins::BYTE_RESULT_TYPE_NAME, byte_result_type);

	// int result type
	auto int_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetInt(), DIRECT);
	auto int_error_list_alias = make_shared<AliasDefinition>(type_table,
			get_error_list_type_specifier(), DIRECT);
	auto int_result_type_mapping = make_shared<type_map>(type_map { {
			*Builtins::INT_RESULT_DATA_NAME, int_alias }, {
			*Builtins::INT_RESULT_ERRORS_NAME, int_error_list_alias } });
	auto int_result_type_table = make_shared<const TypeTable>(
			int_result_type_mapping, type_table);
	auto int_result_maybe = MaybeType::Build(type_table,
			get_int_result_type_specifier())->GetData<MaybeType>();
	auto int_type_constructor_result =
			TypeAliasDeclarationStatement::GetTypeConstructor(
					GetDefaultLocation(), Builtins::INT_RESULT_DATA_NAME,
					GetDefaultLocation(), GetDefaultLocation(),
					PrimitiveTypeSpecifier::GetInt(),
					get_int_result_type_specifier(), int_alias, result, result);
	assert(ErrorList::IsTerminator(int_type_constructor_result.GetErrors()));
	auto int_result_constructor_symbols = make_shared<SymbolTable>();
	int_result_constructor_symbols->InsertSymbol(
			*Builtins::INT_RESULT_DATA_NAME,
			int_type_constructor_result.GetData());
	auto int_result_type = make_shared<SumType>(int_result_type_table,
			Builtins::INT_RESULT_DATA_NAME, int_result_constructor_symbols,
			int_result_maybe);
	type_table->AddType(*Builtins::INT_RESULT_TYPE_NAME, int_result_type);

	// byte read result type
	auto byte_read_alias = make_shared<AliasDefinition>(type_table,
			PrimitiveTypeSpecifier::GetByte(), DIRECT);
	auto byte_read_error_list_alias = make_shared<AliasDefinition>(type_table,
			get_error_list_type_specifier(), DIRECT);
	auto byte_read_eof_maybe = MaybeType::Build(type_table,
			get_byte_read_result_eof_type_specifier())->GetData<MaybeType>();
	auto byte_read_eof_type = make_shared<UnitType>(byte_read_eof_maybe);
	auto byte_read_result_type_mapping = make_shared<type_map>(type_map { {
			*Builtins::BYTE_READ_RESULT_DATA_NAME, byte_read_alias }, {
			*Builtins::BYTE_READ_RESULT_EOF_NAME, byte_read_eof_type },
			{ *Builtins::BYTE_READ_RESULT_ERRORS_NAME,
					byte_read_error_list_alias } });
	auto byte_read_result_type_table = make_shared<const TypeTable>(
			byte_read_result_type_mapping, type_table);
	auto byte_read_result_maybe = MaybeType::Build(type_table,
			get_byte_read_result_type_specifier())->GetData<MaybeType>();
	auto byte_read_type_constructor_result =
			TypeAliasDeclarationStatement::GetTypeConstructor(
					GetDefaultLocation(), Builtins::BYTE_READ_RESULT_DATA_NAME,
					GetDefaultLocation(), GetDefaultLocation(),
					PrimitiveTypeSpecifier::GetInt(),
					get_byte_read_result_type_specifier(), byte_read_alias,
					result, result);
	assert(
			ErrorList::IsTerminator(
					byte_read_type_constructor_result.GetErrors()));
	auto byte_read_result_constructor_symbols = make_shared<SymbolTable>();
	byte_read_result_constructor_symbols->InsertSymbol(
			*Builtins::BYTE_READ_RESULT_DATA_NAME,
			byte_read_type_constructor_result.GetData());
	auto byte_read_result_type = make_shared<SumType>(
			byte_read_result_type_table, Builtins::BYTE_READ_RESULT_DATA_NAME,
			byte_read_result_constructor_symbols, byte_read_result_maybe);
	type_table->AddType(*Builtins::BYTE_READ_RESULT_TYPE_NAME,
			byte_read_result_type);

//	cout << "Builtin type table:" << endl;
//	type_table->print(cout, Indent(0), DEEP);
//	cout << "End builtin type table." << endl;

	return result;
}

const_shared_ptr<std::string> Builtins::ERROR_TYPE_NAME = make_shared<
		std::string>("error");

const_shared_ptr<std::string> Builtins::ERROR_CODE_NAME = make_shared<
		std::string>("id");

const_shared_ptr<std::string> Builtins::ERROR_MESSAGE_NAME = make_shared<
		std::string>("message");

const_shared_ptr<ComplexTypeSpecifier> Builtins::get_error_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::ERROR_TYPE_NAME);
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
		std::string>("error");

const_shared_ptr<std::string> Builtins::ERROR_LIST_NEXT_NAME = make_shared<
		std::string>("next");

const_shared_ptr<ComplexTypeSpecifier> Builtins::get_error_list_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::ERROR_LIST_TYPE_NAME);
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
			ComplexTypeSpecifier>(Builtins::STREAM_MODE_TYPE_NAME);
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
			ComplexTypeSpecifier>(Builtins::BYTE_RESULT_TYPE_NAME);
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
			ComplexTypeSpecifier>(Builtins::INT_RESULT_TYPE_NAME);
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
			ComplexTypeSpecifier>(Builtins::BYTE_READ_RESULT_TYPE_NAME);
	return instance;
}
const_shared_ptr<ComplexTypeSpecifier> Builtins::get_byte_read_result_eof_type_specifier() {
	static const_shared_ptr<ComplexTypeSpecifier> instance = make_shared<
			ComplexTypeSpecifier>(Builtins::BYTE_READ_RESULT_EOF_NAME,
			get_byte_read_result_type_specifier(),
			NamespaceQualifierList::GetTerminator());
	return instance;
}

volatile_shared_ptr<file_handle_map> Builtins::get_file_handle_map() {
	static volatile_shared_ptr<file_handle_map> instance = make_shared<
			file_handle_map>();
	return instance;
}

volatile_shared_ptr<mutex> Builtins::get_file_handle_map_mutex() {
	static volatile_shared_ptr<mutex> instance = make_shared<mutex>();
	return instance;
}

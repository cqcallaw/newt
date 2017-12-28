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

#include <open_expression.h>
#include <execution_context.h>
#include <builtins.h>
#include <record.h>
#include <sum.h>
#include <fstream>
#include <bitset>
#include <cerrno>
#include <cstring>
#include <constant_expression.h>

OpenExpression::OpenExpression(const yy::location location,
		const_shared_ptr<Expression> expression,
		const ArgumentListRef argument_list,
		const yy::location argument_list_location) :
		InvokeExpression(location, expression, argument_list,
				argument_list_location) {
}

OpenExpression::~OpenExpression() {
}

TypedResult<TypeSpecifier> OpenExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return TypedResult<TypeSpecifier>(Builtins::get_int_result_type_specifier(),
			ErrorList::GetTerminator());
}

const_shared_ptr<Result> OpenExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	// need to handle in-band errors
	// return file handle
	// return platform-dependent error codes; should handle mapping to platform-independent codes in std lib.

	// we assume that our validation pass caught any argument length issues
	auto path_argument_expression = GetArgumentListRef()->GetData();
	auto path_argument_evaluation = path_argument_expression->Evaluate(context,
			closure);

	auto index = 0;
	auto result_code = 0;
	auto result_message = make_shared<string>();
	auto errors = path_argument_evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto path = path_argument_evaluation->GetData<string>();
		auto mode_argument_expression =
				GetArgumentListRef()->GetNext()->GetData();
		auto mode_argument_evaluation = mode_argument_expression->Evaluate(
				context, closure);

		errors = mode_argument_evaluation->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			std::ios::openmode open_mode = std::ios::openmode();
			auto mode_definition =
					mode_argument_evaluation->GetData<Record>()->GetDefinition();

			if (*static_pointer_cast<const bool>(
					mode_definition->GetSymbol(*Builtins::STREAM_MODE_IN_NAME)->GetValue())
					== true) {
				open_mode = open_mode | std::ios::in;
			}

			if (*static_pointer_cast<const bool>(
					mode_definition->GetSymbol(*Builtins::STREAM_MODE_OUT_NAME)->GetValue())
					== true) {
				open_mode = open_mode | std::ios::out;
			}

			if (*static_pointer_cast<const bool>(
					mode_definition->GetSymbol(
							*Builtins::STREAM_MODE_BINARY_NAME)->GetValue())
					== true) {
				open_mode = open_mode | std::ios::binary;
			}

			if (*static_pointer_cast<const bool>(
					mode_definition->GetSymbol(*Builtins::STREAM_MODE_ATE_NAME)->GetValue())
					== true) {
				open_mode = open_mode | std::ios::ate;
			}

			if (*static_pointer_cast<const bool>(
					mode_definition->GetSymbol(*Builtins::STREAM_MODE_APP_NAME)->GetValue())
					== true) {
				open_mode = open_mode | std::ios::app;
			}

			if (*static_pointer_cast<const bool>(
					mode_definition->GetSymbol(
							*Builtins::STREAM_MODE_TRUNC_NAME)->GetValue())
					== true) {
				open_mode = open_mode | std::ios::trunc;
			}

			auto stream = make_shared<fstream>();
			stream->exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				stream->open(*path, open_mode);
				// add stream to file handle list
				// N.B. that this process is not synchronized
				index = Builtins::get_file_handle_map()->size();
				Builtins::get_file_handle_map()->insert(
						std::pair<int, volatile_shared_ptr<fstream>>(index,
								stream));
			} catch (std::fstream::failure &e) {
				result_code = errno;
				result_message = make_shared<string>(
						std::strerror(result_code));
			}
		}
	}

	auto type_table = closure->GetTypeTable();
	auto error_list_type_result =
			Builtins::get_error_list_type_specifier()->GetType(type_table,
					RETURN);
	assert(ErrorList::IsTerminator(error_list_type_result->GetErrors()));
	auto error_list_type = error_list_type_result->GetData<TypeDefinition>();

	if (result_code == 0) {
		auto result = make_shared<Sum>(Builtins::INT_RESULT_DATA_NAME,
				make_shared<int>(index));

		return make_shared<Result>(result, errors);
	} else {
		auto error_symbol_map = make_shared<symbol_map>();
		auto insert_result = error_symbol_map->insert(
				std::pair<const string, const_shared_ptr<Symbol>>(
						*Builtins::ERROR_CODE_NAME,
						make_shared<Symbol>(make_shared<int>(result_code))));
		assert(insert_result.second);
		insert_result = error_symbol_map->insert(
				std::pair<const string, const_shared_ptr<Symbol>>(
						*Builtins::ERROR_MESSAGE_NAME,
						make_shared<Symbol>(result_message)));
		assert(insert_result.second);
		auto error_symbol_table = make_shared<SymbolTable>(Modifier::Type::NONE,
				error_symbol_map);
		auto error = make_shared<Record>(error_symbol_table);

		auto error_list_symbol_map = make_shared<symbol_map>();
		insert_result = error_list_symbol_map->insert(
				std::pair<const string, const_shared_ptr<Symbol>>(
						*Builtins::ERROR_LIST_DATA_NAME,
						make_shared<Symbol>(
								Builtins::get_error_type_specifier(), error)));
		assert(insert_result.second);

		auto terminator = static_pointer_cast<const Record>(
				error_list_type->GetDefaultValue(type_table));
		auto error_list_sum = make_shared<Sum>(TypeTable::GetNilName(),
				terminator);
		insert_result = error_list_symbol_map->insert(
				std::pair<const string, const_shared_ptr<Symbol>>(
						*Builtins::ERROR_LIST_NEXT_NAME,
						make_shared<Symbol>(
								Builtins::get_error_list_maybe_type_specifier(),
								error_list_sum)));
		assert(insert_result.second);

		auto error_list_symbol_table = make_shared<SymbolTable>(
				Modifier::Type::NONE, error_list_symbol_map);
		auto error_list = make_shared<Record>(error_list_symbol_table);

		auto result = make_shared<Sum>(Builtins::INT_RESULT_ERRORS_NAME,
				error_list);
		return make_shared<Result>(result, errors);
	}
}

TypedResult<string> OpenExpression::ToString(
		const shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buf;
	buf << "open(";
	auto errors = ErrorList::GetTerminator();
	ArgumentListRef argument = GetArgumentListRef();
	while (!ArgumentList::IsTerminator(argument)) {
		auto argument_result = argument->GetData()->ToString(execution_context);
		errors = ErrorList::Concatenate(errors, argument_result.GetErrors());
		if (ErrorList::IsTerminator(errors)) {
			buf << *(argument_result.GetData());
			if (!ArgumentList::IsTerminator(argument->GetNext())) {
				buf << ",";
			}
		}

		argument = argument->GetNext();
	}
	buf << ")";
	return TypedResult<string>(const_shared_ptr<string>(new string(buf.str())),
			errors);
}

const ErrorListRef OpenExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	// check argument count
	auto errors = ErrorList::GetTerminator();
	auto type_table = execution_context->GetTypeTable();

	uint arg_count = 0;
	ArgumentListRef argument = GetArgumentListRef();
	while (!ArgumentList::IsTerminator(argument)) {
		auto argument_subject = argument->GetData();
		auto validation = argument_subject->Validate(execution_context);
		errors = ErrorList::Concatenate(errors, validation);

		if (ErrorList::IsTerminator(errors)) {
			auto argument_type_specifier_result =
					argument_subject->GetTypeSpecifier(execution_context);

			errors = ErrorList::Concatenate(errors,
					argument_type_specifier_result.GetErrors());
			if (ErrorList::IsTerminator(errors)) {
				auto argument_type_specifier =
						argument_type_specifier_result.GetData();

				if (arg_count == 0) {
					auto specifier = PrimitiveTypeSpecifier::GetString();
					auto assignability =
							argument_type_specifier->AnalyzeAssignmentTo(
									specifier, type_table);
					if (assignability == AnalysisResult::AMBIGUOUS) {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::AMBIGUOUS_WIDENING_CONVERSION,
												argument_type_specifier->GetLocation().begin.line,
												argument_type_specifier->GetLocation().begin.column,
												argument_type_specifier->ToString(),
												specifier->ToString()), errors);
					} else if (assignability == INCOMPATIBLE) {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::ASSIGNMENT_TYPE_ERROR,
												argument_type_specifier->GetLocation().begin.line,
												argument_type_specifier->GetLocation().begin.column,
												argument_type_specifier->ToString(),
												specifier->ToString()), errors);
					}
				} else if (arg_count == 1) {
					auto specifier = Builtins::get_stream_mode_type_specifier();
					auto assignability =
							argument_type_specifier->AnalyzeAssignmentTo(
									specifier, type_table);
					if (assignability == AnalysisResult::AMBIGUOUS) {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::AMBIGUOUS_WIDENING_CONVERSION,
												argument_type_specifier->GetLocation().begin.line,
												argument_type_specifier->GetLocation().begin.column,
												argument_type_specifier->ToString(),
												specifier->ToString()), errors);
					} else if (assignability == INCOMPATIBLE) {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::ASSIGNMENT_TYPE_ERROR,
												argument_type_specifier->GetLocation().begin.line,
												argument_type_specifier->GetLocation().begin.column,
												argument_type_specifier->ToString(),
												specifier->ToString()), errors);
					}
				} else {
					// too many arguments
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::TOO_MANY_ARGUMENTS,
											argument_subject->GetLocation().begin.line,
											argument_subject->GetLocation().begin.column,
											"(string, stream_mode) -> int_result"),
									errors);
					break;
				}
			}
		}

		arg_count++;
		argument = argument->GetNext();
	}

	return errors;
}

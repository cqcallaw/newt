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

#include <close_expression.h>
#include <execution_context.h>
#include <builtins.h>
#include <fstream>
#include <unit_type.h>
#include <cerrno>
#include <cstring>
#include <sum.h>
#include <record.h>
#include <maybe_type_specifier.h>

CloseExpression::CloseExpression(const yy::location location,
		const_shared_ptr<Expression> expression,
		const ArgumentListRef argument_list,
		const yy::location argument_list_location) :
		InvokeExpression(location, expression, argument_list,
				argument_list_location) {
}

CloseExpression::~CloseExpression() {
}

TypedResult<TypeSpecifier> CloseExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return TypedResult<TypeSpecifier>(
			Builtins::get_error_list_maybe_type_specifier(),
			ErrorList::GetTerminator());
}

const_shared_ptr<Result> CloseExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto file_handle_expression = GetArgumentListRef()->GetData();
	auto file_handle_evaluation = file_handle_expression->Evaluate(context,
			closure);

	auto result_code = 0;
	char* result_message;
	auto errors = file_handle_evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto file_handle = file_handle_evaluation->GetData<int>();

		auto map = Builtins::get_file_handle_map();

		Builtins::get_file_handle_map_mutex()->lock();
		auto handle_entry = map->find(*file_handle);
		if (handle_entry != map->end()) {
			try {
				handle_entry->second->close();
			} catch (std::fstream::failure &e) {
				result_code = errno;
				result_message = std::strerror(result_code);
			}
			map->erase(handle_entry);
		}
		Builtins::get_file_handle_map_mutex()->unlock();
	}

	auto type_table = closure->GetTypeTable();
	auto error_list_type = type_table->GetType<TypeDefinition>(
			Builtins::get_error_list_type_specifier(), DEEP);
	auto terminator = static_pointer_cast<const Record>(
			error_list_type->GetDefaultValue(type_table));
	if (result_code == 0) {
		auto result = make_shared<Sum>(TypeTable::GetNilName(), terminator);
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
						make_shared<Symbol>(
								make_shared<string>(result_message))));
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

		auto result = make_shared<Sum>(MaybeTypeSpecifier::VARIANT_NAME,
				error_list);
		return make_shared<Result>(result, errors);
	}
}

TypedResult<string> CloseExpression::ToString(
		const shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buf;
	buf << "close(";
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

const ErrorListRef CloseExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
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
					auto specifier = PrimitiveTypeSpecifier::GetInt();
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
											"(int) -> error_list?"), errors);
					break;
				}
			}
		}

		arg_count++;
		argument = argument->GetNext();
	}

	return errors;
}

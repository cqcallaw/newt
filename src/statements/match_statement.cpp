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

#include <match_statement.h>
#include <statement_block.h>
#include <expression.h>
#include <sum_type_specifier.h>
#include <sum_type.h>
#include <sum.h>
#include <execution_context.h>
#include <symbol.h>

MatchStatement::~MatchStatement() {
}

const ErrorListRef MatchStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors;
	errors = m_source_expression->Validate(execution_context);

	if (ErrorList::IsTerminator(errors)) {
		auto expression_type = m_source_expression->GetType(execution_context);

		auto expression_type_as_sum = dynamic_pointer_cast<
				const SumTypeSpecifier>(expression_type);
		if (expression_type_as_sum) {

			auto sum_type = execution_context->GetTypeTable()->GetType<SumType>(
					expression_type_as_sum->GetTypeName());

			shared_ptr<std::set<std::string>> match_names = make_shared<
					std::set<std::string>>();

			if (sum_type) {
				bool complete_match = false;

				auto match_list = m_match_list;
				while (!MatchList::IsTerminator(match_list)) {
					auto match = match_list->GetData();
					auto match_name = match->GetName();
					auto match_body = match->GetBlock();

					auto variant_type = sum_type->GetTypeTable()->GetType<
							TypeDefinition>(match_name);
					if (variant_type) {
						match_names->insert(*match_name);
						auto block_context = ExecutionContext::GetEmptyChild(
								execution_context, Modifier::NONE, EPHEMERAL);

						const_shared_ptr<void> default_value =
								variant_type->GetDefaultValue(match_name);
						const_shared_ptr<Symbol> default_symbol =
								variant_type->GetSymbol(default_value);
						block_context->InsertSymbol(*match_name,
								default_symbol);

						auto block_errors = match_body->preprocess(
								block_context);
						errors = ErrorList::Concatenate(errors, block_errors);
					} else if (*match_name == "_") {
						complete_match = true;
						auto block_context = ExecutionContext::GetEmptyChild(
								execution_context, Modifier::NONE, EPHEMERAL);
						auto block_errors = match_body->preprocess(
								block_context);
						errors = ErrorList::Concatenate(errors, block_errors);
					} else {
						errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::UNDECLARED_TYPE,
										match->GetNameLocation().begin.line,
										match->GetNameLocation().begin.column,
										expression_type_as_sum->ToString() + "."
												+ *match_name), errors);
					}

					match_list = match_list->GetNext();
				}

				if (!complete_match) {
					//we don't have a default clause; check for total match coverage
					auto sum_table = sum_type->GetTypeTable();
					auto variant_names = sum_table->GetTypeNames();

					if (*variant_names != *match_names) {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::INCOMPLETE_MATCH,
												m_source_expression->GetPosition().begin.line,
												m_source_expression->GetPosition().begin.column,
												expression_type->ToString()),
										errors);
					}
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::UNDECLARED_TYPE,
								m_source_expression->GetPosition().begin.line,
								m_source_expression->GetPosition().begin.column,
								expression_type_as_sum->ToString()), errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::MATCH_REQUIRES_SUM,
							m_source_expression->GetPosition().begin.line,
							m_source_expression->GetPosition().begin.column,
							expression_type->ToString()), errors);
		}
	}

	return errors;
}

const ErrorListRef MatchStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors;

	auto expression_type = m_source_expression->GetType(execution_context);
	auto expression_type_as_sum = dynamic_pointer_cast<const SumTypeSpecifier>(
			expression_type);
	if (expression_type_as_sum) {
		auto result = m_source_expression->Evaluate(execution_context);
		errors = result->GetErrors();

		if (ErrorList::IsTerminator(errors)) {
			auto as_sum = static_pointer_cast<const Sum>(result->GetData());
			auto tag = *as_sum->GetTag();

			auto sum_type = execution_context->GetTypeTable()->GetType<SumType>(
					expression_type_as_sum->GetTypeName());
			if (sum_type) {
				bool matched = false;
				auto match_list = m_match_list;
				while (!MatchList::IsTerminator(match_list)) {
					auto match = match_list->GetData();
					auto match_name = *match->GetName();
					auto match_body = match->GetBlock();

					if (match_name == tag) {
						matched = true;

						auto variant_type = sum_type->GetTypeTable()->GetType<
								TypeDefinition>(match_name);
						if (variant_type) {
							auto block_context =
									ExecutionContext::GetEmptyChild(
											execution_context, Modifier::NONE,
											EPHEMERAL);

							const_shared_ptr<Symbol> default_symbol =
									variant_type->GetSymbol(as_sum->GetValue());
							block_context->InsertSymbol(match_name,
									default_symbol);

							auto block_errors = match_body->execute(
									block_context);
							errors = ErrorList::Concatenate(errors,
									block_errors);
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::UNDECLARED_TYPE,
													match->GetNameLocation().begin.line,
													match->GetNameLocation().begin.column,
													expression_type_as_sum->ToString()
															+ "." + match_name),
											errors);
						}
						break;
					} else if (match_name == "_") {
						matched = true;
						auto block_context = ExecutionContext::GetEmptyChild(
								execution_context, Modifier::NONE, EPHEMERAL);

						auto block_errors = match_body->execute(block_context);
						errors = ErrorList::Concatenate(errors, block_errors);
					}

					match_list = match_list->GetNext();
				}

				if (!matched) {

					errors =
							ErrorList::From(
									make_shared<Error>(Error::RUNTIME,
											Error::MATCH_FAILURE,
											m_source_expression->GetPosition().begin.line,
											m_source_expression->GetPosition().begin.column,
											expression_type->ToString()),
									errors);
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::UNDECLARED_TYPE,
								m_source_expression->GetPosition().begin.line,
								m_source_expression->GetPosition().begin.column,
								expression_type_as_sum->ToString()), errors);
			}

		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::MATCH_REQUIRES_SUM,
						m_source_expression->GetPosition().begin.line,
						m_source_expression->GetPosition().begin.column,
						expression_type->ToString()), errors);
	}

	return errors;
}

const ErrorListRef MatchStatement::GetReturnStatementErrors(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors;

	auto match = m_match_list;
	while (!MatchList::IsTerminator(match)) {
		auto match_body = match->GetData()->GetBlock();
		errors = ErrorList::Concatenate(errors,
				match_body->GetReturnStatementErrors(type_specifier,
						execution_context));
		match = match->GetNext();
	}

	return errors;
}

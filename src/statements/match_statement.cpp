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
#include <maybe_type.h>
#include <sum.h>
#include <execution_context.h>
#include <symbol.h>
#include <iterator>
#include <sstream>
#include <nested_type_specifier.h>
#include <maybe_type_specifier.h>
#include <unit_type.h>
#include <return_statement.h>

MatchStatement::MatchStatement(const yy::location statement_location,
		const_shared_ptr<Expression> source_expression,
		const MatchListRef match_list, const yy::location match_list_location) :
		m_statement_location(statement_location), m_source_expression(
				source_expression), m_match_list(match_list), m_match_list_location(
				match_list_location), m_match_contexts(
				GenerateMatchContexts(match_list)) {
	assert(m_match_contexts);
}

MatchStatement::~MatchStatement() {
}

const PreprocessResult MatchStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors;
	errors = m_source_expression->Validate(context);

	auto return_coverage = PreprocessResult::ReturnCoverage::NONE;
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier_result =
				m_source_expression->GetTypeSpecifier(context);

		errors = expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();

			auto expression_type_result = expression_type_specifier->GetType(
					context->GetTypeTable(), RESOLVE);

			errors = expression_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto expression_type = expression_type_result->GetData<
						TypeDefinition>();

				shared_ptr<const TypeTable> type_definition;

				auto sum_type = dynamic_pointer_cast<const SumType>(
						expression_type);
				if (sum_type) {
					type_definition = sum_type->GetDefinition();
				}

				if (type_definition) {
					auto source_sum_specifier = dynamic_pointer_cast<
							const ComplexTypeSpecifier>(
							expression_type_specifier);
					assert(source_sum_specifier);

					auto match_names = make_shared<std::set<std::string>>();

					shared_ptr<const StatementBlock> default_match_block =
							nullptr;
					shared_ptr<ExecutionContext> default_match_context = nullptr;

					auto match_list = m_match_list;
					auto match_context = m_match_contexts;
					auto initial_state = true;
					while (!MatchList::IsTerminator(match_list)) {
						auto match = match_list->GetData();
						auto match_name = match->GetName();
						auto alias_name = match->GetAlias();
						auto match_body = match->GetBlock();

						if (*match_name == *Match::DEFAULT_MATCH_NAME) {
							// N.B. we must not do any preprocessing here, since an explicit match may still exist in the match list
							// explicit matches always take precedence over default match blocks
							auto matched_context = match_context->GetData();
							assert(matched_context);
							matched_context->LinkToParent(context);

							default_match_context = matched_context;
							default_match_block = match_body;
						} else {
							auto variant_type = type_definition->GetType<
									TypeDefinition>(match_name, SHALLOW,
									RETURN);
							if (variant_type) {
								if (match_names->find(*match_name)
										== match_names->end()) {
									match_names->insert(*match_name);
									auto matched_context =
											match_context->GetData();
									assert(matched_context);
									matched_context->LinkToParent(context);

									plain_shared_ptr<TypeSpecifier> variant_type_specifier;
									auto as_alias = dynamic_pointer_cast<
											const AliasDefinition>(
											variant_type);
									if (as_alias) {
										variant_type_specifier =
												as_alias->GetOriginal();
									} else {
										variant_type_specifier =
												variant_type->GetTypeSpecifier(
														match->GetName(),
														source_sum_specifier,
														GetDefaultLocation());
									}

									const_shared_ptr<void> default_value =
											variant_type->GetDefaultValue(
													*type_definition);
									const_shared_ptr<Symbol> default_symbol =
											variant_type->GetSymbol(
													context->GetTypeTable(),
													variant_type_specifier,
													default_value);
									matched_context->InsertSymbol(*alias_name,
											default_symbol);

									auto match_preprocess_result =
											match_body->Preprocess(
													matched_context,
													return_type_specifier);
									auto match_return_coverage =
											match_preprocess_result.GetReturnCoverage();

									return_coverage =
											ReturnStatement::CoverageTransition(
													return_coverage,
													match_return_coverage,
													initial_state);
									initial_state = false;

									errors =
											ErrorList::Concatenate(errors,
													match_preprocess_result.GetErrors());
								} else {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::DUPLICATE_MATCH_BLOCK,
															match->GetNameLocation().begin.line,
															match->GetNameLocation().begin.column,
															*match_name),
													errors);
								}
							} else {
								errors =
										ErrorList::From(
												make_shared<Error>(
														Error::SEMANTIC,
														Error::UNDECLARED_TYPE,
														match->GetNameLocation().begin.line,
														match->GetNameLocation().begin.column,
														expression_type_specifier->ToString()
																+ "."
																+ *match_name),
												errors);
							}
						}

						initial_state = false;
						match_context = match_context->GetNext();
						match_list = match_list->GetNext();
					}

					auto variant_names = type_definition->GetTypeNames();
					if (*variant_names != *match_names) {
						// make sure partial matches are made complete by a default match block
						if (default_match_block) {
							//we have a default match block; preprocess it
							auto default_block_preprocess_result =
									default_match_block->Preprocess(
											default_match_context,
											return_type_specifier);

							return_coverage =
									ReturnStatement::CoverageTransition(
											return_coverage,
											default_block_preprocess_result.GetReturnCoverage(),
											initial_state);

							errors =
									ErrorList::Concatenate(errors,
											default_block_preprocess_result.GetErrors());
						} else {
							std::set<std::string> difference;
							std::set_difference(variant_names->begin(),
									variant_names->end(), match_names->begin(),
									match_names->end(),
									std::inserter(difference,
											difference.begin()));

							ostringstream buffer;
							std::copy(difference.begin(), difference.end(),
									std::ostream_iterator<string>(buffer,
											", "));

							string result = buffer.str();
							//ref: http://stackoverflow.com/a/16630332/577298
							result.pop_back();
							result.pop_back();

							errors = ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::INCOMPLETE_MATCH,
											m_statement_location.begin.line,
											m_statement_location.begin.column,
											result), errors);
						}
					} else if (default_match_block) {
						//make sure there aren't any extraneous default match blocks
						errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::EXTRANEOUS_DEFAULT_MATCH,
										m_statement_location.begin.line,
										m_statement_location.begin.column),
								errors);
					}
				} else {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::MATCH_REQUIRES_SUM,
											m_source_expression->GetLocation().begin.line,
											m_source_expression->GetLocation().begin.column,
											expression_type_specifier->ToString()),
									errors);
				}
			}
		}
	}

	return PreprocessResult(return_coverage, errors);
}

const ErrorListRef MatchStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto expression_type_specifier_result =
			m_source_expression->GetTypeSpecifier(context);

	auto errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		auto expression_type_result = expression_type_specifier->GetType(
				context->GetTypeTable());

		errors = expression_type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type = expression_type_result->GetData<
					TypeDefinition>();

			auto sum_type = dynamic_pointer_cast<const SumType>(
					expression_type);
			if (sum_type) {
				auto source_sum_specifier = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(expression_type_specifier);
				assert(source_sum_specifier);

				auto result = m_source_expression->Evaluate(context, closure);
				errors = result->GetErrors();

				if (ErrorList::IsTerminator(errors)) {
					auto as_sum = result->GetData<Sum>(); //TODO: validate the assumption that this result yields a sum
					auto tag = *as_sum->GetTag();

					shared_ptr<const StatementBlock> default_match_block =
							nullptr;
					shared_ptr<ExecutionContext> default_match_context = nullptr;

					bool matched = false;
					auto match_list = m_match_list;
					auto match_context = m_match_contexts;
					while (!MatchList::IsTerminator(match_list)) {
						auto match = match_list->GetData();
						auto match_name = *match->GetName();
						auto match_body = match->GetBlock();

						if (match_name == tag) {
							matched = true;
							auto variant_type =
									sum_type->GetDefinition()->GetType<
											TypeDefinition>(match_name, SHALLOW,
											RETURN);
							if (variant_type) {
								auto matched_context = match_context->GetData();
								auto matched_parent =
										matched_context->GetParent()->GetData();

								assert(matched_parent == context);
								assert(
										matched_parent->GetTypeTable()
												== context->GetTypeTable());

								plain_shared_ptr<TypeSpecifier> variant_type_specifier;
								auto as_alias = dynamic_pointer_cast<
										const AliasDefinition>(variant_type);
								if (as_alias) {
									variant_type_specifier =
											as_alias->GetOriginal();
								} else {
									variant_type_specifier =
											variant_type->GetTypeSpecifier(
													match->GetName(),
													source_sum_specifier,
													GetDefaultLocation());
								}

								auto alias_name = *(match->GetAlias());
								auto set_result = matched_context->SetSymbol(
										alias_name, variant_type_specifier,
										as_sum->GetValue(),
										context->GetTypeTable());
								assert(set_result == SET_SUCCESS);

								auto block_errors = match_body->Execute(
										matched_context);
								context->SetReturnValue(
										matched_context->GetReturnValue());

								errors = ErrorList::Concatenate(errors,
										block_errors);
							} else {
								errors =
										ErrorList::From(
												make_shared<Error>(
														Error::SEMANTIC,
														Error::UNDECLARED_TYPE,
														match->GetNameLocation().begin.line,
														match->GetNameLocation().begin.column,
														expression_type_specifier->ToString()
																+ "."
																+ match_name),
												errors);
							}
							break;
						} else if (match_name == *Match::DEFAULT_MATCH_NAME) {
							auto matched_context = match_context->GetData();
							auto matched_parent =
									matched_context->GetParent()->GetData();

							assert(matched_parent == context);
							assert(
									matched_parent->GetTypeTable()
											== context->GetTypeTable());

							default_match_context = matched_context;
							default_match_block = match_body;
						}

						match_context = match_context->GetNext();
						match_list = match_list->GetNext();
					}

					if (!matched) {
						if (default_match_block) {
							auto block_errors = default_match_block->Execute(
									default_match_context);
							context->SetReturnValue(
									default_match_context->GetReturnValue());

							errors = ErrorList::Concatenate(errors,
									block_errors);
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::RUNTIME,
													Error::MATCH_FAILURE,
													m_source_expression->GetLocation().begin.line,
													m_source_expression->GetLocation().begin.column,
													expression_type_specifier->ToString()),
											errors);
						}
					}
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::MATCH_REQUIRES_SUM,
								m_source_expression->GetLocation().begin.line,
								m_source_expression->GetLocation().begin.column,
								expression_type_specifier->ToString()), errors);
			}
		}
	}

	return errors;
}

const MatchContextListRef MatchStatement::GenerateMatchContexts(
		const MatchListRef match_list) {
	auto subject = match_list;

	MatchContextListRef result = MatchContextList::GetTerminator();
	while (!MatchList::IsTerminator(subject)) {
		auto context = make_shared<ExecutionContext>(Modifier::Type::MUTABLE);
		result = MatchContextList::From(context, result);
		subject = subject->GetNext();
	}

	return result;
}


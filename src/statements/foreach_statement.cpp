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

#include <foreach_statement.h>
#include <expression.h>
#include <statement_block.h>
#include <execution_context.h>
#include <record_type.h>
#include <function_type_specifier.h>
#include <variant_function_specifier.h>
#include <maybe_type.h>
#include <maybe_type_specifier.h>

const_shared_ptr<std::string> ForeachStatement::NEXT_NAME = make_shared<
		std::string>("next");

ForeachStatement::ForeachStatement(const_shared_ptr<string> name,
		const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> statement_block) :
		m_name(name), m_expression(expression), m_statement_block(
				statement_block), m_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)) {
}

ForeachStatement::~ForeachStatement() {
}

const PreprocessResult ForeachStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = ErrorList::GetTerminator();
	auto return_coverage = PreprocessResult::ReturnCoverage::NONE;
	auto type_table = context->GetTypeTable();

	m_block_context->LinkToParent(context);

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			context, RESOLVE);
	auto expression_type_specifier_errors =
			expression_type_specifier_result.GetErrors();

	if (ErrorList::IsTerminator(expression_type_specifier_errors)) {
		auto validation_errors = m_expression->Validate(context);

		if (ErrorList::IsTerminator(validation_errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();

			auto expression_type_result = expression_type_specifier->GetType(
					type_table, RESOLVE);
			auto expression_type_errors = expression_type_result->GetErrors();
			if (ErrorList::IsTerminator(expression_type_errors)) {
				auto expression_type = expression_type_result->GetData<
						TypeDefinition>();
				auto as_record = dynamic_pointer_cast<const RecordType>(
						expression_type);

				if (as_record) {
					auto complex_expression_type_specifier =
							dynamic_pointer_cast<const ComplexTypeSpecifier>(
									expression_type_specifier);
					auto next_member = as_record->GetMember(
							*ForeachStatement::NEXT_NAME);
					if (next_member) {
						auto member_type_specifier =
								next_member->GetTypeSpecifier(
										ForeachStatement::NEXT_NAME,
										complex_expression_type_specifier,
										GetDefaultLocation());

						auto as_function_type_specifier = dynamic_pointer_cast<
								const FunctionTypeSpecifier>(
								member_type_specifier);
						if (as_function_type_specifier) {
							member_type_specifier =
									as_function_type_specifier->GetReturnTypeSpecifier();
						}

						auto member_as_maybe_specifier = dynamic_pointer_cast<
								const MaybeTypeSpecifier>(
								member_type_specifier);
						if (member_as_maybe_specifier) {
							auto member_base_specifier =
									member_as_maybe_specifier->GetBaseTypeSpecifier();
							auto maybe_type_relation =
									member_base_specifier->AnalyzeAssignmentTo(
											expression_type_specifier,
											context->GetTypeTable());
							if (maybe_type_relation == EQUIVALENT) {
								const_shared_ptr<void> default_value =
										expression_type->GetDefaultValue(
												*type_table);
								const_shared_ptr<Symbol> default_symbol =
										expression_type->GetSymbol(
												context->GetTypeTable(),
												expression_type_specifier,
												default_value);
								m_block_context->InsertSymbol(*m_name,
										default_symbol);
							} else {
								errors =
										ErrorList::From(
												make_shared<Error>(
														Error::SEMANTIC,
														Error::FOREACH_NEXT_MUST_EVALUATE_TO_BASE_TYPE,
														m_expression->GetLocation().begin.line,
														m_expression->GetLocation().begin.column),
												errors);
							}
						} else {
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::FOREACH_NEXT_MUST_BE_MAYBE,
													m_expression->GetLocation().begin.line,
													m_expression->GetLocation().begin.column),
											errors);
						}
					} else {
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::FOREACH_STMT_REQUIRES_NEXT,
												m_expression->GetLocation().begin.line,
												m_expression->GetLocation().begin.column),
										errors);
					}
				} else {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::FOREACH_STMT_SOURCE_MUST_BE_RECORD,
									m_expression->GetLocation().begin.line,
									m_expression->GetLocation().begin.column),
							errors);
				}
			} else {
				errors = expression_type_errors;
			}
		} else {
			errors = validation_errors;
		}
	} else {
		errors = expression_type_specifier_errors;
	}

	auto block_preprocess_result = m_statement_block->Preprocess(
			m_block_context, return_type_specifier);
	return_coverage = block_preprocess_result.GetReturnCoverage();

	errors = ErrorList::Concatenate(errors,
			block_preprocess_result.GetErrors());

	return PreprocessResult(return_coverage, errors);
}

const ErrorListRef ForeachStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto errors = ErrorList::GetTerminator();
	return errors;
}

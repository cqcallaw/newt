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
#include <statement_block.h>
#include <execution_context.h>
#include <record_type.h>
#include <variant_function_specifier.h>
#include <function_declaration.h>
#include <maybe_type.h>
#include <maybe_type_specifier.h>
#include <sum.h>
#include <record.h>
#include <basic_variable.h>
#include <variable_expression.h>
#include <unit_type.h>

const_shared_ptr<std::string> ForeachStatement::NEXT_NAME = make_shared<
		std::string>("next");

ForeachStatement::ForeachStatement(
		const_shared_ptr<string> evaluation_identifier,
		const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> statement_block) :
		m_evaluation_identifier(evaluation_identifier), m_expression(
				expression), m_statement_block(statement_block), m_block_context(
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
				auto soure_type_specifier = expression_type_specifier;

				auto as_maybe = dynamic_pointer_cast<const MaybeType>(
						expression_type);
				if (as_maybe) {
					expression_type = as_maybe->GetDefinition()->GetType<
							TypeDefinition>(MaybeTypeSpecifier::VARIANT_NAME,
							DEEP, RESOLVE);
					auto expression_specifier_as_maybe = dynamic_pointer_cast<
							const MaybeTypeSpecifier>(soure_type_specifier);
					expression_type_specifier =
							expression_specifier_as_maybe->GetBaseTypeSpecifier();
				}

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
						// TODO: disallow multi-variant functions

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
								auto default_value =
										expression_type->GetDefaultValue(
												*type_table);

								auto default_symbol =
										expression_type->GetSymbol(
												context->GetTypeTable(),
												expression_type_specifier,
												default_value);
								m_block_context->InsertSymbol(
										*m_evaluation_identifier,
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
													Error::RETURN_STMT_MUST_BE_MAYBE,
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
									Error::STMT_SOURCE_MUST_BE_RECORD,
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

const ExecutionResult ForeachStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto execution_context = ExecutionContext::GetRuntimeInstance(
			m_block_context, context);

	auto errors = ErrorList::GetTerminator();

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			context, RESOLVE);
	auto expression_type_specifier_errors =
			expression_type_specifier_result.GetErrors();

	if (ErrorList::IsTerminator(expression_type_specifier_errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		auto eval = m_expression->Evaluate(context, closure);
		errors = ErrorList::Concatenate(errors, eval->GetErrors());
		if (ErrorList::IsTerminator(errors)) {
			auto raw_value = eval->GetRawData();
			auto tag = make_shared<const string>("BEGIN THE LOOP");

			plain_shared_ptr<ComplexTypeSpecifier> source_type_specifier;
			auto as_maybe_specifier = dynamic_pointer_cast<
					const MaybeTypeSpecifier>(expression_type_specifier);
			if (as_maybe_specifier) {
				source_type_specifier = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(
						as_maybe_specifier->GetBaseTypeSpecifier());

				auto as_sum = static_pointer_cast<const Sum>(raw_value);
				tag = as_sum->GetTag();
				raw_value = as_sum->GetValue();
			} else {
				source_type_specifier = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(expression_type_specifier);
			}

			while (ErrorList::IsTerminator(errors)
					&& *tag != *TypeTable::GetNilName()) {
				auto record = static_pointer_cast<const Record>(raw_value);
				assert(record);
				auto symbol = make_shared<const Symbol>(source_type_specifier,
						record);
				auto set_result = execution_context->SetSymbol(
						*m_evaluation_identifier, source_type_specifier, record,
						context->GetTypeTable());
				assert(set_result == SET_SUCCESS);

				auto execution_result = m_statement_block->Execute(
						execution_context);
				errors = execution_result.GetErrors();

				if (!ErrorList::IsTerminator(errors)) {
					break;
				}

				auto return_value = execution_result.GetReturnValue();
				if (return_value != Symbol::GetDefaultSymbol()) {
					return ExecutionResult(return_value);
				}

				auto definition = record->GetDefinition();
				auto next_member = definition->GetSymbol(
						ForeachStatement::NEXT_NAME);

				plain_shared_ptr<Sum> next_value = nullptr;
				auto member_type_specifier = next_member->GetTypeSpecifier();
				auto as_function_type_specifier = dynamic_pointer_cast<
						const FunctionTypeSpecifier>(member_type_specifier);
				if (as_function_type_specifier) {
					auto function_eval = EvaluateMemberFunction(record,
							source_type_specifier, ForeachStatement::NEXT_NAME,
							context, closure);

					auto function_eval_errors = function_eval->GetErrors();
					if (ErrorList::IsTerminator(function_eval_errors)) {
						next_value = function_eval->GetData<Sum>();
						assert(next_value);
						tag = next_value->GetTag();
						raw_value = next_value->GetValue();
					} else {
						errors = ErrorList::Concatenate(errors,
								function_eval_errors);
					}
				} else {
					next_value = static_pointer_cast<const Sum>(
							next_member->GetValue());
					assert(next_value);

					tag = next_value->GetTag();
					raw_value = next_value->GetValue();
				}
			}
		}
	}

	return ExecutionResult(errors);
}

const_shared_ptr<Result> ForeachStatement::EvaluateMemberFunction(
		const_shared_ptr<Record> record,
		const_shared_ptr<ComplexTypeSpecifier> record_type_specifier,
		const_shared_ptr<std::string> member_name,
		const shared_ptr<ExecutionContext> execution_context,
		const shared_ptr<ExecutionContext> closure) {
	auto errors = ErrorList::GetTerminator();
	plain_shared_ptr<void> value = nullptr;

	auto member = record->GetDefinition()->GetSymbol(member_name);
	if (member != Symbol::GetDefaultSymbol()) {
		auto member_type_specifier = member->GetTypeSpecifier();
		auto as_function_type_specifier = dynamic_pointer_cast<
				const FunctionTypeSpecifier>(member_type_specifier);
		if (as_function_type_specifier) {
			// do function invocation
			auto as_function = static_pointer_cast<const Function>(
					member->GetValue());

			auto function_type_specifier = as_function->GetTypeSpecifier();
			auto as_function_declaration = dynamic_pointer_cast<
					const FunctionDeclaration>(function_type_specifier);
			assert(as_function_declaration); // N.B. that this asserts that we have a single-variant function

			auto invocation_context = ExecutionContext::GetEmptyChild(
					execution_context, Modifier::Type::MUTABLE, EPHEMERAL);

			auto parameter_list = as_function_declaration->GetParameterList();
			auto argument_list = ArgumentList::GetTerminator();
			while (ErrorList::IsTerminator(errors)
					&& !DeclarationList::IsTerminator(parameter_list)) {
				auto parameter_declaration = parameter_list->GetData();

				auto declaration_type_specifier =
						parameter_declaration->GetTypeSpecifier();
				auto declaration_type_result =
						declaration_type_specifier->GetType(
								closure->GetTypeTable(), RETURN);

				auto type_lookup_errors = declaration_type_result->GetErrors();
				if (ErrorList::IsTerminator(type_lookup_errors)) {
					auto declaration_type = declaration_type_result->GetData<
							TypeDefinition>();

					// set symbol
					auto maybe_wrapper = make_shared<Sum>(
							MaybeTypeSpecifier::VARIANT_NAME, record);
					auto symbol = declaration_type->GetSymbol(
							closure->GetTypeTable(), declaration_type_specifier,
							maybe_wrapper);
					auto insert_result = invocation_context->InsertSymbol(
							*parameter_declaration->GetName(), symbol);
					assert(insert_result == INSERT_SUCCESS);

					// generate argument list
					auto argument_variable = make_shared<const BasicVariable>(
							parameter_declaration->GetName(),
							parameter_declaration->GetLocation());

					auto argument_expression = make_shared<
							const VariableExpression>(
							parameter_declaration->GetLocation(),
							argument_variable);

					argument_list = ArgumentList::From(argument_expression,
							argument_list);
				} else {
					errors = ErrorList::Concatenate(errors, type_lookup_errors);
				}
				parameter_list = parameter_list->GetNext();
			}

			if (ErrorList::IsTerminator(errors)) {
				// make sure argument list order matches parameter order
				argument_list = ArgumentList::Reverse(argument_list);

				auto function_eval = as_function->Evaluate(argument_list,
						as_function_declaration->GetLocation(),
						invocation_context);

				auto function_eval_errors = function_eval->GetErrors();
				if (ErrorList::IsTerminator(function_eval_errors)) {
					value = function_eval->GetRawData();
				} else {
					errors = ErrorList::Concatenate(errors,
							function_eval_errors);
				}
			}
		}
	} else {
		// couldn't find member
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_MEMBER,
						record_type_specifier->GetLocation().begin.line,
						record_type_specifier->GetLocation().begin.column,
						*member_name, record_type_specifier->ToString()),
				errors);
	}

	return make_shared<Result>(value, errors);
}

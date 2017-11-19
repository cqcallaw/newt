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

#include <using_expression.h>
#include <statement_block.h>
#include <primitive_type.h>
#include <execution_context.h>
#include <record_type.h>
#include <function_type.h>
#include <maybe_type_specifier.h>
#include <function_declaration.h>
#include <foreach_statement.h>
#include <sum.h>
#include <function.h>
#include <record.h>

const_shared_ptr<std::string> UsingExpression::SETUP_NAME = make_shared<
		std::string>("setup");
const_shared_ptr<std::string> UsingExpression::TEARDOWN_NAME = make_shared<
		std::string>("teardown");

UsingExpression::UsingExpression(const yy::location location,
		const_shared_ptr<Expression> expression,
		const_shared_ptr<string> identifier,
		const_shared_ptr<TypeSpecifier> return_type_specifier,
		const_shared_ptr<StatementBlock> body) :
		Expression(location), m_expression(expression), m_identifier(
				identifier), m_return_type_specifier(return_type_specifier), m_body(
				body), m_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)) {
}

UsingExpression::~UsingExpression() {
}

TypedResult<TypeSpecifier> UsingExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return m_return_type_specifier;
}

const_shared_ptr<Result> UsingExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	// call setup
	// inject value of expression into block
	// execute block
	// call teardown

	auto errors = ErrorList::GetTerminator();
	plain_shared_ptr<void> value = nullptr;

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			context, RESOLVE);
	auto expression_type_specifier_errors =
			expression_type_specifier_result.GetErrors();

	if (ErrorList::IsTerminator(expression_type_specifier_errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		auto execution_context = ExecutionContext::GetRuntimeInstance(
				m_block_context, context);
		auto type_table = context->GetTypeTable();

		auto expression_type_result = expression_type_specifier->GetType(
				type_table, AliasResolution::RESOLVE);
		errors = expression_type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type = expression_type_result->GetData<
					TypeDefinition>();
			auto default_value = expression_type->GetDefaultValue(*type_table);

			auto eval = m_expression->Evaluate(context, closure);

			errors = eval->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto raw_value = eval->GetRawData();
				auto as_complex = dynamic_pointer_cast<
						const ComplexTypeSpecifier>(expression_type_specifier);

				auto record = static_pointer_cast<const Record>(raw_value);
				assert(record);

				// call setup
				auto setup_eval = ForeachStatement::EvaluateMemberFunction(
						record, as_complex, UsingExpression::SETUP_NAME,
						context, closure);

				auto setup_errors = setup_eval->GetErrors();
				if (ErrorList::IsTerminator(setup_errors)) {
					auto setup_eval_value = setup_eval->GetData<Sum>();
					auto setup_result_tag = setup_eval_value->GetTag();

					if (setup_result_tag == TypeTable::GetNilName()) {
						// no setup errors; clear to continue
						auto symbol = make_shared<const Symbol>(as_complex,
								record);
						auto set_result = execution_context->SetSymbol(
								*m_identifier, as_complex, record,
								context->GetTypeTable());
						assert(set_result == SET_SUCCESS);

						auto execution_result = m_body->Execute(
								execution_context);
						errors = ErrorList::Concatenate(errors,
								execution_result.GetErrors());
						auto return_value = execution_result.GetReturnValue();
						if (return_value != Symbol::GetDefaultSymbol()) {
							auto final_return_value_result =
									Function::GetFinalReturnValue(
											return_value->GetValue(),
											return_value->GetTypeSpecifier(),
											m_return_type_specifier,
											context->GetTypeTable());
							auto final_return_value_errors =
									final_return_value_result->GetErrors();

							if (ErrorList::IsTerminator(
									final_return_value_errors)) {
								value = final_return_value_result->GetRawData();
							} else {
								errors = ErrorList::Concatenate(errors,
										final_return_value_errors);
							}
						}

						// call tear down
						auto teardown_eval =
								ForeachStatement::EvaluateMemberFunction(record,
										as_complex,
										UsingExpression::TEARDOWN_NAME, context,
										closure);
						auto teardown_errors = teardown_eval->GetErrors();
						if (ErrorList::IsTerminator(teardown_errors)) {
							auto teardown_eval_value = teardown_eval->GetData<
									Sum>();
							auto tag = teardown_eval_value->GetTag();

							if (tag != TypeTable::GetNilName()) {
								// in-band teardown error
								// N.B. that this will replace the output of the using block
								auto teardown_symbol =
										record->GetDefinition()->GetSymbol(
												*UsingExpression::TEARDOWN_NAME);
								assert(
										teardown_symbol
												!= Symbol::GetDefaultSymbol());

								auto teardown_symbol_type =
										teardown_symbol->GetTypeSpecifier();
								auto teardown_type_as_function =
										dynamic_pointer_cast<
												const FunctionTypeSpecifier>(
												teardown_symbol_type);
								assert(teardown_type_as_function);

								auto teardown_return_type_specifier =
										teardown_type_as_function->GetReturnTypeSpecifier();
								auto teardown_return_as_maybe =
										dynamic_pointer_cast<
												const MaybeTypeSpecifier>(
												teardown_return_type_specifier);
								assert(teardown_return_as_maybe);

								auto teardown_return_base_type_specifier =
										teardown_return_as_maybe->GetBaseTypeSpecifier();

								auto final_return_value_result =
										Function::GetFinalReturnValue(
												teardown_eval_value->GetValue(),
												teardown_return_base_type_specifier,
												m_return_type_specifier,
												context->GetTypeTable());
								auto final_return_value_errors =
										final_return_value_result->GetErrors();

								if (ErrorList::IsTerminator(
										final_return_value_errors)) {
									value =
											final_return_value_result->GetRawData();
								} else {
									errors = ErrorList::Concatenate(errors,
											final_return_value_errors);
								}
							}
						} else {
							errors = ErrorList::Concatenate(errors,
									teardown_errors);
						}
					} else {
						// in-band setup error
						auto setup_symbol = record->GetDefinition()->GetSymbol(
								*UsingExpression::SETUP_NAME);
						assert(setup_symbol != Symbol::GetDefaultSymbol());

						auto setup_symbol_type =
								setup_symbol->GetTypeSpecifier();
						auto setup_type_as_function = dynamic_pointer_cast<
								const FunctionTypeSpecifier>(setup_symbol_type);
						assert(setup_type_as_function);

						auto setup_return_type_specifier =
								setup_type_as_function->GetReturnTypeSpecifier();
						auto setup_return_as_maybe = dynamic_pointer_cast<
								const MaybeTypeSpecifier>(
								setup_return_type_specifier);
						assert(setup_return_as_maybe);

						auto setup_return_base_type_specifier =
								setup_return_as_maybe->GetBaseTypeSpecifier();

						auto final_return_value_result =
								Function::GetFinalReturnValue(
										setup_eval_value->GetValue(),
										setup_return_base_type_specifier,
										m_return_type_specifier,
										context->GetTypeTable());
						auto final_return_value_errors =
								final_return_value_result->GetErrors();

						if (ErrorList::IsTerminator(
								final_return_value_errors)) {
							value = final_return_value_result->GetRawData();
						} else {
							errors = ErrorList::Concatenate(errors,
									final_return_value_errors);
						}
					}
				} else {
					errors = ErrorList::Concatenate(errors, setup_errors);
				}
			}
		}
	}

	return make_shared<Result>(value, errors);
}

const bool UsingExpression::IsConstant() const {
	return false;
}

const ErrorListRef UsingExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	// check that expression generates valid disposable
	// check that return type includes the disposable method return types
	// validate body, including return type
	auto errors = m_expression->Validate(execution_context);

	if (ErrorList::IsTerminator(errors)) {
		auto type_table = execution_context->GetTypeTable();
		auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
				execution_context, AliasResolution::RESOLVE);
		errors = expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();
			auto expression_type_result = expression_type_specifier->GetType(
					type_table, AliasResolution::RESOLVE);
			errors = expression_type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto expression_type = expression_type_result->GetData<
						TypeDefinition>();
				auto as_record = dynamic_pointer_cast<const RecordType>(
						expression_type);
				if (as_record) {
					auto complex_expression_type_specifier =
							dynamic_pointer_cast<const ComplexTypeSpecifier>(
									expression_type_specifier);

					errors = ErrorList::Concatenate(errors,
							ValidateMember(complex_expression_type_specifier,
									m_return_type_specifier,
									m_expression->GetLocation(), type_table,
									as_record, UsingExpression::SETUP_NAME));
					errors = ErrorList::Concatenate(errors,
							ValidateMember(complex_expression_type_specifier,
									m_return_type_specifier,
									m_expression->GetLocation(), type_table,
									as_record, UsingExpression::TEARDOWN_NAME));
					if (ErrorList::IsTerminator(errors)) {
						// no source expression errors encountered; insert symbols into context
						m_block_context->LinkToParent(execution_context);
						auto default_value = expression_type->GetDefaultValue(
								*type_table);

						auto default_symbol = expression_type->GetSymbol(
								type_table, expression_type_specifier,
								default_value);
						m_block_context->InsertSymbol(*m_identifier,
								default_symbol);

						auto preprocess_result = m_body->Preprocess(
								m_block_context, m_return_type_specifier);

						errors = preprocess_result.GetErrors();

						if (preprocess_result.GetReturnCoverage()
								!= PreprocessResult::ReturnCoverage::FULL) {
							errors = ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::MISSING_RETURN_COVERAGE,
											GetLocation().end.line,
											GetLocation().end.column), errors);
						}
					}
				} else {
					// error: not a record
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::STMT_SOURCE_MUST_BE_RECORD,
									m_expression->GetLocation().begin.line,
									m_expression->GetLocation().begin.column),
							errors);
				}
			}
		}
	}

	return errors;
}

const ErrorListRef UsingExpression::ValidateMember(
		const_shared_ptr<ComplexTypeSpecifier> expression_type_specifier,
		const_shared_ptr<TypeSpecifier> block_return_type_specifier,
		const yy::location expression_location,
		const_shared_ptr<TypeTable> type_table,
		const_shared_ptr<RecordType> source_type,
		const_shared_ptr<string> name) {
	auto errors = ErrorList::GetTerminator();

	auto setup_member = source_type->GetMember(*name);
	if (setup_member) {
		auto as_alias = dynamic_pointer_cast<const AliasDefinition>(
				setup_member);
		if (as_alias) {
			setup_member = as_alias->GetOrigin();
		}

		auto setup_as_function = dynamic_pointer_cast<const FunctionType>(
				setup_member);
		if (setup_as_function) {
			auto setup_return_type_specifier =
					setup_as_function->GetTypeSpecifier()->GetReturnTypeSpecifier();
			auto setup_as_maybe =
					dynamic_pointer_cast<const MaybeTypeSpecifier>(
							setup_return_type_specifier);
			if (setup_as_maybe) {
				auto base_type_specifier =
						setup_as_maybe->GetBaseTypeSpecifier();

				auto assignability = base_type_specifier->AnalyzeAssignmentTo(
						block_return_type_specifier, *type_table);
				if (assignability == AnalysisResult::AMBIGUOUS) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::USING_AMBIGUOUS_WIDENING_CONVERSION,
									expression_location.begin.line,
									expression_location.begin.column,
									base_type_specifier->ToString(),
									block_return_type_specifier->ToString(),
									*name), errors);
				} else if (assignability == INCOMPATIBLE) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::USING_ASSIGNMENT_TYPE_ERROR,
									expression_location.begin.line,
									expression_location.begin.column,
									base_type_specifier->ToString(),
									block_return_type_specifier->ToString(),
									*name), errors);
				}
			} else {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::RETURN_STMT_MUST_BE_MAYBE,
										setup_return_type_specifier->GetLocation().begin.line,
										setup_return_type_specifier->GetLocation().begin.column),
								errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::EXPRESSION_IS_NOT_A_FUNCTION,
							expression_location.begin.line,
							expression_location.begin.column,
							expression_type_specifier->ToString() + "."
									+ *name), errors);
		}
	} else {
		// no setup member
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_MEMBER,
						expression_type_specifier->GetLocation().begin.line,
						expression_type_specifier->GetLocation().begin.column,
						*name, expression_type_specifier->ToString()), errors);
	}

	return errors;
}

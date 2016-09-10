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

#include <declaration_statement.h>
#include <defaults.h>
#include <error.h>
#include <execution_context.h>
#include <function.h>
#include <function_declaration.h>
#include <function_type_specifier.h>
#include <invoke_expression.h>
#include <linked_list.h>
#include <location.hh>
#include <modifier.h>
#include <position.hh>
#include <primitive_type_specifier.h>
#include <record_type_specifier.h>
#include <result.h>
#include <sum_type.h>
#include <sum_type_specifier.h>
#include <symbol_context_list.h>
#include <specifiers/type_specifier.h>
#include <type_table.h>
#include <memory>
#include <sstream>
#include <string>

InvokeExpression::InvokeExpression(const yy::location position,
		const_shared_ptr<Expression> expression, ArgumentListRef argument_list,
		const yy::location argument_list_position) :
		Expression(position), m_expression(expression), m_argument_list(
				argument_list), m_argument_list_position(argument_list_position) {
}

InvokeExpression::~InvokeExpression() {
}

TResult<TypeSpecifier> InvokeExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	shared_ptr<const TypeSpecifier> result = PrimitiveTypeSpecifier::GetNone();

	auto expression_type_specifer_result = m_expression->GetTypeSpecifier(
			execution_context, RESOLVE);

	auto errors = expression_type_specifer_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifer_result.GetData();
		const_shared_ptr<FunctionTypeSpecifier> as_function =
				std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
						expression_type_specifier);
		if (as_function) {
			result = as_function->GetReturnTypeSpecifier();
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::NOT_A_FUNCTION,
							GetPosition().begin.line,
							GetPosition().begin.column), errors);
		}
	}

	return TResult<TypeSpecifier>(result, errors);
}

const_shared_ptr<Result> InvokeExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	plain_shared_ptr<void> value;

	auto type_specifier_result = m_expression->GetTypeSpecifier(context);
	auto errors = type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type_specifier = type_specifier_result.GetData();
		const_shared_ptr<FunctionTypeSpecifier> as_function =
				std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
						type_specifier);

		if (as_function) {
			const_shared_ptr<Result> expression_result = m_expression->Evaluate(
					context, closure);

			errors = expression_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto function = expression_result->GetData<Function>();

				const_shared_ptr<Result> eval_result = function->Evaluate(
						m_argument_list, context);

				errors = eval_result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					value = eval_result->GetRawData();
				}
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::NOT_A_FUNCTION,
							m_expression->GetPosition().begin.line,
							m_expression->GetPosition().begin.column), errors);
		}
	}

	return make_shared<Result>(value, errors);
}

const_shared_ptr<Result> InvokeExpression::ToString(
		const shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buf;
	const_shared_ptr<Result> expression_result = m_expression->ToString(
			execution_context);

	if (expression_result->GetErrors()) {
		buf << *(expression_result->GetData<string>());
		buf << "(";
		ErrorListRef errors =

		ErrorList::GetTerminator();
		ArgumentListRef argument = m_argument_list;
		while (!ArgumentList::IsTerminator(argument)) {
			const_shared_ptr<Result> argument_result =
					argument->GetData()->ToString(execution_context);
			errors = ErrorList::Concatenate(errors,
					argument_result->GetErrors());
			if (ErrorList::IsTerminator(errors)) {
				buf << *(argument_result->GetData<string>());
				if (!ArgumentList::IsTerminator(argument->GetNext())) {
					buf << ",";
				}
			}
		}
		buf << ")";
		return make_shared<Result>(
				const_shared_ptr<void>(new string(buf.str())), errors);
	} else {
		return expression_result;
	}
}

const ErrorListRef InvokeExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	auto type_specifier_result = m_expression->GetTypeSpecifier(
			execution_context);

	auto errors = type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		errors = m_expression->Validate(execution_context);
		if (ErrorList::IsTerminator(errors)) {
			auto type_specifier = type_specifier_result.GetData();
			const_shared_ptr<FunctionTypeSpecifier> as_function =
					std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
							type_specifier);

			if (as_function) {
				//generate a temporary context for validation
				auto new_parent = SymbolContextList::From(execution_context,
						execution_context->GetParent());
				shared_ptr<ExecutionContext> tmp_context = make_shared<
						ExecutionContext>(Modifier::Type::NONE, new_parent,
						execution_context->GetTypeTable(),
						execution_context->GetLifeTime());

				ArgumentListRef argument = m_argument_list;

				const_shared_ptr<FunctionDeclaration> as_function_declaration =
						std::dynamic_pointer_cast<const FunctionDeclaration>(
								type_specifier);

				if (as_function_declaration) {
					DeclarationListRef parameter =
							as_function_declaration->GetParameterList();

					while (!ArgumentList::IsTerminator(argument)) {
						const_shared_ptr<Expression> argument_expression =
								argument->GetData();
						if (!DeclarationList::IsTerminator(parameter)) {
							auto argument_expression_errors =
									argument_expression->Validate(
											execution_context);

							if (ErrorList::IsTerminator(
									argument_expression_errors)) {
								const_shared_ptr<DeclarationStatement> declaration =
										parameter->GetData();

								const_shared_ptr<TypeSpecifier> parameter_type_specifier =
										declaration->GetTypeSpecifier();
								auto argument_type_specifier_result =
										argument_expression->GetTypeSpecifier(
												execution_context);

								auto argument_type_specifier_errors =
										argument_type_specifier_result.GetErrors();

								if (ErrorList::IsTerminator(
										argument_type_specifier_errors)) {
									auto argument_type_specifier =
											argument_type_specifier_result.GetData();
									auto assignment_analysis =
											argument_type_specifier->AnalyzeAssignmentTo(
													parameter_type_specifier,
													execution_context->GetTypeTable());
									if (assignment_analysis == AMBIGUOUS) {
										errors =
												ErrorList::From(
														make_shared<Error>(
																Error::SEMANTIC,
																Error::FUNCTION_PARAMETER_TYPE_MISMATCH_AMBIGUOUS,
																argument_expression->GetPosition().begin.line,
																argument_expression->GetPosition().begin.column,
																argument_type_specifier->ToString(),
																parameter_type_specifier->ToString()),
														errors);
									} else if (assignment_analysis
											== INCOMPATIBLE) {
										errors =
												ErrorList::From(
														make_shared<Error>(
																Error::SEMANTIC,
																Error::FUNCTION_PARAMETER_TYPE_MISMATCH_INCOMPATIBLE,
																argument_expression->GetPosition().begin.line,
																argument_expression->GetPosition().begin.column,
																argument_type_specifier->ToString(),
																parameter_type_specifier->ToString()),
														errors);
									}
								} else {
									errors = ErrorList::Concatenate(errors,
											argument_type_specifier_errors);
								}
							} else {
								errors = ErrorList::Concatenate(errors,
										argument_expression_errors);
							}

							argument = argument->GetNext();
							parameter = parameter->GetNext();
						} else {
							//argument list is longer than parameter list
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::TOO_MANY_ARGUMENTS,
													argument_expression->GetPosition().begin.line,
													argument_expression->GetPosition().begin.column,
													as_function->ToString()),
											errors);
							break;
						}
					}

					//handle any remaining parameter declarations. if any parameter declarations don't have default values, generate an error
					while (!DeclarationList::IsTerminator(parameter)) {
						const_shared_ptr<DeclarationStatement> declaration =
								parameter->GetData();

						if (declaration->GetInitializerExpression()) {
							errors = ErrorList::Concatenate(
									declaration->Preprocess(tmp_context,
											tmp_context), errors);
						} else {
							errors = ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::NO_PARAMETER_DEFAULT,
											m_argument_list_position.end.line,
											m_argument_list_position.end.column,
											*declaration->GetName()), errors);
						}

						parameter = parameter->GetNext();
					}
				} else {
					TypeSpecifierListRef type_parameter_list =
							as_function->GetParameterTypeList();

					while (!ArgumentList::IsTerminator(argument)) {
						const_shared_ptr<Expression> argument_expression =
								argument->GetData();
						if (!TypeSpecifierList::IsTerminator(
								type_parameter_list)) {
							const_shared_ptr<TypeSpecifier> parameter_type =
									type_parameter_list->GetData();
							auto argument_type_specifier_result =
									argument_expression->GetTypeSpecifier(
											execution_context);

							auto argument_type_specifier_errors =
									argument_type_specifier_result.GetErrors();
							if (ErrorList::IsTerminator(
									argument_type_specifier_errors)) {
								auto argument_type_specifier =
										argument_type_specifier_result.GetData();
								auto conversion_analysis =
										argument_type_specifier->AnalyzeAssignmentTo(
												parameter_type,
												execution_context->GetTypeTable());
								if (conversion_analysis == AMBIGUOUS) {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::FUNCTION_PARAMETER_TYPE_MISMATCH_AMBIGUOUS,
															argument_expression->GetPosition().begin.line,
															argument_expression->GetPosition().begin.column,
															argument_type_specifier->ToString(),
															parameter_type->ToString()),
													errors);
								} else if (conversion_analysis
										== INCOMPATIBLE) {
									errors =
											ErrorList::From(
													make_shared<Error>(
															Error::SEMANTIC,
															Error::FUNCTION_PARAMETER_TYPE_MISMATCH_INCOMPATIBLE,
															argument_expression->GetPosition().begin.line,
															argument_expression->GetPosition().begin.column,
															argument_type_specifier->ToString(),
															parameter_type->ToString()),
													errors);
								}
							} else {
								errors = ErrorList::Concatenate(errors,
										argument_type_specifier_errors);
							}

							argument = argument->GetNext();
							type_parameter_list =
									type_parameter_list->GetNext();
						} else {
							//argument list is longer than parameter list
							errors =
									ErrorList::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::TOO_MANY_ARGUMENTS,
													argument_expression->GetPosition().begin.line,
													argument_expression->GetPosition().begin.column,
													as_function->ToString()),
											errors);
							break;
						}
					}
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::NOT_A_FUNCTION,
								m_expression->GetPosition().begin.line,
								m_expression->GetPosition().begin.column),
						errors);
			}
		}
	}

	return errors;
}

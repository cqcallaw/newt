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
#include <execution_context_list.h>
#include <function.h>
#include <function_declaration.h>
#include <function_type_specifier.h>
#include <variant_function_specifier.h>
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
#include <specifiers/type_specifier.h>
#include <type_table.h>
#include <memory>
#include <sstream>
#include <string>
#include <builtins.h>
#include <variable_expression.h>
#include <variable.h>
#include <open_expression.h>
#include <close_expression.h>
#include <get_byte_expression.h>
#include <put_byte_expression.h>

const std::map<string, InvokeExpression::invoke_expression_generator> InvokeExpression::BuiltinFunctionGeneratorMap =
		std::map<string, invoke_expression_generator> {
				{ "open",
						[](const yy::location location,
								const_shared_ptr<Expression> expression,
								const ArgumentListRef argument_list,
								const yy::location argument_list_location) -> const_shared_ptr<InvokeExpression> {
							return make_shared<OpenExpression>(location, expression,
									argument_list, argument_list_location);
						} },
				{ "close",
						[](const yy::location location,
								const_shared_ptr<Expression> expression,
								const ArgumentListRef argument_list,
								const yy::location argument_list_location) -> const_shared_ptr<InvokeExpression> {
							return make_shared<CloseExpression>(location, expression,
									argument_list, argument_list_location);
						} },
				{ "get",
						[](const yy::location location,
								const_shared_ptr<Expression> expression,
								const ArgumentListRef argument_list,
								const yy::location argument_list_location) -> const_shared_ptr<InvokeExpression> {
							return make_shared<GetByteExpression>(location, expression,
									argument_list, argument_list_location);
						} },
				{ "put",
						[](const yy::location location,
								const_shared_ptr<Expression> expression,
								const ArgumentListRef argument_list,
								const yy::location argument_list_location) -> const_shared_ptr<InvokeExpression> {
							return make_shared<PutByteExpression>(location, expression,
									argument_list, argument_list_location);
						} } };

InvokeExpression::InvokeExpression(const yy::location position,
		const_shared_ptr<Expression> expression,
		const ArgumentListRef argument_list,
		const yy::location argument_list_location,
		const TypeSpecifierListRef type_argument_list,
		const yy::location type_argument_list_location) :
		Expression(position), m_expression(expression), m_argument_list(
				argument_list), m_argument_list_location(
				argument_list_location), m_type_argument_list(
				type_argument_list), m_type_argument_list_location(
				type_argument_list_location) {
}

InvokeExpression::~InvokeExpression() {
}

TypedResult<TypeSpecifier> InvokeExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	shared_ptr<const TypeSpecifier> result = nullptr;

	auto expression_type_specifer_result = m_expression->GetTypeSpecifier(
			execution_context, RESOLVE);

	auto errors = expression_type_specifer_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifer_result.GetData();
		auto as_function =
				std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
						expression_type_specifier);
		if (as_function) {
			result = as_function->GetReturnTypeSpecifier();
		} else {
			auto as_variant_function = std::dynamic_pointer_cast<
					const VariantFunctionSpecifier>(expression_type_specifier);
			if (as_variant_function) {
				//look up overload
				auto variant_result = Function::GetVariant(m_argument_list,
						m_argument_list_location,
						as_variant_function->GetVariantList(),
						execution_context);

				auto variant_result_errors = variant_result.GetErrors();
				if (ErrorList::IsTerminator(variant_result_errors)) {
					auto variant = variant_result.GetData();
					auto return_type_specifer =
							variant->GetDeclaration()->GetReturnTypeSpecifier();
					result = return_type_specifer;
				} else {
					errors = variant_result_errors;
				}

			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::NOT_A_FUNCTION, GetLocation().begin),
						errors);
			}
		}
	}

	return TypedResult<TypeSpecifier>(result, errors);
}

const_shared_ptr<Result> InvokeExpression::Evaluate(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	plain_shared_ptr<void> value;

	auto type_specifier_result = m_expression->GetTypeSpecifier(context);
	auto errors = type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type_specifier = type_specifier_result.GetData();

		shared_ptr<const Function> function = nullptr;

		const_shared_ptr<FunctionTypeSpecifier> as_function =
				std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
						type_specifier);
		if (as_function) {
			auto expression_result = m_expression->Evaluate(context, closure);
			errors = expression_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				function = expression_result->GetData<Function>();
			}
		}

		auto as_variant_function = std::dynamic_pointer_cast<
				const VariantFunctionSpecifier>(type_specifier);
		if (as_variant_function) {
			auto expression_result = m_expression->Evaluate(context, closure);
			errors = expression_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				function = expression_result->GetData<Function>();
			}
		}

		if (function) {
			if (ErrorList::IsTerminator(errors)) {
				auto eval_result = function->Evaluate(m_argument_list,
						m_argument_list_location, context);

				errors = eval_result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					value = eval_result->GetRawData();
				}
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::NOT_A_FUNCTION,
							GetLocation().begin), errors);
		}
	}

	return make_shared<Result>(value, errors);
}

TypedResult<string> InvokeExpression::ToString(
		const shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buf;
	auto expression_result = m_expression->ToString(execution_context);

	if (ErrorList::IsTerminator(expression_result.GetErrors())) {
		buf << *(expression_result.GetData());
		buf << "(";
		auto errors = ErrorList::GetTerminator();
		ArgumentListRef argument = m_argument_list;
		while (!ArgumentList::IsTerminator(argument)) {
			auto argument_result = argument->GetData()->ToString(
					execution_context);
			errors = ErrorList::Concatenate(errors,
					argument_result.GetErrors());
			if (ErrorList::IsTerminator(errors)) {
				buf << *(argument_result.GetData());
				if (!ArgumentList::IsTerminator(argument->GetNext())) {
					buf << ",";
				}
			}
		}
		buf << ")";
		return TypedResult<string>(
				const_shared_ptr<string>(new string(buf.str())), errors);
	} else {
		return expression_result;
	}
}

const ErrorListRef InvokeExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) const {
	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			execution_context);

	auto errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto expression_type_specifier =
				expression_type_specifier_result.GetData();

		errors = m_expression->Validate(execution_context,
				ComplexType::DefaultTypeSpecifierMap);
		if (ErrorList::IsTerminator(errors)) {

			auto is_function = false;
			DeclarationListRef parameter_list = nullptr;

			auto as_function_declaration = std::dynamic_pointer_cast<
					const FunctionDeclaration>(expression_type_specifier);
			if (as_function_declaration) {
				is_function = true;
				parameter_list = as_function_declaration->GetParameterList();
			}

			auto as_variant_function = std::dynamic_pointer_cast<
					const VariantFunctionSpecifier>(expression_type_specifier);
			if (as_variant_function) {
				is_function = true;
				auto variant_result = Function::GetVariant(m_argument_list,
						m_argument_list_location,
						as_variant_function->GetVariantList(),
						execution_context);
				auto variant_result_errors = variant_result.GetErrors();
				if (ErrorList::IsTerminator(variant_result_errors)) {
					auto variant = variant_result.GetData();

					parameter_list =
							variant->GetDeclaration()->GetParameterList();
				} else {
					errors = ErrorList::Concatenate(errors,
							variant_result_errors);

					return errors;
				}
			}

			auto as_function_type_specifier = std::dynamic_pointer_cast<
					const FunctionTypeSpecifier>(expression_type_specifier);
			if (!is_function && as_function_type_specifier) {
				is_function = true;
				//generate placeholder parameter names
				auto temp_declaration_result =
						FunctionDeclaration::FromTypeSpecifier(
								*as_function_type_specifier,
								*execution_context->GetTypeTable());
				auto temp_declaration_errors =
						temp_declaration_result->GetErrors();
				if (ErrorList::IsTerminator(temp_declaration_errors)) {
					auto temp_declaration = temp_declaration_result->GetData<
							FunctionDeclaration>();
					parameter_list = temp_declaration->GetParameterList();
				} else {
					errors = ErrorList::Concatenate(errors,
							temp_declaration_errors);

					return errors;
				}
			}

			if (is_function) {
				//generate a temporary context for validation
				auto tmp_context = ExecutionContext::GetEmptyChild(
						execution_context, Modifier::Type::NONE, TEMPORARY);

				auto argument_subject = m_argument_list;
				auto parameter_subject = parameter_list;
				while (!ArgumentList::IsTerminator(argument_subject)) {
					auto argument_expression = argument_subject->GetData();
					if (!DeclarationList::IsTerminator(parameter_subject)) {
						auto argument_expression_errors =
								argument_expression->Validate(execution_context,
										ComplexType::DefaultTypeSpecifierMap);

						if (ErrorList::IsTerminator(
								argument_expression_errors)) {
							auto declaration = parameter_subject->GetData();
							auto parameter_type_specifier =
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
															argument_expression->GetLocation().begin,
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
															argument_expression->GetLocation().begin,
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

						argument_subject = argument_subject->GetNext();
						parameter_subject = parameter_subject->GetNext();
					} else {
						//argument list is longer than parameter list
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::TOO_MANY_ARGUMENTS,
												argument_expression->GetLocation().begin,
												expression_type_specifier->ToString()),
										errors);
						break;
					}
				}

				//handle any remaining parameter declarations; if any parameter declarations don't have default values, generate an error
				while (!DeclarationList::IsTerminator(parameter_subject)) {
					auto declaration = parameter_subject->GetData();

					if (declaration->GetInitializerExpression()) {
						errors =
								ErrorList::Concatenate(
										declaration->Preprocess(tmp_context,
												tmp_context,
												as_function_declaration->GetTypeParameterList()).GetErrors(),
										errors);
					} else {
						errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::NO_PARAMETER_DEFAULT,
										m_argument_list_location.end,
										*declaration->GetName()), errors);
					}

					parameter_subject = parameter_subject->GetNext();
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::NOT_A_FUNCTION,
								m_expression->GetLocation().begin), errors);
			}
		}
	}

	return errors;
}

const_shared_ptr<InvokeExpression> InvokeExpression::BuildInvokeExpression(
		const yy::location location, const_shared_ptr<Expression> expression,
		const ArgumentListRef argument_list,
		const yy::location argument_list_location,
		const TypeSpecifierListRef type_argument_list,
		const yy::location type_argument_list_location) {
	auto variable_expression = dynamic_pointer_cast<const VariableExpression>(
			expression);
	if (variable_expression) {
		auto variable_name = variable_expression->GetVariable()->GetName();
		auto builtin_entry = BuiltinFunctionGeneratorMap.find(*variable_name);
		if (builtin_entry != BuiltinFunctionGeneratorMap.end()) {
			auto generator = builtin_entry->second;
			return generator(location, expression, argument_list,
					argument_list_location);
		}
	}

	return make_shared<InvokeExpression>(location, expression, argument_list,
			argument_list_location, type_argument_list,
			type_argument_list_location);
}

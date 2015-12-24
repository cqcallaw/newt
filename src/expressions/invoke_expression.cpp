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

#include <sstream>
#include <invoke_expression.h>
#include <function_declaration.h>
#include <declaration_statement.h>
#include <function.h>
#include <execution_context.h>
#include <defaults.h>

InvokeExpression::InvokeExpression(const yy::location position,
		const_shared_ptr<Expression> expression, ArgumentList argument_list,
		const yy::location argument_list_position) :
		Expression(position), m_expression(expression), m_argument_list(
				argument_list), m_argument_list_position(argument_list_position) {
}

InvokeExpression::~InvokeExpression() {
}

const_shared_ptr<TypeSpecifier> InvokeExpression::GetType(
		const_shared_ptr<ExecutionContext> execution_context) const {
	const_shared_ptr<TypeSpecifier> type_specifier = m_expression->GetType(
			execution_context);
	const_shared_ptr<FunctionTypeSpecifier> as_function =
			std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
					type_specifier);

	if (as_function) {
		return as_function->GetReturnType();
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const Result* InvokeExpression::Evaluate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();
	plain_shared_ptr<void> value;

	const_shared_ptr<TypeSpecifier> type_specifier = m_expression->GetType(
			execution_context);
	const_shared_ptr<FunctionTypeSpecifier> as_function =
			std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
					type_specifier);

	if (as_function) {
		const Result* expression_result = m_expression->Evaluate(
				execution_context);

		errors = expression_result->GetErrors();
		if (ErrorListBase::IsTerminator(errors)) {
			auto function = static_pointer_cast<const Function>(
					expression_result->GetData());

			const Result* eval_result = function->Evaluate(m_argument_list,
					execution_context);

			errors = eval_result->GetErrors();
			if (ErrorListBase::IsTerminator(errors)) {
				value = eval_result->GetData();
			}

			delete eval_result;
		}
	} else {
		errors = ErrorListBase::From(
				make_shared<Error>(Error::SEMANTIC, Error::NOT_A_FUNCTION,
						m_expression->GetPosition().begin.line,
						m_expression->GetPosition().begin.column), errors);
	}

	return new Result(value, errors);
}

const Result* InvokeExpression::ToString(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buf;
	const Result* expression_result = m_expression->ToString(execution_context);

	if (expression_result->GetErrors()) {
		buf
				<< *(static_pointer_cast<const string>(
						expression_result->GetData()));
		buf << "(";
		ErrorList errors =

		ErrorListBase::GetTerminator();
		ArgumentList argument = m_argument_list;
		while (!ArgumentListBase::IsTerminator(argument)) {
			const Result* argument_result = argument->GetData()->ToString(
					execution_context);
			errors = ErrorListBase::Concatenate(errors,
					argument_result->GetErrors());
			if (ErrorListBase::IsTerminator(errors)) {
				buf
						<< *(static_pointer_cast<const string>(
								argument_result->GetData()));
				if (!ArgumentListBase::IsTerminator(argument->GetNext())) {
					buf << ",";
				}
			}
			delete argument_result;
		}
		buf << ")";
		return new Result(const_shared_ptr<void>(new string(buf.str())), errors);
	} else {
		return expression_result;
	}
}

const ErrorList InvokeExpression::Validate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	const_shared_ptr<TypeSpecifier> type_specifier = m_expression->GetType(
			execution_context);

	errors = m_expression->Validate(execution_context);

	if (ErrorListBase::IsTerminator(errors)) {
		const_shared_ptr<FunctionTypeSpecifier> as_function =
				std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
						type_specifier);

		if (as_function) {
			//generate a temporary context for validation
			auto parent = execution_context->GetSymbolContext()->GetParent();
			auto new_parent = SymbolContextListBase::From(
					execution_context->GetSymbolContext(), parent);
			auto tmp_map = make_shared<symbol_map>();
			volatile_shared_ptr<SymbolTable> tmp_table = std::make_shared<
					SymbolTable>(Modifier::Type::NONE, new_parent, tmp_map);
			const_shared_ptr<ExecutionContext> tmp_context =
					execution_context->WithSymbolContext(tmp_table);

			ArgumentList argument = m_argument_list;

			const_shared_ptr<FunctionDeclaration> as_function_declaration =
					std::dynamic_pointer_cast<const FunctionDeclaration>(
							type_specifier);

			if (as_function_declaration) {
				DeclarationList parameter =
						as_function_declaration->GetParameterList();

				while (!ArgumentListBase::IsTerminator(argument)) {
					const_shared_ptr<Expression> argument_expression =
							argument->GetData();
					if (!DeclarationListBase::IsTerminator(parameter)) {
						const_shared_ptr<DeclarationStatement> declaration =
								parameter->GetData();

						const_shared_ptr<TypeSpecifier> parameter_type =
								declaration->GetType();
						const_shared_ptr<TypeSpecifier> argument_type =
								argument_expression->GetType(execution_context);
						if (!argument_type->IsAssignableTo(parameter_type)) {
							errors =
									ErrorListBase::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::FUNCTION_PARAMETER_TYPE_MISMATCH,
													argument_expression->GetPosition().begin.line,
													argument_expression->GetPosition().begin.column,
													argument_type->ToString(),
													parameter_type->ToString()),
											errors);
						}

						argument = argument->GetNext();
						parameter = parameter->GetNext();
					} else {
						//argument list is longer than parameter list
						errors =
								ErrorListBase::From(
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
				while (!DeclarationListBase::IsTerminator(parameter)) {
					const_shared_ptr<DeclarationStatement> declaration =
							parameter->GetData();

					if (declaration->GetInitializerExpression() != nullptr) {
						errors = ErrorListBase::Concatenate(
								declaration->preprocess(tmp_context), errors);
					} else {
						errors = ErrorListBase::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::NO_PARAMETER_DEFAULT,
										m_argument_list_position.end.line,
										m_argument_list_position.end.column,
										*declaration->GetName()), errors);
					}

					parameter = parameter->GetNext();
				}
			} else {
				TypeSpecifierList type_parameter_list =
						as_function->GetParameterTypeList();

				while (!ArgumentListBase::IsTerminator(argument)) {
					const_shared_ptr<Expression> argument_expression =
							argument->GetData();
					if (!TypeSpecifierListBase::IsTerminator(
							type_parameter_list)) {
						const_shared_ptr<TypeSpecifier> parameter_type =
								type_parameter_list->GetData();
						const_shared_ptr<TypeSpecifier> argument_type =
								argument_expression->GetType(execution_context);

						if (!argument_type->IsAssignableTo(parameter_type)) {
							errors =
									ErrorListBase::From(
											make_shared<Error>(Error::SEMANTIC,
													Error::FUNCTION_PARAMETER_TYPE_MISMATCH,
													argument_expression->GetPosition().begin.line,
													argument_expression->GetPosition().begin.column,
													argument_type->ToString(),
													parameter_type->ToString()),
											errors);
						}

						argument = argument->GetNext();
						type_parameter_list = type_parameter_list->GetNext();
					} else {
						//argument list is longer than parameter list
						errors =
								ErrorListBase::From(
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
			errors = ErrorListBase::From(
					make_shared<Error>(Error::SEMANTIC, Error::NOT_A_FUNCTION,
							m_expression->GetPosition().begin.line,
							m_expression->GetPosition().begin.column), errors);
		}
	}

	return errors;
}

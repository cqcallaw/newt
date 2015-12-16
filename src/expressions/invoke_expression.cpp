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
#include <argument_list.h>
#include <function_declaration.h>
#include <declaration_statement.h>
#include <declaration_list.h>
#include <function.h>
#include <execution_context.h>

InvokeExpression::InvokeExpression(const YYLTYPE position,
		const Expression* expression, const ArgumentList* argument_list,
		const YYLTYPE argument_list_position) :
		Expression(position), m_expression(expression), m_argument_list(
				argument_list), m_argument_list_position(argument_list_position) {
}

InvokeExpression::~InvokeExpression() {
}

const TypeSpecifier* InvokeExpression::GetType(
		const ExecutionContext* execution_context) const {
	const TypeSpecifier* type_specifier = m_expression->GetType(
			execution_context);
	const FunctionTypeSpecifier* as_function =
			dynamic_cast<const FunctionTypeSpecifier*>(type_specifier);

	if (as_function) {
		return as_function->GetReturnType();
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const Result* InvokeExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();
	const void* value = nullptr;

	const TypeSpecifier* type_specifier = m_expression->GetType(
			execution_context);
	const FunctionTypeSpecifier* as_function =
			dynamic_cast<const FunctionTypeSpecifier*>(type_specifier);

	if (as_function) {
		const Result* expression_result = m_expression->Evaluate(
				execution_context);

		errors = expression_result->GetErrors();
		if (errors->IsTerminator()) {
			const Function* function =
					static_cast<const Function*>(expression_result->GetData());

			const Result* eval_result = function->Evaluate(m_argument_list,
					execution_context);

			errors = eval_result->GetErrors();
			if (errors->IsTerminator()) {
				value = eval_result->GetData();
			}

			delete eval_result;
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::NOT_A_FUNCTION,
						m_expression->GetPosition().first_line,
						m_expression->GetPosition().first_column));
	}

	return new Result(value, errors);
}

const Result* InvokeExpression::ToString(
		const ExecutionContext* execution_context) const {
	ostringstream buf;
	const Result* expression_result = m_expression->ToString(execution_context);

	if (expression_result->GetErrors()) {
		buf << *static_cast<const string*>(expression_result->GetData());
		buf << "(";
		const LinkedList<const Error*>* errors =

		LinkedList<const Error*>::GetTerminator();
		const LinkedList<const Expression*>* argument = m_argument_list;
		while (!argument->IsTerminator()) {
			const Result* argument_result = argument->GetData()->ToString(
					execution_context);
			errors = errors->Concatenate(argument_result->GetErrors(), true);
			if (errors->IsTerminator()) {
				buf << *static_cast<const string*>(argument_result->GetData());
				if (!argument->GetNext()->IsTerminator()) {
					buf << ",";
				}
			}
			delete argument_result;
		}
		buf << ")";
		return new Result(new string(buf.str()), errors);
	} else {
		return expression_result;
	}
}

const LinkedList<const Error*>* InvokeExpression::Validate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const TypeSpecifier* type_specifier = m_expression->GetType(
			execution_context);

	errors = m_expression->Validate(execution_context);

	if (errors->IsTerminator()) {
		const FunctionTypeSpecifier* as_function =
				dynamic_cast<const FunctionTypeSpecifier*>(type_specifier);

		if (as_function) {
			//generate a temporary context for validation
			auto parent = execution_context->GetSymbolContext()->GetParent();
			auto new_parent = parent->With(
					execution_context->GetSymbolContext());
			auto tmp_map = new map<const string, const Symbol*, comparator>();
			SymbolTable* tmp_table = new SymbolTable(Modifier::Type::NONE,
					new_parent, tmp_map);
			const ExecutionContext* tmp_context =
					execution_context->WithSymbolContext(tmp_table);

			const LinkedList<const Expression*>* argument = m_argument_list;

			const FunctionDeclaration* as_function_declaration =
					dynamic_cast<const FunctionDeclaration*>(type_specifier);

			if (as_function_declaration) {
				const LinkedList<const DeclarationStatement*>* parameter =
						as_function_declaration->GetParameterList();

				while (!argument->IsTerminator()) {
					const Expression* argument_expression = argument->GetData();
					if (!parameter->IsTerminator()) {
						const DeclarationStatement* declaration =
								parameter->GetData();

						const TypeSpecifier* parameter_type =
								declaration->GetType();
						const TypeSpecifier* argument_type =
								argument_expression->GetType(execution_context);
						if (!argument_type->IsAssignableTo(parameter_type)) {
							errors =
									errors->With(
											new Error(Error::SEMANTIC,
													Error::FUNCTION_PARAMETER_TYPE_MISMATCH,
													argument_expression->GetPosition().first_line,
													argument_expression->GetPosition().first_column,
													argument_type->ToString(),
													parameter_type->ToString()));
						}

						argument = argument->GetNext();
						parameter = parameter->GetNext();
					} else {
						//argument list is longer than parameter list
						errors =
								errors->With(
										new Error(Error::SEMANTIC,
												Error::TOO_MANY_ARGUMENTS,
												argument_expression->GetPosition().first_line,
												argument_expression->GetPosition().first_column,
												as_function->ToString()));
						break;
					}
				}

				//handle any remaining parameter declarations. if any parameter declarations don't have default values, generate an error
				while (!parameter->IsTerminator()) {
					const DeclarationStatement* declaration =
							parameter->GetData();

					if (declaration->GetInitializerExpression() != nullptr) {
						errors = errors->Concatenate(
								declaration->preprocess(tmp_context), true);
					} else {
						errors = errors->With(
								new Error(Error::SEMANTIC,
										Error::NO_PARAMETER_DEFAULT,
										m_argument_list_position.last_line,
										m_argument_list_position.last_column,
										*declaration->GetName()));
					}

					parameter = parameter->GetNext();
				}
			} else {
				const LinkedList<const TypeSpecifier*>* type_parameter_list =
						as_function->GetParameterTypeList();

				while (!argument->IsTerminator()) {
					const Expression* argument_expression = argument->GetData();
					if (!type_parameter_list->IsTerminator()) {
						const TypeSpecifier* parameter_type =
								type_parameter_list->GetData();
						const TypeSpecifier* argument_type =
								argument_expression->GetType(execution_context);

						if (!argument_type->IsAssignableTo(parameter_type)) {
							errors =
									errors->With(
											new Error(Error::SEMANTIC,
													Error::FUNCTION_PARAMETER_TYPE_MISMATCH,
													argument_expression->GetPosition().first_line,
													argument_expression->GetPosition().first_column,
													argument_type->ToString(),
													parameter_type->ToString()));
						}

						argument = argument->GetNext();
						type_parameter_list = type_parameter_list->GetNext();
					} else {
						//argument list is longer than parameter list
						errors =
								errors->With(
										new Error(Error::SEMANTIC,
												Error::TOO_MANY_ARGUMENTS,
												argument_expression->GetPosition().first_line,
												argument_expression->GetPosition().first_column,
												as_function->ToString()));
						break;
					}
				}
			}

			delete tmp_context;
			delete tmp_table;
			delete tmp_map;
			delete new_parent;
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::NOT_A_FUNCTION,
							m_expression->GetPosition().first_line,
							m_expression->GetPosition().first_column));
		}
	}

	return errors;
}

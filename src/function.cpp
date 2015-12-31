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

#include <function.h>
#include <expression.h>
#include <declaration_statement.h>
#include <execution_context.h>
#include <function_declaration.h>
#include <error.h>
#include <statement_block.h>
#include <constant_expression.h>
#include <defaults.h>

Function::Function(const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> body,
		const_shared_ptr<ExecutionContext> closure) :
		m_declaration(declaration), m_body(body), m_closure(closure) {
}

Function::~Function() {
}

const_shared_ptr<Result> Function::Evaluate(ArgumentList argument_list,
		const_shared_ptr<ExecutionContext> invocation_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	auto invocation_symbol_context = invocation_context->GetSymbolContext();
	auto invocation_context_parent = invocation_symbol_context->GetParent();
	auto parent_context = SymbolContextListBase::From(invocation_symbol_context,
			invocation_context_parent);
	auto table = make_shared<SymbolTable>(parent_context);
	shared_ptr<ExecutionContext> function_execution_context = shared_ptr<
			ExecutionContext>(
			new ExecutionContext(table, m_closure->GetTypeTable()));

	//populate evaluation context with results of argument evaluation
	ArgumentList argument = argument_list;
	DeclarationList parameter = m_declaration->GetParameterList();
	while (!ArgumentListBase::IsTerminator(argument)) {
		const_shared_ptr<Expression> argument_expression = argument->GetData();
		if (!DeclarationListBase::IsTerminator(parameter)) {
			const_shared_ptr<DeclarationStatement> declaration =
					parameter->GetData();

			const_shared_ptr<Result> argument_evaluation =
					ConstantExpression::GetConstantExpression(
							argument_expression, invocation_context);
			auto evaluation_errors = argument_evaluation->GetErrors();
			if (ErrorListBase::IsTerminator(evaluation_errors)) {
				//generate a declaration statement for the function execution context.
				//it's tempting to just stuff a value into the symbol table,
				//but this simplistic approach ignores widening conversions
				auto evaluated_expression =
						static_pointer_cast<const Expression>(
								argument_evaluation->GetData());
				const DeclarationStatement* argument_declaration =
						declaration->WithInitializerExpression(
								evaluated_expression);

				auto preprocessing_errors = ErrorListBase::Concatenate(errors,
						declaration->preprocess(function_execution_context));
				if (ErrorListBase::IsTerminator(preprocessing_errors)) {
					errors = ErrorListBase::Concatenate(errors,
							argument_declaration->execute(
									function_execution_context));
				} else {
					errors = ErrorListBase::Concatenate(errors,
							preprocessing_errors);
				}

				delete argument_declaration;
			} else {
				errors = ErrorListBase::Concatenate(errors, evaluation_errors);
			}

			argument = argument->GetNext();
			parameter = parameter->GetNext();
		} else {
			//argument list is longer than parameter list
			errors = ErrorListBase::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::TOO_MANY_ARGUMENTS,
							argument_expression->GetPosition().begin.line,
							argument_expression->GetPosition().begin.column,
							m_declaration->ToString()), errors);
			break;
		}
	}

	//handle any remaining parameter declarations. if any parameter declarations don't have default values, generate an error
	while (!DeclarationListBase::IsTerminator(parameter)) {
		const_shared_ptr<DeclarationStatement> declaration =
				parameter->GetData();

		if (declaration->GetInitializerExpression() != nullptr) {
			errors = ErrorListBase::Concatenate(errors,
					declaration->preprocess(function_execution_context));
			errors = ErrorListBase::Concatenate(errors,
					declaration->execute(function_execution_context));
			parameter = parameter->GetNext();
		} else {
			errors = ErrorListBase::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NO_PARAMETER_DEFAULT,
							declaration->GetPosition().begin.line,
							declaration->GetPosition().begin.column,
							*declaration->GetName()), errors);
			break;
		}
	}

	//juggle the references so the evaluation context is a child of the closure context
	auto closure_symbol_context = m_closure->GetSymbolContext();
	auto closure_symbol_context_parent = closure_symbol_context->GetParent();
	parent_context = SymbolContextListBase::From(closure_symbol_context,
			closure_symbol_context_parent);
	auto final_symbol_context = table->WithParent(parent_context);

	//TODO: determine if it is necessary to merge type tables

	if (ErrorListBase::IsTerminator(errors)) {
		shared_ptr<ExecutionContext> child_context =
				shared_ptr<ExecutionContext>(
						new ExecutionContext(final_symbol_context,
								m_closure->GetTypeTable()));
		errors = ErrorListBase::Concatenate(errors,
				m_body->execute(child_context));

		return make_shared<Result>(child_context->GetReturnValue(), errors);
	} else {
		return make_shared<Result>(nullptr, errors);
	}
}

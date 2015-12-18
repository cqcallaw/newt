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

Function::Function(const FunctionDeclaration* declaration,
		const StatementBlock* body, const ExecutionContext* closure) :
		m_declaration(declaration), m_body(body), m_closure(closure) {
}

Function::~Function() {
}

const Result* Function::Evaluate(const ArgumentList* argument_list,
		const ExecutionContext* invocation_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	auto invocation_symbol_context = invocation_context->GetSymbolContext();
	auto invocation_context_parent = invocation_symbol_context->GetParent();
	auto parent_context = invocation_context_parent->With(
			invocation_symbol_context);
	SymbolTable* table = new SymbolTable(parent_context);
	ExecutionContext* function_execution_context = new ExecutionContext(table,
			m_closure->GetTypeTable());

	//populate evaluation context with results of argument evaluation
	const LinkedList<const Expression*>* argument = argument_list;
	const LinkedList<const DeclarationStatement*>* parameter =
			m_declaration->GetParameterList();
	while (!argument->IsTerminator()) {
		const Expression* argument_expression = argument->GetData();
		if (!parameter->IsTerminator()) {
			const DeclarationStatement* declaration = parameter->GetData();

			const Result* argument_evaluation =
					ConstantExpression::GetConstantExpression(
							argument_expression, invocation_context);
			auto evaluation_errors = argument_evaluation->GetErrors();
			if (evaluation_errors->IsTerminator()) {
				//generate a declaration statement for the function execution context.
				//it's tempting to just stuff a value into the symbol table,
				//but this simplistic approach ignores widening conversions
				const Expression* evaluated_expression =
						static_cast<const Expression*>(argument_evaluation->GetData());
				const DeclarationStatement* argument_declaration =
						declaration->WithInitializerExpression(
								evaluated_expression);

				auto preprocessing_errors = errors->Concatenate(
						declaration->preprocess(function_execution_context),
						true);
				if (preprocessing_errors->IsTerminator()) {
					errors = errors->Concatenate(
							argument_declaration->execute(
									function_execution_context), true);
				} else {
					errors = errors->Concatenate(preprocessing_errors, true);
				}

				delete argument_declaration;
				delete evaluated_expression;
			} else {
				errors = errors->Concatenate(evaluation_errors, true);
			}

			delete argument_evaluation;

			argument = argument->GetNext();
			parameter = parameter->GetNext();
		} else {
			//argument list is longer than parameter list
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::TOO_MANY_ARGUMENTS,
							argument_expression->GetPosition().first_line,
							argument_expression->GetPosition().first_column,
							m_declaration->ToString()));
			break;
		}
	}

	//handle any remaining parameter declarations. if any parameter declarations don't have default values, generate an error
	while (!parameter->IsTerminator()) {
		const DeclarationStatement* declaration = parameter->GetData();

		if (declaration->GetInitializerExpression() != nullptr) {
			errors = errors->Concatenate(
					declaration->preprocess(function_execution_context), true);
			errors = errors->Concatenate(
					declaration->execute(function_execution_context), true);
			parameter = parameter->GetNext();
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::NO_PARAMETER_DEFAULT,
							declaration->GetPosition().first_line,
							declaration->GetPosition().first_column,
							*declaration->GetName()));
			break;
		}
	}

	//juggle the references so the evaluation context is a child of the closure context
	auto closure_symbol_context = m_closure->GetSymbolContext();
	auto closure_symbol_context_parent = closure_symbol_context->GetParent();
	parent_context = closure_symbol_context_parent->With(
			closure_symbol_context);
	auto final_symbol_context = table->WithParent(parent_context);

	//TODO: determine if it is necessary to merge type tables

	if (errors->IsTerminator()) {
		ExecutionContext* child_context = new ExecutionContext(
				final_symbol_context, m_closure->GetTypeTable());
		errors = errors->Concatenate(m_body->execute(child_context), true);

		return new Result(child_context->GetReturnValue(), errors);
	} else {
		return new Result(nullptr, errors);
	}
}

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
#include <argument_list.h>
#include <declaration_list.h>
#include <declaration_statement.h>
#include <execution_context.h>
#include <function_declaration.h>
#include <error.h>
#include <statement_block.h>

Function::Function(const FunctionDeclaration* declaration,
		const StatementBlock* body, const ExecutionContext* closure) :
		m_declaration(declaration), m_body(body), m_closure(closure) {
}

Function::~Function() {
}

const Result* Function::Evaluate(const ArgumentList* argument_list,
		const ExecutionContext* invocation_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	//N.B. the argument evaluation is almost identical to the validation of an InvokeExpression,
	//but execution is performed in addition to preprocessing.
	auto invocation_symbol_context = invocation_context->GetSymbolContext();
	auto invocation_context_parent = invocation_symbol_context->GetParent();
	auto parent_context = invocation_context_parent->With(
			invocation_symbol_context);
	SymbolTable* table = new SymbolTable(parent_context);
	ExecutionContext* execution_context = new ExecutionContext(table,
			m_closure->GetTypeTable());

	//populate evaluation context with results of argument evaluation
	const LinkedList<const Expression*>* argument = argument_list;
	const LinkedList<const DeclarationStatement*>* parameter =
			m_declaration->GetParameterList();

	while (!argument->IsTerminator()) {
		const Expression* argument_expression = argument->GetData();
		if (!parameter->IsTerminator()) {
			const DeclarationStatement* declaration = parameter->GetData();
			const DeclarationStatement* argument_declaration =
					declaration->WithInitializerExpression(argument_expression);

			auto preprocessing_errors = errors->Concatenate(
					argument_declaration->preprocess(execution_context), true);
			if (preprocessing_errors->IsTerminator()) {
				errors = errors->Concatenate(
						argument_declaration->execute(execution_context), true);
			} else {
				errors = errors->Concatenate(preprocessing_errors, true);
			}

			delete argument_declaration;

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
					declaration->preprocess(execution_context), true);
			errors = errors->Concatenate(
					declaration->execute(execution_context), true);
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

	//TODO: determine if it is necessary to type tables

	ExecutionContext* child_context = new ExecutionContext(final_symbol_context,
			m_closure->GetTypeTable());

	errors = errors->Concatenate(m_body->execute(child_context), true);

	return new Result(child_context->GetReturnValue(), errors);
}

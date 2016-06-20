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

#include <function_declaration.h>
#include <function_expression.h>
#include <function.h>
#include <statement_block.h>
#include <execution_context.h>
#include <function_type_specifier.h>
#include <symbol_table.h>
#include <complex_type.h>
#include <declaration_statement.h>
#include <inferred_declaration_statement.h>

FunctionExpression::FunctionExpression(const yy::location position,
		const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> body) :
		Expression(position), m_declaration(declaration), m_body(body) {
}

FunctionExpression::~FunctionExpression() {
}

const_shared_ptr<TypeSpecifier> FunctionExpression::GetTypeSpecifier(
		const shared_ptr<ExecutionContext> execution_context,
		AliasResolution resolution) const {
	return m_declaration;
}

const_shared_ptr<Result> FunctionExpression::Evaluate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	shared_ptr<const Function> function;
	if (execution_context->GetLifeTime() == PERSISTENT) {
		function = make_shared<Function>(m_declaration, m_body,
				weak_ptr<ExecutionContext>(execution_context));
	} else {
		function = make_shared<Function>(m_declaration, m_body,
				execution_context);
	}

	return make_shared<Result>(function, errors);
}

const bool FunctionExpression::IsConstant() const {
	return true;
}

const ErrorListRef FunctionExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	//generate a temporary context for validation
	auto new_parent = SymbolContextList::From(execution_context,
			execution_context->GetParent());
	shared_ptr<ExecutionContext> tmp_context = make_shared<ExecutionContext>(
			Modifier::Type::NONE, new_parent, execution_context->GetTypeTable(),
			execution_context->GetLifeTime());

	DeclarationListRef declaration = m_declaration->GetParameterList();
	while (!DeclarationList::IsTerminator(declaration)) {
		auto declaration_statement = declaration->GetData();

		ErrorListRef parameter_errors = ErrorList::GetTerminator();
		//validate the constancy of parameter declarations of an inferred type
		const_shared_ptr<InferredDeclarationStatement> as_inferred =
				dynamic_pointer_cast<const InferredDeclarationStatement>(
						declaration_statement);
		if (as_inferred) {
			auto init_expression = as_inferred->GetInitializerExpression();
			if (!init_expression->IsConstant()) {
				parameter_errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::FUNCTION_PARAMETER_DEFAULT_MUST_BE_CONSTANT,
										init_expression->GetPosition().begin.line,
										init_expression->GetPosition().begin.column),
								parameter_errors);
			}
		}

		if (ErrorList::IsTerminator(parameter_errors)) {
			auto preprocess_errors = declaration_statement->Preprocess(
					tmp_context);

			if (ErrorList::IsTerminator(preprocess_errors)) {
				auto execution_errors = declaration_statement->Execute(
						tmp_context);
				if (!ErrorList::IsTerminator(execution_errors)) {
					parameter_errors = ErrorList::Concatenate(parameter_errors,
							execution_errors);
				}
			} else {
				parameter_errors = ErrorList::Concatenate(parameter_errors,
						preprocess_errors);
			}
		} else {
			errors = ErrorList::Concatenate(errors, parameter_errors);
		}

		declaration = declaration->GetNext();
	}

	if (ErrorList::IsTerminator(errors)) {
		errors = ErrorList::Concatenate(errors,
				m_body->Preprocess(tmp_context));

		errors = ErrorList::Concatenate(errors,
				m_body->GetReturnStatementErrors(
						m_declaration->GetReturnTypeSpecifier(), tmp_context));
	}

	return errors;
}


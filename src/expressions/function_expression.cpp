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
#include <declaration_statement.h>
#include <symbol_table.h>

FunctionExpression::FunctionExpression(const yy::location position,
		const_shared_ptr<FunctionDeclaration> declaration,
		const_shared_ptr<StatementBlock> body) :
		Expression(position), m_declaration(declaration), m_body(body) {
}

FunctionExpression::~FunctionExpression() {
}

const_shared_ptr<TypeSpecifier> FunctionExpression::GetType(
		const_shared_ptr<ExecutionContext> execution_context) const {
	return m_declaration;
}

const_shared_ptr<Result> FunctionExpression::Evaluate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	return make_shared<Result>(
			make_shared<Function>(m_declaration, m_body, execution_context),
			errors);
}

const bool FunctionExpression::IsConstant() const {
	return true;
}

const ErrorListRef FunctionExpression::Validate(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	//generate a temporary context for validation
	auto parent = execution_context->GetSymbolContext()->GetParent();
	auto new_parent = SymbolContextList::From(
			execution_context->GetSymbolContext(), parent);
	auto tmp_map = make_shared<symbol_map>();
	volatile_shared_ptr<SymbolTable> tmp_table = make_shared<SymbolTable>(
			Modifier::Type::NONE, new_parent, tmp_map);
	shared_ptr<ExecutionContext> tmp_context =
			execution_context->WithSymbolContext(tmp_table);

	DeclarationListRef declaration = m_declaration->GetParameterList();
	while (!DeclarationList::IsTerminator(declaration)) {
		auto declaration_statement = declaration->GetData();
		auto preprocessing_errors = ErrorList::Concatenate(errors,
				declaration_statement->preprocess(tmp_context));

		if (ErrorList::IsTerminator(preprocessing_errors)) {
			errors = ErrorList::Concatenate(errors,
					declaration_statement->execute(tmp_context));
		} else {
			errors = ErrorList::Concatenate(errors, preprocessing_errors);
		}

		declaration = declaration->GetNext();
	}

	errors = m_body->preprocess(tmp_context);

	AnalysisResult returns = m_body->Returns(m_declaration->GetReturnType(),
			tmp_context);
	if (returns == AnalysisResult::NO) {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::FUNCTION_RETURN_MISMATCH,
						GetPosition().begin.line, GetPosition().begin.column),
				errors);
	}

	return errors;
}


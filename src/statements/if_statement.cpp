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

#include "if_statement.h"

#include <expression.h>
#include <statement_block.h>
#include <defaults.h>
#include <assert.h>
#include <type.h>
#include <error.h>
#include <execution_context.h>

IfStatement::IfStatement(const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> block) :
		IfStatement(expression, block, nullptr) {
}

IfStatement::IfStatement(const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> block,
		const_shared_ptr<StatementBlock> else_block) :
		m_expression(expression), m_block(block), m_else_block(else_block), m_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)), m_else_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)) {
}

IfStatement::~IfStatement() {
}

const ErrorListRef IfStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (m_expression) {
		auto expression_analysis = m_expression->GetTypeSpecifier(
				execution_context)->AnalyzeAssignmentTo(
				PrimitiveTypeSpecifier::GetInt(),
				execution_context->GetTypeTable());
		if (expression_analysis == EQUIVALENT
				|| expression_analysis == UNAMBIGUOUS) {
			SymbolContextListRef new_parent = SymbolContextList::From(
					execution_context, execution_context->GetParent());
			const shared_ptr<ExecutionContext> new_execution_context =
					execution_context->WithContents(m_block_context)->WithParent(
							new_parent);

			errors = m_block->preprocess(execution_context);

			if (m_else_block) {
				//pre-process else block
				new_parent = SymbolContextList::From(execution_context,
						execution_context->GetParent());
				const shared_ptr<ExecutionContext> new_execution_context =
						execution_context->WithContents(m_else_block_context)->WithParent(
								new_parent);

				errors = m_else_block->preprocess(execution_context);
			}

		} else {
			yy::location position = m_expression->GetPosition();
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::INVALID_TYPE_FOR_IF_STMT_EXPRESSION,
							position.begin.line, position.begin.column),
					errors);
		}
	} else {
		assert(false);
	}

	return errors;
}

const ErrorListRef IfStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<Result> evaluation = m_expression->Evaluate(
			execution_context);
	//NOTE: we are relying on our preprocessing passing to guarantee that the previous evaluation returned no errors
	bool test = *(evaluation->GetData<bool>());

	if (test) {
		SymbolContextListRef new_parent = SymbolContextList::From(
				execution_context, execution_context->GetParent());
		shared_ptr<ExecutionContext> new_execution_context =
				m_block_context->WithParent(new_parent);

		errors = m_block->execute(execution_context);
	} else if (m_else_block) {
		SymbolContextListRef new_parent = SymbolContextList::From(
				execution_context, execution_context->GetParent());
		shared_ptr<ExecutionContext> new_execution_context =
				m_else_block_context->WithParent(new_parent);

		errors = m_else_block->execute(execution_context);
	}

	return errors;
}

const ErrorListRef IfStatement::GetReturnStatementErrors(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const shared_ptr<ExecutionContext> execution_context) const {
	auto errors = m_block->GetReturnStatementErrors(type_specifier,
			execution_context);
	if (m_else_block) {
		errors = ErrorList::Concatenate(errors,
				m_else_block->GetReturnStatementErrors(type_specifier,
						execution_context));
	}
	return errors;
}

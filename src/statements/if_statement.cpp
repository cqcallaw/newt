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
#include <symbol_table.h>
#include <execution_context.h>

IfStatement::IfStatement(const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> block) :
		IfStatement(expression, block, nullptr) {
}

IfStatement::IfStatement(const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> block,
		const_shared_ptr<StatementBlock> else_block) :
		m_expression(expression), m_block(block), m_else_block(else_block), m_block_table(
				new SymbolTable()), m_else_block_table(new SymbolTable()) {
}

IfStatement::~IfStatement() {
}

const ErrorList IfStatement::preprocess(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	if (m_expression != nullptr) {
		if (m_expression->GetType(execution_context)->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {

			volatile_shared_ptr<SymbolContext> symbol_context =
					execution_context->GetSymbolContext();
			SymbolContextList new_parent = SymbolContextListBase::From(
					symbol_context, symbol_context->GetParent());
			volatile_shared_ptr<SymbolTable> tmp_table =
					make_shared<SymbolTable>(
							SymbolTable(m_block_table->GetModifiers(),
									new_parent, m_block_table->GetTable()));
			shared_ptr<ExecutionContext> new_execution_context =
					execution_context->WithSymbolContext(tmp_table);

			errors = m_block->preprocess(execution_context);

			if (m_else_block != nullptr) {
				//pre-process else block
				new_parent = SymbolContextListBase::From(symbol_context,
						symbol_context->GetParent());
				tmp_table = make_shared<SymbolTable>(
						m_else_block_table->GetModifiers(), new_parent,
						m_else_block_table->GetTable());
				new_execution_context = execution_context->WithSymbolContext(
						tmp_table);

				errors = m_else_block->preprocess(execution_context);
			}

		} else {
			yy::location position = m_expression->GetPosition();
			errors = ErrorListBase::From(
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

const ErrorList IfStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	const_shared_ptr<Result> evaluation = m_expression->Evaluate(execution_context);
	//NOTE: we are relying on our preprocessing passing to guarantee that the previous evaluation returned no errors
	bool test = *(static_pointer_cast<const bool>(evaluation->GetData()));
	
	if (test) {
		volatile_shared_ptr<SymbolContext> symbol_context =
				execution_context->GetSymbolContext();
		SymbolContextList new_parent = SymbolContextListBase::From(
				symbol_context, symbol_context->GetParent());
		volatile_shared_ptr<SymbolTable> tmp_table = make_shared<SymbolTable>(
				m_block_table->GetModifiers(), new_parent,
				m_block_table->GetTable());
		shared_ptr<ExecutionContext> new_execution_context =
				execution_context->WithSymbolContext(tmp_table);

		errors = m_block->execute(execution_context);
	} else if (m_else_block != nullptr) {
		volatile_shared_ptr<SymbolContext> symbol_context =
				execution_context->GetSymbolContext();
		SymbolContextList new_parent = SymbolContextListBase::From(
				symbol_context, symbol_context->GetParent());
		volatile_shared_ptr<SymbolTable> tmp_table = make_shared<SymbolTable>(
				m_else_block_table->GetModifiers(), new_parent,
				m_else_block_table->GetTable());
		shared_ptr<ExecutionContext> new_execution_context =
				execution_context->WithSymbolContext(tmp_table);

		errors = m_else_block->execute(execution_context);
	}

	return errors;
}

const AnalysisResult IfStatement::Returns(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<ExecutionContext> execution_context) const {
	AnalysisResult result = m_block->Returns(type_specifier, execution_context);

	if (m_else_block) {
		AnalysisResult else_result = m_else_block->Returns(type_specifier,
				execution_context);

		if (result == AnalysisResult::YES) {
			result = else_result;
		}
	}

	return result;
}

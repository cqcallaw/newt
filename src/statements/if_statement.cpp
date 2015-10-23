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

IfStatement::IfStatement(const Expression* expression,
		const StatementBlock* block) :
		IfStatement(expression, block, nullptr) {
}

IfStatement::IfStatement(const Expression* expression,
		const StatementBlock* block, const StatementBlock* else_block) :
		m_expression(expression), m_block(block), m_else_block(else_block), m_block_table(
				new SymbolTable()), m_else_block_table(new SymbolTable()) {
}

IfStatement::~IfStatement() {
}

const LinkedList<const Error*>* IfStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	if (m_expression != nullptr) {
		if (m_expression->GetType(execution_context)->IsAssignableTo(
				PrimitiveTypeSpecifier::GetInt())) {

			SymbolContext* symbol_context =
					execution_context->GetSymbolContext();
			auto new_parent = symbol_context->GetParent()->With(symbol_context);
			SymbolTable* tmp_table = new SymbolTable(
					m_block_table->GetModifiers(), new_parent,
					m_block_table->GetTable());
			const ExecutionContext* new_execution_context =
					execution_context->WithSymbolContext(tmp_table);

			errors = m_block->preprocess(execution_context);

			delete new_execution_context;
			delete tmp_table;
			delete new_parent;

			if (m_else_block != nullptr) {
				//pre-process else block
				new_parent = symbol_context->GetParent()->With(symbol_context);
				tmp_table = new SymbolTable(m_else_block_table->GetModifiers(),
						new_parent, m_else_block_table->GetTable());
				new_execution_context = execution_context->WithSymbolContext(
						tmp_table);

				errors = m_else_block->preprocess(execution_context);

				delete new_execution_context;
				delete tmp_table;
				delete new_parent;
			}

		} else {
			YYLTYPE position = m_expression->GetPosition();
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_TYPE_FOR_IF_STMT_EXPRESSION,
							position.first_line, position.first_column));
		}
	} else {
		assert(false);
	}

	return errors;
}

const LinkedList<const Error*>* IfStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Result* evaluation = m_expression->Evaluate(execution_context);
	//NOTE: we are relying on our preprocessing passing to guarantee that the previous evaluation returned no errors
	bool test = *((bool*) evaluation->GetData());
	delete (evaluation);
	if (test) {
		SymbolContext* symbol_context = execution_context->GetSymbolContext();
		const auto new_parent = symbol_context->GetParent()->With(
				symbol_context);
		SymbolTable* tmp_table = new SymbolTable(m_block_table->GetModifiers(),
				new_parent, m_block_table->GetTable());
		const ExecutionContext* new_execution_context =
				execution_context->WithSymbolContext(tmp_table);

		errors = m_block->execute(execution_context);

		delete new_execution_context;
		delete tmp_table;
		delete new_parent;
	} else if (m_else_block != nullptr) {
		SymbolContext* symbol_context = execution_context->GetSymbolContext();
		const auto new_parent = symbol_context->GetParent()->With(
				symbol_context);
		SymbolTable* tmp_table = new SymbolTable(
				m_else_block_table->GetModifiers(), new_parent,
				m_else_block_table->GetTable());
		const ExecutionContext* new_execution_context =
				execution_context->WithSymbolContext(tmp_table);

		errors = m_else_block->execute(execution_context);

		delete new_execution_context;
		delete tmp_table;
		delete new_parent;
	}

	return errors;
}

const AnalysisResult IfStatement::Returns(const TypeSpecifier* type_specifier,
		const ExecutionContext* execution_context) const {
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

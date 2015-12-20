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

#include "for_statement.h"
#include <expression.h>
#include <defaults.h>
#include <assert.h>
#include <assignment_statement.h>
#include "statement_block.h"
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <type_specifier.h>

ForStatement::ForStatement(const AssignmentStatement* initial,
		const Expression* loop_expression,
		const AssignmentStatement* loop_assignment,
		const StatementBlock* statement_block) :
		m_initial(initial), m_loop_expression(loop_expression), m_loop_assignment(
				loop_assignment), m_statement_block(statement_block), m_block_table(
				new SymbolTable()) {
	assert(loop_expression != nullptr);
	assert(loop_assignment != nullptr);
}

ForStatement::~ForStatement() {
}

const LinkedList<const Error*>* ForStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors;

	if (m_loop_expression != nullptr
			&& !(m_loop_expression->GetType(execution_context)->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt()))) {
		yy::location position = m_loop_expression->GetPosition();
		errors = new LinkedList<const Error*>(
				new Error(Error::SEMANTIC,
						Error::INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
						position.begin.line, position.begin.column));
	} else {
		SymbolContext* symbol_context = execution_context->GetSymbolContext();
		const auto new_parent = symbol_context->GetParent()->With(
				symbol_context);
		SymbolTable* tmp_table = new SymbolTable(m_block_table->GetModifiers(),
				new_parent, m_block_table->GetTable());
		const ExecutionContext* new_execution_context =
				execution_context->WithSymbolContext(tmp_table);
		errors = m_statement_block->preprocess(new_execution_context);
		delete new_execution_context;
		delete tmp_table;
		delete new_parent;
	}

	return errors;
}

const LinkedList<const Error*>* ForStatement::execute(
		ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* initialization_errors;

	SymbolContext* symbol_context = execution_context->GetSymbolContext();
	const auto new_parent = symbol_context->GetParent()->With(symbol_context);
	SymbolTable* tmp_table = new SymbolTable(m_block_table->GetModifiers(),
			new_parent, m_block_table->GetTable());
	ExecutionContext* new_execution_context =
			execution_context->WithSymbolContext(tmp_table);

	if (m_initial != nullptr) {
		initialization_errors = m_initial->execute(new_execution_context);
		if (initialization_errors
				!= LinkedList<const Error*>::GetTerminator()) {
			return initialization_errors;
		}
	}
	const Result* evaluation = (Result*) m_loop_expression->Evaluate(
			new_execution_context);

	if (evaluation->GetErrors() != LinkedList<const Error*>::GetTerminator()) {
		delete (evaluation);
		return evaluation->GetErrors();
	}

	while (*(static_pointer_cast<const bool>(evaluation->GetData()))) {
		const LinkedList<const Error*>* iteration_errors = LinkedList<
				const Error*>::GetTerminator();
		if (m_statement_block != nullptr) {
			iteration_errors = m_statement_block->execute(
					new_execution_context);
		}
		if (iteration_errors != LinkedList<const Error*>::GetTerminator()) {
			return iteration_errors;
		}

		const LinkedList<const Error*>* assignment_errors;
		assignment_errors = m_loop_assignment->execute(new_execution_context);
		if (assignment_errors != LinkedList<const Error*>::GetTerminator()) {
			return assignment_errors;
		}

		delete (evaluation);
		evaluation = (Result*) m_loop_expression->Evaluate(
				new_execution_context);

		if (evaluation->GetErrors()
				!= LinkedList<const Error*>::GetTerminator()) {
			return evaluation->GetErrors();
		}
	}
	delete (evaluation);

	delete new_execution_context;
	delete tmp_table;
	delete new_parent;

	return LinkedList<const Error*>::GetTerminator();
}

const AnalysisResult ForStatement::Returns(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const ExecutionContext* execution_context) const {
	//as of this writing, it is deemed prohibitively complicated to
	//perform the semantic analysis that would determine whether or not
	//this loop will execute, or how many times it will execute.
	return AnalysisResult::NO;
}

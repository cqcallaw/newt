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

ForStatement::ForStatement(const_shared_ptr<AssignmentStatement> initial,
		const_shared_ptr<Expression> loop_expression,
		const_shared_ptr<AssignmentStatement> loop_assignment,
		const_shared_ptr<StatementBlock> statement_block) :
		m_initial(initial), m_loop_expression(loop_expression), m_loop_assignment(
				loop_assignment), m_statement_block(statement_block), m_block_table(
				make_shared<SymbolTable>()) {
	assert(loop_expression != nullptr);
	assert(loop_assignment != nullptr);
}

ForStatement::~ForStatement() {
}

const ErrorList ForStatement::preprocess(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors;

	if (m_loop_expression != nullptr
			&& !(m_loop_expression->GetType(execution_context)->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt()))) {
		yy::location position = m_loop_expression->GetPosition();
		errors = ErrorListBase::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
						position.begin.line, position.begin.column), errors);
	} else {
		volatile_shared_ptr<SymbolContext> symbol_context =
				execution_context->GetSymbolContext();
		const auto new_parent = SymbolContextListBase::From(symbol_context,
				symbol_context->GetParent());
		volatile_shared_ptr<SymbolTable> tmp_table = make_shared<SymbolTable>(
				m_block_table->GetModifiers(), new_parent,
				m_block_table->GetTable());
		const_shared_ptr<ExecutionContext> new_execution_context =
				execution_context->WithSymbolContext(tmp_table);
		errors = m_statement_block->preprocess(new_execution_context);
	}

	return errors;
}

const ErrorList ForStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorList initialization_errors;

	volatile_shared_ptr<SymbolContext> symbol_context =
			execution_context->GetSymbolContext();
	const auto new_parent = SymbolContextListBase::From(symbol_context,
			symbol_context->GetParent());
	volatile_shared_ptr<SymbolTable> tmp_table = make_shared<SymbolTable>(
			m_block_table->GetModifiers(), new_parent,
			m_block_table->GetTable());

	shared_ptr<ExecutionContext> new_execution_context =
			execution_context->WithSymbolContext(tmp_table);

	if (m_initial != nullptr) {
		initialization_errors = m_initial->execute(new_execution_context);
		if (!ErrorListBase::IsTerminator(initialization_errors)) {
			return initialization_errors;
		}
	}
	plain_shared_ptr<Result> evaluation = m_loop_expression->Evaluate(
			new_execution_context);

	if (!ErrorListBase::IsTerminator(evaluation->GetErrors())) {

		return evaluation->GetErrors();
	}

	while (*(static_pointer_cast<const bool>(evaluation->GetData()))) {
		ErrorList iteration_errors = ErrorListBase::GetTerminator();
		if (m_statement_block != nullptr) {
			iteration_errors = m_statement_block->execute(
					new_execution_context);
		}
		if (!ErrorListBase::IsTerminator(iteration_errors)) {
			return iteration_errors;
		}

		ErrorList assignment_errors;
		assignment_errors = m_loop_assignment->execute(new_execution_context);
		if (!ErrorListBase::IsTerminator(assignment_errors)) {
			return assignment_errors;
		}

		evaluation = m_loop_expression->Evaluate(new_execution_context);

		if (!ErrorListBase::IsTerminator(evaluation->GetErrors())) {
			return evaluation->GetErrors();
		}
	}

	return ErrorListBase::GetTerminator();
}

const AnalysisResult ForStatement::Returns(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<ExecutionContext> execution_context) const {
	//as of this writing, it is deemed prohibitively complicated to
	//perform the semantic analysis that would determine whether or not
	//this loop will execute, or how many times it will execute.
	return AnalysisResult::NO;
}

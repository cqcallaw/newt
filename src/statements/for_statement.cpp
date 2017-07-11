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
#include <declaration_statement.h>
#include "statement_block.h"
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <specifiers/type_specifier.h>

ForStatement::ForStatement(const_shared_ptr<AssignmentStatement> initial,
		const_shared_ptr<Expression> loop_expression,
		const_shared_ptr<AssignmentStatement> loop_assignment,
		const_shared_ptr<StatementBlock> statement_block) :
		ForStatement(static_pointer_cast<const Statement>(initial),
				loop_expression, loop_assignment, statement_block) {
}

ForStatement::ForStatement(const_shared_ptr<DeclarationStatement> initial,
		const_shared_ptr<Expression> loop_expression,
		const_shared_ptr<AssignmentStatement> loop_assignment,
		const_shared_ptr<StatementBlock> statement_block) :
		ForStatement(static_pointer_cast<const Statement>(initial),
				loop_expression, loop_assignment, statement_block) {
}

ForStatement::~ForStatement() {
}

const PreprocessResult ForStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = ErrorList::GetTerminator();

	ExecutionContextListRef context_parent = ExecutionContextList::From(context,
			context->GetParent());
	auto new_block_typetable = m_block_context->GetTypeTable()->WithParent(
			context->GetTypeTable());

	m_block_context->LinkToParent(context);

	if (m_initial) {
		auto initial_preprocess_result = m_initial->Preprocess(m_block_context,
				m_block_context, return_type_specifier);
		errors = initial_preprocess_result.GetErrors();
		if (!ErrorList::IsTerminator(errors)) {
			return PreprocessResult(PreprocessResult::ReturnCoverage::NONE,
					errors);
		}
	}

	auto return_coverage = PreprocessResult::ReturnCoverage::NONE;
	// can't nest this logic because m_initial might be empty
	if (m_loop_expression) {
		auto loop_expression_type_specifier_result =
				m_loop_expression->GetTypeSpecifier(context);

		errors = loop_expression_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto loop_expression_type_specifier =
					loop_expression_type_specifier_result.GetData();
			auto loop_expression_analysis =
					loop_expression_type_specifier->AnalyzeAssignmentTo(
							PrimitiveTypeSpecifier::GetInt(),
							context->GetTypeTable());
			if (loop_expression_analysis == EQUIVALENT
					|| loop_expression_analysis == UNAMBIGUOUS) {
				auto block_preprocess_result = m_statement_block->Preprocess(
						m_block_context, return_type_specifier);
				return_coverage = block_preprocess_result.GetReturnCoverage();
				errors = block_preprocess_result.GetErrors();
			} else {
				yy::location position = m_loop_expression->GetLocation();
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::INVALID_TYPE_FOR_FOR_STMT_EXPRESSION,
								position.begin.line, position.begin.column),
						errors);
			}
		}
	}

	return PreprocessResult(return_coverage, errors);
}

const ErrorListRef ForStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto execution_context = ExecutionContext::GetRuntimeInstance(
			m_block_context, context);

	if (m_initial) {
		auto initialization_errors = m_initial->Execute(execution_context,
				closure);
		if (!ErrorList::IsTerminator(initialization_errors)) {
			return initialization_errors;
		}
	}

	auto evaluation = m_loop_expression->Evaluate(execution_context, closure);
	auto errors = evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		while (*(evaluation->GetData<bool>())) {
			ErrorListRef iteration_errors = ErrorList::GetTerminator();
			if (m_statement_block) {
				iteration_errors = m_statement_block->Execute(
						execution_context);
				context->SetReturnValue(execution_context->GetReturnValue());
			}
			if (!ErrorList::IsTerminator(iteration_errors)) {
				errors = iteration_errors;
				break;
			}

			auto assignment_errors = m_loop_assignment->Execute(
					execution_context, execution_context);
			if (!ErrorList::IsTerminator(assignment_errors)) {
				errors = iteration_errors;
				break;
			}

			evaluation = m_loop_expression->Evaluate(execution_context,
					closure);
			errors = evaluation->GetErrors();
			if (!ErrorList::IsTerminator(errors)) {
				errors = iteration_errors;
				break;
			}
		}
	}

	return errors;
}

ForStatement::ForStatement(const_shared_ptr<Statement> initial,
		const_shared_ptr<Expression> loop_expression,
		const_shared_ptr<AssignmentStatement> loop_assignment,
		const_shared_ptr<StatementBlock> statement_block) :
		m_initial(initial), m_loop_expression(loop_expression), m_loop_assignment(
				loop_assignment), m_statement_block(statement_block), m_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)) {
	assert(loop_expression);
	assert(loop_assignment);
}

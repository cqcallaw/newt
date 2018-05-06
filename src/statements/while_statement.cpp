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

#include <expression.h>
#include <statement_block.h>
#include <execution_context.h>
#include <return_statement.h>
#include <while_statement.h>
#include <complex_type.h>

WhileStatement::WhileStatement(const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> block, WhileMode mode) :
		m_expression(expression), m_block(block), m_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)), m_mode(
				mode) {
}

WhileStatement::~WhileStatement() {
}

const PreprocessResult WhileStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	assert(m_expression);

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			context);

	auto return_coverage = PreprocessResult::ReturnCoverage::NONE;
	auto errors = expression_type_specifier_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		errors = m_expression->Validate(context,
				TypeSpecifier::DefaultTypeSpecifierMap);
		if (ErrorList::IsTerminator(errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();
			auto expression_analysis =
					expression_type_specifier->AnalyzeAssignmentTo(
							PrimitiveTypeSpecifier::GetInt(),
							context->GetTypeTable());
			if (expression_analysis == EQUIVALENT
					|| expression_analysis == UNAMBIGUOUS) {

				// the preprocessed statement block context must persist, as it will contain initialized variables, etc.
				// thus we have the context member variables, but these cannot be linked to a context until preprocessing begins
				// N.B. that this linkage setup introduces a dependency between preprocess and execute stages
				m_block_context->LinkToParent(context);

				auto block_result = m_block->Preprocess(m_block_context,
						closure, type_parameter_list, return_type_specifier);
				auto block_return_coverage = block_result.GetReturnCoverage();

				return_coverage = ReturnStatement::CoverageTransition(
						return_coverage, block_return_coverage, true);
				errors = block_result.GetErrors();
			} else {
				yy::location position = m_expression->GetLocation();
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::INVALID_CONDITIONAL_EXPRESSION_TYPE,
								position.begin,
								expression_type_specifier->ToString()), errors);
			}
		}
	}

	return PreprocessResult(return_coverage, errors);
}

const ExecutionResult WhileStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto execution_context = ExecutionContext::GetRuntimeInstance(
			m_block_context, context);

	if (m_mode == DO_WHILE) {
		auto execution_result = m_block->Execute(execution_context, closure);
		if (execution_result.NeedsReturn()) {
			return execution_result;
		}
	}

	while (*(m_expression->Evaluate(context, closure)->GetData<bool>())) {
		auto execution_result = m_block->Execute(execution_context, closure);
		if (execution_result.NeedsReturn()) {
			return execution_result;
		}
	}

	return ExecutionResult();
}

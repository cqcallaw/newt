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

#include "statement_block.h"

#include <assert.h>
#include "statement.h"
#include <execution_context.h>
#include <specifiers/type_specifier.h>
#include <return_statement.h>
#include <if_statement.h>

using namespace std;

StatementBlock::StatementBlock(StatementListRef statements,
		const yy::location location) :
		m_statements(statements), m_location(location) {
}

StatementBlock::~StatementBlock() {
}

const PreprocessResult StatementBlock::Preprocess(
		const shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	return Preprocess(execution_context, execution_context,
			return_type_specifier);
}

const PreprocessResult StatementBlock::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure_context,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = ErrorList::GetTerminator();
	auto subject = m_statements;
	auto return_coverage = PreprocessResult::ReturnCoverage::NONE;
	while (!StatementList::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		//TODO: handle nested statement blocks
		auto statement_result = statement->Preprocess(context, closure_context,
				return_type_specifier);

		auto statement_return_coverage = statement_result.GetReturnCoverage();
		return_coverage = max(return_coverage, statement_return_coverage);
		errors = ErrorList::Concatenate(errors, statement_result.GetErrors());

		subject = subject->GetNext();
	}

	return PreprocessResult(return_coverage, errors);
}

const ErrorListRef StatementBlock::Execute(
		shared_ptr<ExecutionContext> execution_context) const {
	return Execute(execution_context, execution_context);
}

const ErrorListRef StatementBlock::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure_context) const {
	auto subject = m_statements;
	while (!StatementList::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		auto errors = statement->Execute(context, closure_context);
		if (!ErrorList::IsTerminator(errors)
				|| context->GetReturnValue() != Symbol::GetDefaultSymbol()
				|| context->GetExitCode()) {
			//we've either encountered an error, a return value has been set,
			//or an exit code has been set
			return errors;
		}
		subject = subject->GetNext();
	}

	return ErrorList::GetTerminator();
}

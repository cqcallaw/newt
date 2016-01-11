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
#include <type_specifier.h>

using namespace std;

StatementBlock::StatementBlock(StatementListRef statements,
		const yy::location location) :
		m_statements(statements), m_location(location) {
}

const ErrorListRef StatementBlock::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	auto subject = m_statements;
	while (!StatementList::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		//TODO: handle nested statement blocks
		ErrorListRef statement_errors = statement->preprocess(
				execution_context);
		errors = ErrorList::Concatenate(errors, statement_errors);

		subject = subject->GetNext();
	}

	return errors;
}

const ErrorListRef StatementBlock::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	auto subject = m_statements;
	while (!StatementList::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		auto errors = statement->execute(execution_context);
		if (!ErrorList::IsTerminator(errors)
				|| execution_context->GetReturnValue()
				|| execution_context->GetExitCode()) {
			//we've either encountered an error, a return value has been set,
			//or an exit code has been set
			return errors;
		}
		subject = subject->GetNext();
	}

	return ErrorList::GetTerminator();
}

const AnalysisResult StatementBlock::Returns(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const shared_ptr<ExecutionContext> execution_context) const {
	AnalysisResult result = AnalysisResult::NO;
	auto subject = m_statements;
	while (!StatementList::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		result = static_cast<AnalysisResult>(result
				| statement->Returns(type_specifier, execution_context));
		subject = subject->GetNext();
	}

	return result;
}

StatementBlock::~StatementBlock() {
}

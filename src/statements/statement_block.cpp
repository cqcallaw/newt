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

StatementBlock::StatementBlock(StatementList statements) :
		m_statements(statements) {
}

const ErrorList StatementBlock::preprocess(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();
	auto subject = m_statements;
	while (!StatementListBase::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		//TODO: handle nested statement blocks
		ErrorList statement_errors = statement->preprocess(execution_context);
		errors = ErrorListBase::Concatenate(errors, statement_errors);

		subject = subject->GetNext();
	}

	return errors;
}

const ErrorList StatementBlock::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	auto subject = m_statements;
	while (!StatementListBase::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		auto errors = statement->execute(execution_context);
		if (!ErrorListBase::IsTerminator(errors)
				|| execution_context->GetReturnValue()) {
			//we've either encountered an error, or a return value has been set
			return errors;
		}
		subject = subject->GetNext();
	}

	return ErrorListBase::GetTerminator();
}

const AnalysisResult StatementBlock::Returns(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<ExecutionContext> execution_context) const {
	AnalysisResult result = AnalysisResult::NO;
	auto subject = m_statements;
	while (!StatementListBase::IsTerminator(subject)) {
		const_shared_ptr<Statement> statement = subject->GetData();
		result = static_cast<AnalysisResult>(result
				| statement->Returns(type_specifier, execution_context));
		subject = subject->GetNext();
	}

	return result;
}

StatementBlock::~StatementBlock() {
}

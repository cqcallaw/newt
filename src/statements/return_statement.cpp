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

#include <return_statement.h>
#include <expression.h>
#include <execution_context.h>

ReturnStatement::ReturnStatement(const Expression* expression) :
		m_expression(expression) {
}

ReturnStatement::~ReturnStatement() {
}

const LinkedList<const Error*>* ReturnStatement::preprocess(
		const ExecutionContext* execution_context) const {
	return m_expression->Validate(execution_context);
}

const LinkedList<const Error*>* ReturnStatement::execute(
		ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	auto result = m_expression->Evaluate(execution_context);

	errors = result->GetErrors();
	if (errors->IsTerminator()) {
		execution_context->SetReturnValue(result->GetData());
	}

	return errors;
}

const AnalysisResult ReturnStatement::Returns(
		const TypeSpecifier* type_specifier,
		const ExecutionContext* execution_context) const {
	if (m_expression->GetType(execution_context) == type_specifier) {
		return AnalysisResult::YES;
	} else {
		return AnalysisResult::NO;
	}
}

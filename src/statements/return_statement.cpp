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

ReturnStatement::ReturnStatement(const_shared_ptr<Expression> expression) :
		m_expression(expression) {
}

ReturnStatement::~ReturnStatement() {
}

const ErrorListRef ReturnStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	return m_expression->Validate(execution_context);
}

const ErrorListRef ReturnStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors(ErrorList::GetTerminator());
	auto result = m_expression->Evaluate(execution_context);

	errors = result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		execution_context->SetReturnValue(
				const_shared_ptr<Symbol>(
						new Symbol(m_expression->GetType(execution_context),
								result->GetData())));
	}

	return errors;
}

const ErrorListRef ReturnStatement::GetReturnStatementErrors(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const shared_ptr<ExecutionContext> execution_context) const {
	auto errors = ErrorList::GetTerminator();
	const_shared_ptr<TypeSpecifier> expression_type_specifier =
			m_expression->GetType(execution_context);
	if (!expression_type_specifier->IsAssignableTo(type_specifier)) {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::FUNCTION_RETURN_MISMATCH,
						m_expression->GetPosition().begin.line,
						m_expression->GetPosition().begin.column), errors);
	}

	return errors;
}

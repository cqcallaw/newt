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

#include <function_declaration_statement.h>
#include <assert.h>
#include <function_type_specifier.h>
#include <function_expression.h>
#include <expression.h>

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const std::string* name, const YYLTYPE name_location,
		const FunctionExpression* expression) :
		m_name(name), m_name_location(name_location), m_expression(expression) {
}

FunctionDeclarationStatement::~FunctionDeclarationStatement() {
}

const LinkedList<const Error*>* FunctionDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
}

const LinkedList<const Error*>* FunctionDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
}

const Expression* FunctionDeclarationStatement::GetInitializerExpression() const {
	return m_expression;
}

const DeclarationStatement* FunctionDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	const FunctionExpression* as_function =
			dynamic_cast<const FunctionExpression*>(expression);
	if (as_function) {
		return new FunctionDeclarationStatement(m_name, m_name_location,
				as_function);
	} else {
		return nullptr;
	}
}

const TypeSpecifier* FunctionDeclarationStatement::GetType() const {
	return m_expression->GetType();
}

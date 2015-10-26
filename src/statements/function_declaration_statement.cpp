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
#include <basic_variable.h>
#include <assignment_statement.h>
#include <execution_context.h>
#include <function.h>

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const YYLTYPE position, const std::string* name,
		const YYLTYPE name_location, const FunctionExpression* expression) :
		DeclarationStatement(position), m_name(name), m_name_location(
				name_location), m_expression(expression) {
}

FunctionDeclarationStatement::~FunctionDeclarationStatement() {
}

const LinkedList<const Error*>* FunctionDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const Symbol* existing = execution_context->GetSymbolContext()->GetSymbol(
			m_name);

	if (existing == nullptr || existing == Symbol::DefaultSymbol) {
		errors = m_expression->Validate(execution_context);
		if (errors->IsTerminator()) {
			const Symbol* existing =
					execution_context->GetSymbolContext()->GetSymbol(m_name);

			if (existing == nullptr || existing == Symbol::DefaultSymbol) {
				const Result* result = m_expression->Evaluate(
						execution_context);

				errors = result->GetErrors();
				if (errors->IsTerminator()) {
					const Function* function =
							static_cast<const Function*>(result->GetData());
					const Symbol* symbol = new Symbol(m_name, function);
					SymbolTable* symbol_table =
							static_cast<SymbolTable*>(execution_context->GetSymbolContext());
					InsertResult insert_result = symbol_table->InsertSymbol(
							symbol);

					if (insert_result != INSERT_SUCCESS) {
						assert(false);
					}
				}
				delete result;
			} else {
				errors = errors->With(
						new Error(Error::SEMANTIC,
								Error::PREVIOUSLY_DECLARED_VARIABLE,
								m_name_location.first_line,
								m_name_location.first_column, *(m_name)));

			}
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::PREVIOUSLY_DECLARED_VARIABLE,
						m_name_location.first_line,
						m_name_location.first_column, *(m_name)));

	}

	return errors;
}

const LinkedList<const Error*>* FunctionDeclarationStatement::execute(
		ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	return errors;
}

const Expression* FunctionDeclarationStatement::GetInitializerExpression() const {
	return m_expression;
}

const DeclarationStatement* FunctionDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	const FunctionExpression* as_function =
			dynamic_cast<const FunctionExpression*>(expression);
	if (as_function) {
		return new FunctionDeclarationStatement(GetPosition(), m_name,
				m_name_location, as_function);
	} else {
		return nullptr;
	}
}

const TypeSpecifier* FunctionDeclarationStatement::GetType() const {
	return m_expression->GetType();
}

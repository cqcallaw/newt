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
#include <function_declaration.h>
#include <function_expression.h>
#include <expression.h>
#include <basic_variable.h>
#include <assignment_statement.h>
#include <execution_context.h>
#include <function.h>

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const yy::location position,
		const_shared_ptr<FunctionTypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_location,
		const Expression* initializer_expression) :
		DeclarationStatement(position), m_type(type), m_type_position(
				type_position), m_name(name), m_name_location(name_location), m_initializer_expression(
				initializer_expression) {
}

FunctionDeclarationStatement::~FunctionDeclarationStatement() {
}

const LinkedList<const Error*>* FunctionDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	auto type_table = execution_context->GetTypeTable();

	const Symbol* symbol = Symbol::GetDefaultSymbol();

	const Symbol* existing = execution_context->GetSymbolContext()->GetSymbol(
			m_name, SHALLOW);

	if (existing == nullptr || existing == Symbol::GetDefaultSymbol()) {
		if (m_initializer_expression) {
			const_shared_ptr<TypeSpecifier> expression_type =
					m_initializer_expression->GetType(execution_context);
			const_shared_ptr<FunctionTypeSpecifier> as_function =
					std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
							expression_type);

			if (as_function) {
				errors = m_initializer_expression->Validate(execution_context);
			} else {
				errors =
						errors->With(
								new Error(Error::SEMANTIC,
										Error::NOT_A_FUNCTION,
										m_initializer_expression->GetPosition().begin.line,
										m_initializer_expression->GetPosition().begin.column));
			}
		}

		if (errors->IsTerminator()) {
			auto value = m_type->DefaultValue(*type_table);
			symbol = new Symbol(static_pointer_cast<const Function>(value));

			SymbolTable* symbol_table =
					static_cast<SymbolTable*>(execution_context->GetSymbolContext());
			InsertResult insert_result = symbol_table->InsertSymbol(*m_name,
					symbol);

			if (insert_result != INSERT_SUCCESS) {
				assert(false);
			}
		}
	} else {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::PREVIOUSLY_DECLARED_VARIABLE,
						m_name_location.begin.line,
						m_name_location.begin.column, *(m_name)));
	}

	return errors;
}

const LinkedList<const Error*>* FunctionDeclarationStatement::execute(
		ExecutionContext* execution_context) const {
	if (m_initializer_expression != nullptr) {
		Variable* temp_variable = new BasicVariable(m_name, m_name_location);
		auto errors = temp_variable->AssignValue(execution_context,
				m_initializer_expression, AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return LinkedList<const Error*>::GetTerminator();
	}
}

const Expression* FunctionDeclarationStatement::GetInitializerExpression() const {
	return m_initializer_expression;
}

const DeclarationStatement* FunctionDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	return new FunctionDeclarationStatement(GetPosition(), m_type,
			expression->GetPosition(), m_name, m_name_location, expression);
}

const_shared_ptr<TypeSpecifier> FunctionDeclarationStatement::GetType() const {
	return m_type;
}

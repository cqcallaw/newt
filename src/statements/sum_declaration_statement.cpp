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

#include <symbol_table.h>
#include <expression.h>
#include <execution_context.h>
#include <sum.h>
#include <sum_declaration_statement.h>
#include <sum_type_specifier.h>
#include <basic_variable.h>

SumDeclarationStatement::SumDeclarationStatement(const yy::location position,
		const_shared_ptr<SumTypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position), m_type(type), m_type_position(
				type_position), m_name(name), m_name_location(name_location), m_initializer_expression(
				initializer_expression) {

}

SumDeclarationStatement::~SumDeclarationStatement() {
}

const ErrorListRef SumDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	plain_shared_ptr<Symbol> symbol;

	if (m_initializer_expression) {
		errors = m_initializer_expression->Validate(execution_context);

		if (ErrorList::IsTerminator(errors)) {
			auto expression_type = m_initializer_expression->GetType(
					execution_context);

			if (expression_type->IsAssignableTo(m_type)) {
				//create a sum with a default value and default tag
				//we can't use the initializer expression type as a tag because it may be equal to the type of the variable
				auto tag = m_type->GetDefaultMember();
				auto value = expression_type->DefaultValue(
						*execution_context->GetTypeTable());
				symbol = make_shared<Symbol>(
						make_shared<Sum>(m_type, tag, value));
			} else {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::INVALID_INITIALIZER_TYPE,
										m_initializer_expression->GetPosition().begin.line,
										m_initializer_expression->GetPosition().begin.column,
										*m_name, m_type->ToString(),
										expression_type->ToString()), errors);
			}
		}
	} else {
		auto sum = make_shared<Sum>(m_type, m_type->GetDefaultMember(),
				m_type->GetDefaultMember()->DefaultValue(
						*execution_context->GetTypeTable()));
		symbol = make_shared<Symbol>(sum);
	}

	InsertResult insert_result = execution_context->InsertSymbol(*m_name,
			symbol);
	if (insert_result == SYMBOL_EXISTS) {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						m_name_location.begin.line,
						m_name_location.begin.column, *m_name), errors);
	}

	return errors;
}

const ErrorListRef SumDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	if (m_initializer_expression) {
		Variable* temp_variable = new BasicVariable(m_name, m_name_location);
		auto errors = temp_variable->AssignValue(execution_context,
				m_initializer_expression, AssignmentType::ASSIGN);
		delete (temp_variable);
	}

	return errors;
}

const DeclarationStatement* SumDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new SumDeclarationStatement(GetPosition(), m_type, m_type_position,
			m_name, m_name_location, expression);
}

const_shared_ptr<TypeSpecifier> SumDeclarationStatement::GetType() const {
	return m_type;
}

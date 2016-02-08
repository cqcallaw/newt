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
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
}

FunctionDeclarationStatement::~FunctionDeclarationStatement() {
}

const ErrorListRef FunctionDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = execution_context->GetTypeTable();

	auto existing = execution_context->GetSymbol(GetName(), SHALLOW);

	if (existing == nullptr || existing == Symbol::GetDefaultSymbol()) {
		if (GetInitializerExpression()) {
			const_shared_ptr<TypeSpecifier> expression_type =
					GetInitializerExpression()->GetType(execution_context);
			const_shared_ptr<FunctionTypeSpecifier> as_function =
					std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
							expression_type);

			if (as_function) {
				errors = GetInitializerExpression()->Validate(
						execution_context);
			} else {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::NOT_A_FUNCTION,
										GetInitializerExpression()->GetPosition().begin.line,
										GetInitializerExpression()->GetPosition().begin.column),
								errors);
			}
		}

		if (ErrorList::IsTerminator(errors)) {
			auto value = m_type->DefaultValue(*type_table);
			auto symbol = make_shared<Symbol>(
					static_pointer_cast<const Function>(value));

			volatile_shared_ptr<SymbolTable> symbol_table = static_pointer_cast<
					SymbolTable>(execution_context);
			InsertResult insert_result = symbol_table->InsertSymbol(*GetName(),
					symbol);

			if (insert_result != INSERT_SUCCESS) {
				assert(false);
			}
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						GetNamePosition().begin.line,
						GetNamePosition().begin.column, *(GetName())), errors);
	}

	return errors;
}

const ErrorListRef FunctionDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	if (GetInitializerExpression()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNamePosition());
		auto errors = temp_variable->AssignValue(execution_context,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}
}

const DeclarationStatement* FunctionDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new FunctionDeclarationStatement(GetPosition(), m_type,
			expression->GetPosition(), GetName(), GetNamePosition(), expression);
}

const_shared_ptr<TypeSpecifier> FunctionDeclarationStatement::GetType() const {
	return m_type;
}

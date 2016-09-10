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

#include <assert.h>
#include <function_declaration.h>
#include <function_expression.h>
#include <expression.h>
#include <basic_variable.h>
#include <assignment_statement.h>
#include <function_declaration_statement.h>
#include <execution_context.h>
#include <function.h>

FunctionDeclarationStatement::FunctionDeclarationStatement(
		const yy::location position,
		const_shared_ptr<FunctionTypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_specifier_location(
				type_specifier_location) {
}

FunctionDeclarationStatement::~FunctionDeclarationStatement() {
}

const ErrorListRef FunctionDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();

	auto existing = context->GetSymbol(GetName(), SHALLOW);
	if (existing == nullptr || existing == Symbol::GetDefaultSymbol()) {
		if (GetInitializerExpression()) {
			auto expression_type_specifier_result =
					GetInitializerExpression()->GetTypeSpecifier(context);

			errors = expression_type_specifier_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto expression_type_specifier =
						expression_type_specifier_result.GetData();
				const_shared_ptr<FunctionTypeSpecifier> as_function =
						std::dynamic_pointer_cast<const FunctionTypeSpecifier>(
								expression_type_specifier);

				if (as_function) {
					// insert default value to allow for recursive invocations
					auto child_context = context->GetEmptyChild(context,
							Modifier::Type::MUTABLE, EPHEMERAL);

					auto value = m_type_specifier->DefaultValue(*type_table);
					auto symbol = make_shared<Symbol>(
							static_pointer_cast<const Function>(value));
					InsertResult insert_result = child_context->InsertSymbol(
							*GetName(), symbol);
					assert(insert_result == INSERT_SUCCESS);

					errors = GetInitializerExpression()->Validate(
							child_context);
					if (ErrorList::IsTerminator(errors)) {
						// passed validation; insert into output context
						InsertResult insert_result = context->InsertSymbol(
								*GetName(), symbol);
						assert(insert_result == INSERT_SUCCESS);
					}
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
		} else {
			// no initializer; initialize to default value
			auto value = m_type_specifier->DefaultValue(*type_table);

			auto symbol = make_shared<Symbol>(
					static_pointer_cast<const Function>(value));
			InsertResult insert_result = context->InsertSymbol(*GetName(),
					symbol);
			assert(insert_result == INSERT_SUCCESS);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						GetNameLocation().begin.line,
						GetNameLocation().begin.column, *(GetName())), errors);
	}

	return errors;
}

const ErrorListRef FunctionDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	if (GetInitializerExpression()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNameLocation());
		auto errors = temp_variable->AssignValue(context, closure,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}
}

const DeclarationStatement* FunctionDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new FunctionDeclarationStatement(GetLocation(), m_type_specifier,
			expression->GetPosition(), GetName(), GetNameLocation(), expression);
}

const_shared_ptr<TypeSpecifier> FunctionDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const yy::location FunctionDeclarationStatement::GetTypeSpecifierLocation() const {
	return m_type_specifier_location;
}

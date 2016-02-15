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

#include <nested_declaration_statement.h>
#include <sum_type.h>
#include <record_type.h>
#include <memory>
#include <nested_type_specifier.h>
#include <execution_context.h>
#include <member_definition.h>
#include <sum.h>
#include <basic_variable.h>
#include <memory>

NestedDeclarationStatement::NestedDeclarationStatement(
		const yy::location position, const_shared_ptr<NestedTypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
}

NestedDeclarationStatement::~NestedDeclarationStatement() {
}

const ErrorListRef NestedDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto existing = execution_context->GetSymbol(GetName(), SHALLOW);

	if (existing == nullptr || existing == Symbol::GetDefaultSymbol()) {
		//make sure we have a sum type specifier, since the parser produces record type specifiers by default
		auto parent_type_specifier = make_shared<SumTypeSpecifier>(
				m_type->GetParent());

		auto parent_sum_type = execution_context->GetTypeTable()->GetType<
				SumType>(parent_type_specifier->GetTypeName());
		if (parent_sum_type) {
			auto type_name = m_type->GetMemberName();

			shared_ptr<const Sum> value;
			auto initializer_expression = GetInitializerExpression();
			if (initializer_expression) {
				errors = initializer_expression->Validate(execution_context);

				if (ErrorList::IsTerminator(errors)) {
					if (initializer_expression->IsConstant()) {
						auto result = initializer_expression->Evaluate(
								execution_context);

						errors = result->GetErrors();
						if (ErrorList::IsTerminator(errors)) {
							value = make_shared<Sum>(
									static_pointer_cast<const SumTypeSpecifier>(
											parent_type_specifier),
									m_type->GetMemberName(), result->GetData());
						}
					}
				}
			} else {
				value = Sum::GetDefaultInstance(
						static_pointer_cast<const SumTypeSpecifier>(
								m_type->GetParent()), parent_sum_type);
			}

			if (ErrorList::IsTerminator(errors)) {
				auto symbol = make_shared<Symbol>(value);

				volatile_shared_ptr<SymbolTable> symbol_table =
						static_pointer_cast<SymbolTable>(execution_context);
				InsertResult insert_result = symbol_table->InsertSymbol(
						*GetName(), symbol);

				if (insert_result != INSERT_SUCCESS) {
					assert(false);
				}
			}
		} else {
			errors =
					ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::UNDECLARED_TYPE,
									GetInitializerExpression()->GetPosition().begin.line,
									GetInitializerExpression()->GetPosition().begin.column,
									m_type->ToString()), errors);
		}
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						GetNamePosition().begin.line,
						GetNamePosition().begin.column, *(GetName())), errors);
	}

	return errors;
}

const ErrorListRef NestedDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	if (GetInitializerExpression()
			&& !GetInitializerExpression()->IsConstant()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNamePosition());
		auto errors = temp_variable->AssignValue(execution_context,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}

	return errors;
}

const DeclarationStatement* NestedDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new NestedDeclarationStatement(GetPosition(), m_type,
			m_type_position, GetName(), GetNamePosition(), expression);
}

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

#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <basic_variable.h>
#include <primitive_declaration_statement.h>
#include <variable_expression.h>

PrimitiveDeclarationStatement::PrimitiveDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type), m_type_position(
				type_position) {
	assert(dynamic_pointer_cast<const PrimitiveTypeSpecifier>(type));
}

PrimitiveDeclarationStatement::~PrimitiveDeclarationStatement() {
}

const ErrorListRef PrimitiveDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	auto symbol = Symbol::GetDefaultSymbol();

	if (GetInitializerExpression()) {
		errors = GetInitializerExpression()->Validate(execution_context);
	}

	auto as_primitive = std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
			m_type);

	if (as_primitive) {
		if (GetInitializerExpression()) {
			const_shared_ptr<TypeSpecifier> expression_type_specifier =
					GetInitializerExpression()->GetTypeSpecifier(execution_context);

			auto expression_as_primitive = std::dynamic_pointer_cast<
					const PrimitiveTypeSpecifier>(expression_type_specifier);

			if (expression_as_primitive == nullptr
					|| !expression_as_primitive->AnalyzeAssignmentTo(as_primitive,
							execution_context->GetTypeTable())) {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::INVALID_INITIALIZER_TYPE,
										GetInitializerExpression()->GetPosition().begin.line,
										GetInitializerExpression()->GetPosition().begin.column,
										*GetName(), as_primitive->ToString(),
										expression_type_specifier->ToString()),
								errors);
			}
		}

		auto type_table = *execution_context->GetTypeTable();
		auto type = m_type->GetType(type_table);
		auto value = type->GetDefaultValue(type_table);
		symbol = type->GetSymbol(type_table, m_type, value);
	} else {
		assert(false);
	}

	if (symbol != Symbol::GetDefaultSymbol()) {
		InsertResult insert_result = execution_context->InsertSymbol(*GetName(),
				symbol);
		if (insert_result == SYMBOL_EXISTS) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::PREVIOUS_DECLARATION,
							GetNameLocation().begin.line,
							GetNameLocation().begin.column, *GetName()),
					errors);
		}
	}

	return errors;
}

const ErrorListRef PrimitiveDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	if (GetInitializerExpression()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNameLocation());
		auto errors = temp_variable->AssignValue(execution_context,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return errors;
	} else {
		return ErrorList::GetTerminator();
	}
}

const DeclarationStatement* PrimitiveDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new PrimitiveDeclarationStatement(GetLocation(), m_type,
			m_type_position, GetName(), GetNameLocation(), expression);
}

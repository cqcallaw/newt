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
#include <basic_variable.h>
#include <default_value_expression.h>
#include <sum_type.h>

SumDeclarationStatement::SumDeclarationStatement(const yy::location position,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<string> name, const yy::location name_location,
		const DeclarationListRef variant_list,
		const yy::location variant_list_location) :
		DeclarationStatement(position, name, name_location,
				make_shared<DefaultValueExpression>(
						DefaultValueExpression(GetDefaultLocation(), type,
								variant_list_location)),
				ModifierList::GetTerminator(), GetDefaultLocation()), m_variant_list(
				variant_list), m_variant_list_location(variant_list_location), m_type(
				make_shared<SumTypeSpecifier>(type)) {
}

SumDeclarationStatement::~SumDeclarationStatement() {
}

const ErrorListRef SumDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = execution_context->GetTypeTable();
	auto result = SumType::Build(execution_context, m_type, m_variant_list);

	errors = result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = static_pointer_cast<const SumType>(result->GetData());
		execution_context->GetTypeTable()->AddType(*GetName(), type);
	}

	return errors;
}

const ErrorListRef SumDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	return ErrorList::GetTerminator();
}

const DeclarationStatement* SumDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
//no-op
	return new SumDeclarationStatement(GetPosition(), m_type, GetName(),
			GetNamePosition(), m_variant_list, m_variant_list_location);
}

const_shared_ptr<TypeSpecifier> SumDeclarationStatement::GetType() const {
	return m_type;
}

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

#include <record_declaration_statement.h>
#include <error.h>
#include <expression.h>
#include <member_declaration.h>
#include <type_table.h>
#include <execution_context.h>
#include <map>
#include <member_definition.h>
#include <record_type.h>
#include <default_value_expression.h>
#include <defaults.h>

RecordDeclarationStatement::RecordDeclarationStatement(
		const yy::location position,
		const_shared_ptr<RecordTypeSpecifier> type,
		const_shared_ptr<string> name, const yy::location name_location,
		DeclarationListRef member_declaration_list,
		const yy::location member_declaration_list_location,
		ModifierListRef modifier_list, const yy::location modifiers_location) :
		DeclarationStatement(position, name, name_location,
				make_shared<DefaultValueExpression>(
						DefaultValueExpression(GetDefaultLocation(), type,
								member_declaration_list_location)),
				modifier_list, modifiers_location), m_member_declaration_list(
				member_declaration_list), m_member_declaration_list_location(
				member_declaration_list_location), m_type(type) {
}

RecordDeclarationStatement::~RecordDeclarationStatement() {
}

const ErrorListRef RecordDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = execution_context->GetTypeTable();

	Modifier::Type modifiers = Modifier::NONE;
	ModifierListRef modifier_list = GetModifierList();
	while (!ModifierList::IsTerminator(modifier_list)) {
		//TODO: check invalid modifiers
		Modifier::Type new_modifier = modifier_list->GetData()->GetType();
		modifiers = Modifier::Type(modifiers | new_modifier);
		modifier_list = modifier_list->GetNext();
	}

	auto result = RecordType::Build(execution_context, modifiers,
			m_member_declaration_list);
	errors = result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = static_pointer_cast<const RecordType>(result->GetData());
		type_table->AddType(*GetName(), type);
	}

	return errors;
}

const ErrorListRef RecordDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	return ErrorList::GetTerminator();
}

const DeclarationStatement* RecordDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	//no-op
	return new RecordDeclarationStatement(GetPosition(), m_type, GetName(),
			GetNamePosition(), m_member_declaration_list,
			m_member_declaration_list_location, GetModifierList(),
			GetModifierListLocation());
}

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
#include <type_table.h>
#include <execution_context.h>
#include <placeholder_type.h>
#include <maybe_type.h>
#include <record_type.h>
#include <default_value_expression.h>
#include <defaults.h>
#include <record.h>

RecordDeclarationStatement::RecordDeclarationStatement(
		const yy::location position,
		const_shared_ptr<RecordTypeSpecifier> type_specifier,
		const_shared_ptr<string> name, const yy::location name_location,
		DeclarationListRef member_declaration_list,
		const yy::location member_declaration_list_location,
		ModifierListRef modifier_list, const yy::location modifiers_location) :
		DeclarationStatement(position, name, name_location,
				make_shared<DefaultValueExpression>(GetDefaultLocation(),
						type_specifier, member_declaration_list_location),
				modifier_list, modifiers_location), m_member_declaration_list(
				member_declaration_list), m_member_declaration_list_location(
				member_declaration_list_location), m_type(type_specifier) {
}

RecordDeclarationStatement::~RecordDeclarationStatement() {
}

const PreprocessResult RecordDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = context->GetTypeTable();

	Modifier::Type modifiers = Modifier::NONE;
	ModifierListRef modifier_list = GetModifierList();
	while (!ModifierList::IsTerminator(modifier_list)) {
		//TODO: check invalid modifiers
		Modifier::Type new_modifier = modifier_list->GetData()->GetType();
		modifiers = Modifier::Type(modifiers | new_modifier);
		modifier_list = modifier_list->GetNext();
	}

	if (!type_table->ContainsType(*m_type)) {
		errors = RecordType::Build(GetName(), context, closure, modifiers,
				m_member_declaration_list, m_type);
	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						GetNameLocation().begin.line,
						GetNameLocation().begin.column, *GetName()), errors);
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ExecutionResult RecordDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	return ExecutionResult();
}

const DeclarationStatement* RecordDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	//no-op
	return new RecordDeclarationStatement(GetLocation(), m_type, GetName(),
			GetNameLocation(), m_member_declaration_list,
			m_member_declaration_list_location, GetModifierList(),
			GetModifierListLocation());
}

const_shared_ptr<RecordDeclarationStatement> RecordDeclarationStatement::WithModifiers(
		ModifierListRef modifiers,
		const yy::location modifiers_location) const {
	return make_shared<RecordDeclarationStatement>(GetLocation(), m_type,
			GetName(), GetNameLocation(), m_member_declaration_list,
			m_member_declaration_list_location, modifiers, modifiers_location);
}

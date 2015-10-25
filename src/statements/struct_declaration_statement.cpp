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

#include <struct_declaration_statement.h>
#include <error.h>
#include <expression.h>
#include <member_declaration.h>
#include <type_table.h>
#include <execution_context.h>
#include <map>
#include <member_definition.h>
#include <compound_type.h>
#include <default_value_expression.h>

StructDeclarationStatement::StructDeclarationStatement(const std::string* name,
		const YYLTYPE name_position,
		const DeclarationList* member_declaration_list,
		const YYLTYPE member_declaration_list_position,
		const ModifierList* modifier_list, const YYLTYPE modifiers_location) :
		m_name(name), m_name_position(name_position), m_member_declaration_list(
				member_declaration_list), m_member_declaration_list_position(
				member_declaration_list_position), m_modifier_list(
				modifier_list), m_modifiers_location(modifiers_location) {
	m_type_specifier = new CompoundTypeSpecifier(*name, name_position);
	m_initializer_expression = new DefaultValueExpression(DefaultLocation,
			m_type_specifier, member_declaration_list_position);
}

StructDeclarationStatement::~StructDeclarationStatement() {
}

const LinkedList<const Error*>* StructDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	TypeTable* type_table = execution_context->GetTypeTable();

	Modifier::Type modifiers = Modifier::NONE;
	const LinkedList<const Modifier*>* modifier_list = m_modifier_list;
	while (modifier_list != LinkedList<const Modifier*>::Terminator) {
		//TODO: check invalid modifiers
		Modifier::Type new_modifier = modifier_list->GetData()->GetType();
		modifiers = Modifier::Type(modifiers | new_modifier);
		modifier_list = modifier_list->GetNext();
	}

	//generate a temporary structure in which to perform evaluations
	//of the member declaration statements
	map<const string, const Symbol*, comparator>* values = new map<const string,
			const Symbol*, comparator>();
	SymbolTable* member_buffer = new SymbolTable(Modifier::NONE,
			LinkedList<SymbolContext*>::Terminator, values);
	ExecutionContext* struct_context =
			execution_context->WithSymbolContext(member_buffer);

	const LinkedList<const DeclarationStatement*>* subject =
			m_member_declaration_list;
	while (!subject->IsTerminator()) {
		const DeclarationStatement* declaration = subject->GetData();

		const Expression* initializer_expression =
				declaration->GetInitializerExpression();
		if (initializer_expression != nullptr
				&& !initializer_expression->IsConstant()) {

			YYLTYPE position = initializer_expression->GetPosition();
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.first_line, position.first_column));
		} else {
			errors = errors->Concatenate(
					declaration->preprocess(struct_context), true);
		}

		if (errors->IsTerminator()) {
			//we've pre-processed this statement without issue
			errors = errors->Concatenate(declaration->execute(struct_context),
					true);
		}

		subject = subject->GetNext();
	}

	std::map<const string, const MemberDefinition*>* mapping = new std::map<
			const string, const MemberDefinition*>();
	map<const string, const Symbol*, comparator>::iterator iter;
	if (errors->IsTerminator()) {
		//we've evaluated everything without issue
		//extract member declaration information into immutable MemberDefinition
		for (iter = values->begin(); iter != values->end(); iter++) {
			const string member_name = iter->first;
			const Symbol* symbol = iter->second;
			const TypeSpecifier* type = symbol->GetType();
			const void* value = symbol->GetValue();
			mapping->insert(
					pair<const string, const MemberDefinition*>(member_name,
							new MemberDefinition(type, value)));

		}
	}

	type_table->AddType(*m_name, new CompoundType(mapping, modifiers));

	return errors;
}

const LinkedList<const Error*>* StructDeclarationStatement::execute(
		ExecutionContext* execution_context) const {
	return LinkedList<const Error*>::Terminator;
}

const DeclarationStatement* StructDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	//no-op
	return new StructDeclarationStatement(m_name, m_name_position,
			m_member_declaration_list, m_member_declaration_list_position,
			m_modifier_list, m_modifiers_location);
}

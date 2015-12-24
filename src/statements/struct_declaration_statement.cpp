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
#include <defaults.h>

StructDeclarationStatement::StructDeclarationStatement(
		const yy::location position, const_shared_ptr<string> name,
		const yy::location name_position,
		DeclarationList member_declaration_list,
		const yy::location member_declaration_list_position,
		ModifierList modifier_list, const yy::location modifiers_location) :
		DeclarationStatement(position), m_name(name), m_name_position(
				name_position), m_member_declaration_list(
				member_declaration_list), m_member_declaration_list_position(
				member_declaration_list_position), m_modifier_list(
				modifier_list), m_modifiers_location(modifiers_location), m_type_specifier(
				const_shared_ptr<const CompoundTypeSpecifier>(
						new CompoundTypeSpecifier(*name, name_position))), m_initializer_expression(
				make_shared<DefaultValueExpression>(
						DefaultValueExpression(GetDefaultLocation(),
								m_type_specifier,
								member_declaration_list_position))) {
}

StructDeclarationStatement::~StructDeclarationStatement() {
}

const ErrorList StructDeclarationStatement::preprocess(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	auto type_table = execution_context->GetTypeTable();

	Modifier::Type modifiers = Modifier::NONE;
	ModifierList modifier_list = m_modifier_list;
	while (!ModifierListBase::IsTerminator(modifier_list)) {
		//TODO: check invalid modifiers
		Modifier::Type new_modifier = modifier_list->GetData()->GetType();
		modifiers = Modifier::Type(modifiers | new_modifier);
		modifier_list = modifier_list->GetNext();
	}

	//generate a temporary structure in which to perform evaluations
	//of the member declaration statements
	const shared_ptr<symbol_map> values = make_shared<symbol_map>();
	volatile_shared_ptr<SymbolTable> member_buffer = make_shared<SymbolTable>(
			Modifier::NONE, SymbolContextListBase::GetTerminator(), values);
	shared_ptr<ExecutionContext> struct_context =
			execution_context->WithSymbolContext(member_buffer);

	DeclarationList subject = m_member_declaration_list;
	while (!DeclarationListBase::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> declaration = subject->GetData();

		const_shared_ptr<Expression> initializer_expression =
				declaration->GetInitializerExpression();
		if (initializer_expression != nullptr
				&& !initializer_expression->IsConstant()) {
			//if we have a non-constant initializer expression, generate an error
			yy::location position = initializer_expression->GetPosition();
			errors = ErrorListBase::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.begin.line, position.begin.column),
					errors);
		} else {
			//otherwise (no initializer expression OR a valid initializer expression), preprocess
			errors = ErrorListBase::Concatenate(errors,
					declaration->preprocess(struct_context));
		}

		if (ErrorListBase::IsTerminator(errors)) {
			//we've pre-processed this statement without issue
			errors = ErrorListBase::Concatenate(errors,
					declaration->execute(struct_context));
		}

		subject = subject->GetNext();
	}

	std::map<const string, const_shared_ptr<MemberDefinition>>* mapping =
			new std::map<const string, const_shared_ptr<MemberDefinition>>();
	symbol_map::iterator iter;
	if (ErrorListBase::IsTerminator(errors)) {
		//we've evaluated everything without issue
		//extract member declaration information into immutable MemberDefinition
		for (iter = values->begin(); iter != values->end(); iter++) {
			const string member_name = iter->first;
			auto symbol = iter->second;
			const_shared_ptr<TypeSpecifier> type = symbol->GetType();
			auto value = symbol->GetValue();
			const_shared_ptr<MemberDefinition> definition = make_shared<
					MemberDefinition>(type, value);
			mapping->insert(
					pair<const string, const_shared_ptr<MemberDefinition>>(
							member_name, definition));
		}
	}

	auto type = make_shared<CompoundType>(mapping, modifiers);
	type_table->AddType(*m_name, type);
	return errors;
}

const ErrorList StructDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	return ErrorListBase::GetTerminator();
}

const DeclarationStatement* StructDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	//no-op
	return new StructDeclarationStatement(GetPosition(), m_name,
			m_name_position, m_member_declaration_list,
			m_member_declaration_list_position, m_modifier_list,
			m_modifiers_location);
}

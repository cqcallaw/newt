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
		const MemberDeclarationList* member_declaration_list,
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

	std::map<const string, const MemberDefinition*>* mapping = new std::map<
			const string, const MemberDefinition*>();

	const LinkedList<const DeclarationStatement*>* subject =
			m_member_declaration_list;
	while (!subject->IsTerminator()) {
		const DeclarationStatement* declaration = subject->GetData();
		const TypeSpecifier* type = declaration->GetType();
		const string* member_name = declaration->GetName();

		//strip out intermediate dimensions
		const ArrayTypeSpecifier* as_array =
				dynamic_cast<const ArrayTypeSpecifier*>(type);
		while (as_array) {
			type = as_array->GetElementTypeSpecifier();
			as_array = dynamic_cast<const ArrayTypeSpecifier*>(type);
		}

		//verify compound types exist
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(type);
		if (as_compound) {
			const string compound_type_name = as_compound->GetTypeName();
			const CompoundType* type = type_table->GetType(compound_type_name);
			if (type == CompoundType::GetDefaultCompoundType()) {
				errors = errors->With(
						new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
								as_compound->GetLocation().first_line,
								as_compound->GetLocation().first_column,
								compound_type_name));
				break;
			}
		}

		const Expression* initializer_expression =
				subject->GetData()->GetInitializerExpression();
		if (initializer_expression != nullptr
				&& !initializer_expression->IsConstant()) {
			YYLTYPE position = initializer_expression->GetPosition();
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.first_line, position.first_column));
		} else {
			const TypeSpecifier* original_type = declaration->GetType();
			mapping->insert(
					pair<const string, const MemberDefinition*>(*member_name,
							new MemberDefinition(original_type,
									declaration->GetInitializerExpression()
											!= nullptr ?
											declaration->GetInitializerExpression()->Evaluate(
													execution_context)->GetData() :
											original_type->DefaultValue(
													type_table))));

		}
		subject = subject->GetNext();
	}

	Modifier::Type modifiers = Modifier::NONE;

	const LinkedList<const Modifier*>* modifier_list = m_modifier_list;

	while (modifier_list != LinkedList<const Modifier*>::Terminator) {
		//TODO: check invalid modifiers
		Modifier::Type new_modifier = modifier_list->GetData()->GetType();
		modifiers = Modifier::Type(modifiers | new_modifier);
		modifier_list = modifier_list->GetNext();
	}

	type_table->AddType(*m_name, new CompoundType(mapping, modifiers));

//TODO: verify member names are unique

	return errors;
}

const LinkedList<const Error*>* StructDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	return LinkedList<const Error*>::Terminator;
}

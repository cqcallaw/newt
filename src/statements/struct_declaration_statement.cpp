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

StructDeclarationStatement::StructDeclarationStatement(const std::string* name,
		const YYLTYPE name_position,
		const MemberDeclarationList* member_declaration_list,
		const YYLTYPE member_declaration_list_position,
		const ModifierList* modifier_list, const YYLTYPE modifiers_location) :
		m_name(name), m_name_position(name_position), m_member_declaration_list(
				member_declaration_list), m_member_declaration_list_position(
				member_declaration_list_position), m_modifier_list(
				modifier_list), m_modifiers_location(modifiers_location) {
}

StructDeclarationStatement::~StructDeclarationStatement() {
}

LinkedList<const Error*>* StructDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;

	const LinkedList<const MemberDeclaration*>* subject =
			m_member_declaration_list;
	while (subject != LinkedList<const MemberDeclaration*>::Terminator) {
		const Expression* initializer_expression =
				subject->GetData()->GetInitializerExpression();
		if (initializer_expression != nullptr
				&& !initializer_expression->IsConstant()) {
			YYLTYPE position = initializer_expression->GetPosition();
			errors = (LinkedList<const Error*>*) errors->With(
					new Error(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.first_line, position.first_column));
		}
		subject = subject->GetNext();
	}

	return errors;
}

const LinkedList<const Error*>* StructDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	TypeTable* type_table = execution_context->GetTypeTable();

	CompoundType* compound_type = new CompoundType();

	const LinkedList<const MemberDeclaration*>* subject =
			m_member_declaration_list;
	while (subject != LinkedList<const MemberDeclaration*>::Terminator) {
		const MemberDeclaration* declaration = subject->GetData();
		const PrimitiveType type = declaration->GetType();
		const string* member_name = declaration->GetName();
		compound_type->insert(
				pair<const string, const MemberDefinition*>(*member_name,
						new MemberDefinition(type,
								declaration->GetInitializerExpression()
										!= nullptr ?
										declaration->GetInitializerExpression()->Evaluate(
												execution_context)->GetData() :
										DefaultTypeValue(type))));
		subject = subject->GetNext();
	}

	type_table->AddType(*m_name, compound_type);

	//TODO: error handling
	return LinkedList<const Error*>::Terminator;
}

/*
 * struct_declaration_statement.cpp
 *
 *  Created on: Aug 28, 2015
 *      Author: caleb
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

	LinkedList<const MemberDeclaration*>* subject = (LinkedList<
			const MemberDeclaration*>*) m_member_declaration_list;
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
		subject = (LinkedList<const MemberDeclaration*>*) subject->GetNext();
	}

	return errors;
}

const LinkedList<const Error*>* StructDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	TypeTable* type_table = execution_context->GetTypeTable();

	CompoundType* type = new CompoundType();

	const LinkedList<const MemberDeclaration*>* subject =
			m_member_declaration_list;
	while (subject != LinkedList<const MemberDeclaration*>::Terminator) {
		const MemberDeclaration* declaration = subject->GetData();
		type->insert(
				pair<string, PrimitiveType>(*declaration->GetName(),
						declaration->GetType()));
		subject = subject->GetNext();
	}

	type_table->AddType(*m_name, type);

	//TODO: error handling
	return LinkedList<const Error*>::Terminator;
}

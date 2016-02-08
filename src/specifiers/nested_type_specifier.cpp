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

#include <sstream>
#include <nested_type_specifier.h>
#include <sum_type.h>
#include <record_type.h>
#include <member_definition.h>
#include <nested_declaration_statement.h>
#include <record_type_specifier.h>

NestedTypeSpecifier::NestedTypeSpecifier(
		const_shared_ptr<ComplexTypeSpecifier> parent,
		const_shared_ptr<std::string> member_name) :
		m_parent(parent), m_member_name(member_name) {
}

NestedTypeSpecifier::~NestedTypeSpecifier() {
}

const std::string NestedTypeSpecifier::ToString() const {
	std::ostringstream buffer;
	buffer << m_parent->ToString() << "." << *m_member_name;
	return buffer.str();
}

const bool NestedTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other) const {
	auto other_as_nested = dynamic_pointer_cast<const NestedTypeSpecifier>(
			other);

	if (other_as_nested) {
		if (other_as_nested->GetParent() == m_parent) {
			return other_as_nested->GetMemberName() == m_member_name;
		}
	}

	return false;
}

const_shared_ptr<void> NestedTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto parent_type = type_table.GetType<TypeDefinition>(
			m_parent->GetTypeName());

	auto as_sum_type = dynamic_pointer_cast<const SumType>(parent_type);
	if (as_sum_type) {
		const_shared_ptr<TypeSpecifier> variant_type =
				as_sum_type->GetMemberType(*m_member_name);
		if (variant_type != PrimitiveTypeSpecifier::GetNone()) {
			return variant_type->DefaultValue(*as_sum_type->GetTypeTable());
		}
	}

	auto as_record_type = dynamic_pointer_cast<const RecordType>(
			parent_type);
	if (as_record_type) {
		auto definition = as_record_type->GetMember(*m_member_name);
		if (definition != MemberDefinition::GetDefaultMemberDefinition()) {
			return definition->GetDefaultValue();
		}
	}

	return const_shared_ptr<void>();
}

const_shared_ptr<DeclarationStatement> NestedTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {

	return make_shared<NestedDeclarationStatement>(position,
			static_pointer_cast<const NestedTypeSpecifier>(type), type_position,
			name, name_position, initializer_expression);
}

bool NestedTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const NestedTypeSpecifier& as_nested =
				dynamic_cast<const NestedTypeSpecifier&>(other);
		return as_nested.m_parent == m_parent
				&& as_nested.m_member_name == m_member_name;
	} catch (std::bad_cast& e) {
		return false;
	}
}

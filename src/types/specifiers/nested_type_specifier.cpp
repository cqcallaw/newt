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

#include <nested_declaration_statement.h>
#include <sstream>
#include <nested_type_specifier.h>
#include <sum_type.h>
#include <record_type.h>
#include <primitive_type.h>
#include <member_definition.h>
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
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto other_as_nested = dynamic_pointer_cast<const NestedTypeSpecifier>(
			other);

	if (other_as_nested) {
		if (*other_as_nested->GetParent() == *m_parent) {
			return *other_as_nested->GetMemberName() == *m_member_name;
		}
	}

	auto un_aliased = ResolveAliasing(type_table);
	if (un_aliased) {
		return un_aliased->IsAssignableTo(other, type_table);
	}

	return false;
}

const_shared_ptr<void> NestedTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto parent_type = type_table.GetType<ComplexType>(m_parent);

	if (parent_type) {
		return parent_type->GetMemberDefaultValue(m_member_name);
	} else {
		return const_shared_ptr<void>();
	}
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

const_shared_ptr<Symbol> NestedTypeSpecifier::GetSymbol(
		const_shared_ptr<void> value, const TypeTable& container) const {
	auto parent_type = container.GetType<ComplexType>(m_parent);

	if (parent_type) {
		auto member_type_specifier = parent_type->GetMemberTypeSpecifier(
				m_member_name);
		return member_type_specifier->GetSymbol(value, container);
	} else {
		return Symbol::GetDefaultSymbol();
	}
}

const_shared_ptr<TypeDefinition> NestedTypeSpecifier::GetType(
		const TypeTable& type_table,
		AliasResolution resolution) const {
	auto parent_type = type_table.GetType<ComplexType>(m_parent);

	if (parent_type) {
		return parent_type->GetDefinition()->GetType<TypeDefinition>(
				m_member_name, resolution);
	} else {
		return nullptr;
	}
}

const_shared_ptr<TypeSpecifier> NestedTypeSpecifier::ResolveAliasing(
		const TypeTable& type_table) const {
	auto parent_type = type_table.GetType<ComplexType>(m_parent);
	if (parent_type) {
		auto type_definition = parent_type->GetDefinition()->GetType<
				TypeDefinition>(m_member_name,
				AliasResolution::RETURN);
		auto as_alias = dynamic_pointer_cast<const AliasDefinition>(
				type_definition);
		if (as_alias) {
			return as_alias->GetOriginal();
		}
	}

	return const_shared_ptr<TypeSpecifier>();
}

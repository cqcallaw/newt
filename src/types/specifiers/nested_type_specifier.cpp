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
#include <complex_type_specifier.h>
#include <sum_type.h>
#include <record_type.h>
#include <primitive_type.h>
#include <unit_type.h>
#include <placeholder_type.h>
#include <record_type_specifier.h>

NestedTypeSpecifier::NestedTypeSpecifier(const_shared_ptr<TypeSpecifier> parent,
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

const AnalysisResult NestedTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto other_as_nested = dynamic_pointer_cast<const NestedTypeSpecifier>(
			other);

	if (other_as_nested) {
		if (*other_as_nested->GetParent() == *m_parent) {
			if (*other_as_nested->GetMemberName() == *m_member_name) {
				return AnalysisResult::EQUIVALENT;
			}
		}
	}

	return AnalysisResult::INCOMPATIBLE;
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

const_shared_ptr<TypeDefinition> NestedTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto parent_type = m_parent->GetType(type_table);
	auto as_complex = dynamic_pointer_cast<const ComplexType>(parent_type);
	if (as_complex) {
		auto as_placeholder = dynamic_pointer_cast<const PlaceholderType>(
				as_complex);
		if (as_placeholder) {
			return as_placeholder;
		} else {
			return as_complex->GetDefinition()->GetType<TypeDefinition>(
					m_member_name, DEEP, resolution);
		}
	} else {
		return nullptr;
	}
}

const_shared_ptr<TypeSpecifier> NestedTypeSpecifier::Resolve(
		const_shared_ptr<TypeSpecifier> source, const TypeTable& type_table) {
	auto as_nested = dynamic_pointer_cast<const NestedTypeSpecifier>(source);

	if (as_nested) {
		auto type = as_nested->GetParent()->GetType(type_table, RESOLVE);
		if (type) {
			auto resolved_parent = Resolve(as_nested->GetParent(), type_table);

			auto resolved_parent_as_complex = dynamic_pointer_cast<
					const ComplexTypeSpecifier>(resolved_parent);
			if (resolved_parent_as_complex) {
				auto parent_type = resolved_parent_as_complex->GetType(
						type_table);

				auto as_complex_type = dynamic_pointer_cast<const ComplexType>(
						parent_type);
				if (as_complex_type) {
					auto type_definition =
							as_complex_type->GetDefinition()->GetType<
									TypeDefinition>(as_nested->m_member_name,
									DEEP, RETURN);
					auto as_alias = dynamic_pointer_cast<const AliasDefinition>(
							type_definition);
					if (as_alias) {
						return as_alias->GetOriginal();
					} else {
						return type->GetTypeSpecifier(as_nested->m_member_name,
								resolved_parent_as_complex);
					}
				}
			}
		}
	}

	return source;
}

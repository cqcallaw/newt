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

#include <complex_type_specifier.h>
#include <complex_type.h>
#include <nested_type_specifier.h>

const_shared_ptr<ComplexType> ComplexTypeSpecifier::GetContainerType(
		const TypeTable& type_table) const {
	auto container_specifier = GetContainer();
	if (container_specifier) {
		auto type = container_specifier->GetType(type_table);

		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			return as_complex;
		}

	}

	return const_shared_ptr<ComplexType>();
}

const_shared_ptr<void> ComplexTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto type = GetType(type_table, RESOLVE);

	if (type) {
		return type->GetDefaultValue(type_table);
	}

	return nullptr;
}

bool ComplexTypeSpecifier::CompareContainers(
		const ComplexTypeSpecifier& other) const {
	auto containers = GetContainer();
	auto other_containers = other.GetContainer();
	if (!containers) {
		return !other_containers;
	}

	if (!other_containers) {
		return !containers;
	}

	return *containers == *other_containers;
}

const_shared_ptr<TypeDefinition> ComplexTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	plain_shared_ptr<ComplexType> type = nullptr;

	auto container_type = GetContainerType(type_table);
	if (container_type) {
		type = container_type->GetDefinition()->GetType<ComplexType>(
				GetTypeName(), DEEP, resolution);
	} else {
		type = type_table.GetType<ComplexType>(GetTypeName(), DEEP, resolution);
	}

	return type;
}

const bool ComplexTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto resolved_other = NestedTypeSpecifier::Resolve(other, type_table);

	const_shared_ptr<ComplexTypeSpecifier> as_complex =
			std::dynamic_pointer_cast<const ComplexTypeSpecifier>(
					resolved_other);
	if (as_complex) {
		try {
			auto container = GetContainer();
			auto other_container = as_complex->GetContainer();
			if (container) {
				if (other_container) {
					return *container == *other_container
							&& as_complex->GetTypeName()->compare(
									*GetTypeName()) == 0;
				}
			} else {
				if (as_complex->GetTypeName()->compare(*GetTypeName()) == 0) {
					return true;
				}
			}

			if (as_complex->AnalyzeConversion(type_table, *this)
					== UNAMBIGUOUS) {
				return true;
			}

		} catch (std::bad_cast& e) {
		}
	}

	return false;
}

const std::string ComplexTypeSpecifier::ToString() const {
	return (GetContainer() ? GetContainer()->ToString() + "." : "")
			+ *m_type_name;
}

bool ComplexTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const ComplexTypeSpecifier& as_complex =
				dynamic_cast<const ComplexTypeSpecifier&>(other);
		return CompareContainers(as_complex)
				&& *GetTypeName() == *as_complex.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const WideningResult ComplexTypeSpecifier::AnalyzeConversion(
		const TypeTable& type_table, const TypeSpecifier& other) const {
	auto type = GetType(type_table);

	if (type) {
		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			return as_complex->AnalyzeConversion(*this, other);
		}
	}

	return INCOMPATIBLE;
}

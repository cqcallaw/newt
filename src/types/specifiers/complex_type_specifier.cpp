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
	if (!GetContainer()) {
		return !other.GetContainer();
	}

	return *GetContainer() == *other.GetContainer();
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
	const_shared_ptr<ComplexTypeSpecifier> as_complex =
			std::dynamic_pointer_cast<const ComplexTypeSpecifier>(other);
	if (as_complex) {
		try {
			//the following two casts are justified because we know the resolution of a
			//complex type specifier will yield a complex type specifier
			const ComplexTypeSpecifier& resolved_specifier =
					dynamic_cast<const ComplexTypeSpecifier&>(ComplexTypeSpecifier::ResolveAliasing(
							*this, type_table));
			const ComplexTypeSpecifier& resolved_other =
					dynamic_cast<const ComplexTypeSpecifier&>(ComplexTypeSpecifier::ResolveAliasing(
							*as_complex, type_table));

			auto container = resolved_specifier.GetContainer();
			auto other_container = resolved_other.GetContainer();
			if (container) {
				if (other_container) {
					return *container == *other_container
							&& resolved_other.GetTypeName()->compare(
									*resolved_specifier.GetTypeName()) == 0;
				}
			} else {
				if (resolved_other.GetTypeName()->compare(
						*resolved_specifier.GetTypeName()) == 0) {
					return true;
				}
			}

			if (resolved_other.AnalyzeConversion(type_table, resolved_specifier)
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

const TypeSpecifier& ComplexTypeSpecifier::ResolveAliasing(
		const TypeSpecifier& original, const TypeTable& type_table) {
	try {
		const ComplexTypeSpecifier& as_complex =
				dynamic_cast<const ComplexTypeSpecifier&>(original);

		auto parent = as_complex.GetContainer();
		if (parent) {
			auto parent_type = parent->GetType(type_table);

			auto parent_as_complex = dynamic_pointer_cast<const ComplexType>(
					parent_type);
			if (parent_as_complex) {
				auto type = parent_as_complex->GetDefinition()->GetType<
						AliasDefinition>(as_complex.GetTypeName(), DEEP,
						RETURN);
				if (type) {
					return *type->GetOriginal();
				}
			}
		}
	} catch (std::bad_cast& e) {
	}

	return original;
}

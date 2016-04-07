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

const TypeSpecifier& ComplexTypeSpecifier::ResolveAliasing(
		const ComplexTypeSpecifier& original, const TypeTable& type_table) {
	auto parent = original.GetContainer();

	if (parent) {
		auto parent_type = parent->GetType(type_table);

		auto parent_as_complex = dynamic_pointer_cast<const ComplexType>(
				parent_type);
		if (parent_as_complex) {
			auto type = parent_as_complex->GetDefinition()->GetType<
					AliasDefinition>(original.GetTypeName(), RETURN);
			if (type) {
				return *type->GetOriginal();
			} else {
				cout << "Nonalias " << *original.GetTypeName() << ":" << endl;
				cout << parent_type->ToString(type_table, Indent(0));
				cout << "End Nonalias" << endl;
			}
		}
	}

	return original;
}

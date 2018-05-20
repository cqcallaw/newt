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

#include <array.h>
#include <record.h>
#include <primitive_type.h>
#include <array_type.h>
#include <record_type.h>

const string Array::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;

	int size = GetSize();
	Indent child_indent = indent + 1;

	auto element_type_specifier = GetElementTypeSpecifier();
	auto element_type_result = element_type_specifier->GetType(type_table,
			RESOLVE);

	if (ErrorList::IsTerminator(element_type_result->GetErrors())) {
		auto element_type = element_type_result->GetData<TypeDefinition>();
		for (int i = 0; i < size; i++) {
			auto value = GetValue<void>(i, type_table);
			auto value_string = element_type->ValueToString(type_table,
					TypeSpecifier::DefaultTypeSpecifierMap, child_indent,
					value); // array type parameters are not yet supported
			os << child_indent << "[" << i << "]";
			os
					<< element_type->GetValueSeparator(child_indent,
							TypeSpecifier::DefaultTypeSpecifierMap,
							value.get()); // array type parameters are not yet supported
			os << value_string << endl;
		}
	}

	return os.str();
}

const shared_ptr<const vector<shared_ptr<const void>>> Array::GetStorage(
		const_shared_ptr<TypeSpecifier> element_specifier,
		const int initial_size, const TypeTable& type_table) {
	auto storage = new vector<shared_ptr<const void>>(initial_size, element_specifier->DefaultValue(
					type_table));
	auto result = const_shared_ptr<const vector<shared_ptr<const void>>>(storage);
	return result;
}


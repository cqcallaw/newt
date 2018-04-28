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

#include <type_specifier.h>
#include <type_definition.h>
#include <complex_type.h>

const_shared_ptr<void> TypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto type_result = GetType(type_table, RESOLVE);

	auto errors = type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = type_result->GetData<TypeDefinition>();
		return type->GetDefaultValue(type_table,
				ComplexType::DefaultTypeSpecifierMap);
	}

	return nullptr;
}

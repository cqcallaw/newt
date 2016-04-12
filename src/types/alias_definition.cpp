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

#include <alias_definition.h>
#include <type_table.h>
#include <primitive_type_specifier.h>
#include <array_type_specifier.h>
#include <complex_type_specifier.h>
#include <symbol.h>
#include <primitive_type.h>

const_shared_ptr<void> AliasDefinition::GetDefaultValue(
		const TypeTable& type_table) const {
	if (m_default_value)
		return m_default_value;
	else {
		auto origin = GetOrigin();
		if (origin) {
			return origin->GetDefaultValue(type_table);
		} else {
			return PrimitiveTypeSpecifier::GetNone();
		}
	}
}

const_shared_ptr<TypeDefinition> AliasDefinition::GetOrigin() const {
	return m_original->GetType(*m_origin_table);
}

const_shared_ptr<Symbol> AliasDefinition::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	auto origin = GetOrigin();
	if (origin) {
		return origin->GetSymbol(type_table, type_specifier, value);
	} else {
		return Symbol::GetDefaultSymbol();
	}
}

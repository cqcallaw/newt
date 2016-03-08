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
#include <symbol.h>

const_shared_ptr<void> AliasDefinition::GetDefaultValue(
		const_shared_ptr<std::string> type_name) const {
	auto origin = GetOrigin();
	if (origin) {
		return origin->GetDefaultValue(type_name);
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const_shared_ptr<ConcreteType> AliasDefinition::GetOrigin() const {
	return m_origin_table->GetType<ConcreteType>(*m_origin_name);
}

const_shared_ptr<Symbol> AliasDefinition::GetSymbol(
		const_shared_ptr<void> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	auto origin = GetOrigin();
	if (origin) {
		return origin->GetSymbol(value, container);
	} else {
		return Symbol::GetDefaultSymbol();
	}
}

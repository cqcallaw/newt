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
		const_shared_ptr<std::string> type_name,
		const TypeTable& type_table) const {
	if (m_default_value)
		return m_default_value;
	else {
		auto origin = GetOrigin();
		if (origin) {
			return origin->GetDefaultValue(type_name, type_table);
		} else {
			return PrimitiveTypeSpecifier::GetNone();
		}
	}
}

const_shared_ptr<TypeDefinition> AliasDefinition::GetOrigin() const {
	//should probably abstract this into a typespecifier pure virtual method
//	auto subject = m_original;
//
//	auto as_array = std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
//			m_original);
//	if (as_array) {
////		shared_ptr<const TypeSpecifier> element_type =
////				as_array->GetElementTypeSpecifier();
//		do {
//			subject = as_array->GetElementTypeSpecifier();
//			as_array = std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
//					subject);
//		} while (as_array);
//	}
//
//	auto as_primitive = std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
//			subject);
//	if (as_primitive) {
//		return make_shared<PrimitiveType>(as_primitive->GetBasicType());
//	}
//
//	auto as_complex = std::dynamic_pointer_cast<const ComplexTypeSpecifier>(
//			subject);
//	if (as_complex) {
//		return m_origin_table->GetType<ConcreteType>(as_complex);
//	}
//
//	return nullptr;

	return m_original->GetType(*m_origin_table);
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

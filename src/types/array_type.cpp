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

#include <array_type.h>
#include <array_type_specifier.h>
#include <array.h>

const std::string ArrayType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	return m_member_type_specifier->ToString() + "[]";
}

const std::string ArrayType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	auto as_array = static_pointer_cast<const Array>(value);
	return as_array->ToString(type_table, indent);
}

const_shared_ptr<TypeSpecifier> ArrayType::GetTypeSpecifier(
		const_shared_ptr<std::string> name) const {
	//auto member_type_specifier = m_member_type->GetTypeSpecifier(name);
	//TODO: cache this
	auto result = make_shared<ArrayTypeSpecifier>(m_member_type_specifier);
	return result;
}

bool ArrayType::IsSpecifiedBy(const std::string& name,
		const TypeSpecifier& type_specifier) const {
	try {
		const ArrayTypeSpecifier& as_array =
				dynamic_cast<const ArrayTypeSpecifier&>(type_specifier);
		return *as_array.GetElementTypeSpecifier() == *m_member_type_specifier;
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<void> ArrayType::GetDefaultValue(
		const_shared_ptr<std::string> type_name,
		const TypeTable& type_table) const {
	return make_shared<Array>(m_member_type_specifier, type_table);
}

const_shared_ptr<Symbol> ArrayType::GetSymbol(const_shared_ptr<void> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	auto cast = static_pointer_cast<const Array>(value);
	return make_shared<Symbol>(cast);
}

const std::string ArrayType::GetValueSeperator(const Indent& indent,
		const_shared_ptr<void> value) const {
	return "\n";
}

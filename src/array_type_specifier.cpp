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

#include <array_type_specifier.h>
#include <array.h>

const string ArrayTypeSpecifier::ToString() const {
	ostringstream buffer;

	if (m_fixed_size) {
		buffer << "static ";
	} else {
		buffer << "dynamic ";
	}
	buffer << m_element_type_specifier->ToString();
	buffer << "[]";
	return buffer.str();
}

const void* ArrayTypeSpecifier::DefaultValue(
		const TypeTable* type_table) const {
	/*const void* value = nullptr;
	 const void* fill_value = m_element_type_specifier->DefaultValue(
	 type_table);

	 int initial_size = 0;
	 if (m_fixed_size) {
	 assert(false);
	 }*/

	return new Array(m_element_type_specifier, type_table);

	/*const PrimitiveTypeSpecifier* as_primitive =
	 dynamic_cast<const PrimitiveTypeSpecifier*>(m_element_type_specifier);
	 if (as_primitive) {
	 if (as_primitive == PrimitiveTypeSpecifier::GetBoolean()) {
	 value = new vector<const bool*>(initial_size,
	 (const bool*) fill_value);
	 } else if (as_primitive == PrimitiveTypeSpecifier::GetInt()) {
	 value = new vector<const int*>(initial_size,
	 (const int*) fill_value);
	 } else if (as_primitive == PrimitiveTypeSpecifier::GetDouble()) {
	 value = new vector<const double*>(initial_size,
	 (const double*) fill_value);
	 } else if (as_primitive == PrimitiveTypeSpecifier::GetString()) {
	 value = new vector<const string*>(initial_size,
	 (const string*) fill_value);
	 } else {
	 assert(false);
	 }
	 }

	 const ArrayTypeSpecifier* as_array =
	 dynamic_cast<const ArrayTypeSpecifier*>(m_element_type_specifier);
	 if (as_array) {
	 value = new vector<const Array*>(initial_size,
	 (const Array*) fill_value);
	 }

	 const CompoundTypeSpecifier* as_compound =
	 dynamic_cast<const CompoundTypeSpecifier*>(m_element_type_specifier);
	 if (as_compound) {
	 value = new vector<const CompoundTypeInstance*>(initial_size,
	 (const CompoundTypeInstance*) fill_value);
	 }

	 return value;*/
}


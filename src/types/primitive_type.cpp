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

#include <primitive_type.h>
#include <sstream>
#include <primitive_type_specifier.h>
#include <indent.h>
#include <symbol.h>

PrimitiveType::~PrimitiveType() {
}

const std::string PrimitiveType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	os << indent + 1;
	os << m_type;
	os << endl;
	return os.str();
}

const std::string PrimitiveType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	buffer
			<< Symbol::ToString(PrimitiveTypeSpecifier::FromBasicType(m_type),
					value, type_table, indent);
	return buffer.str();
}

bool PrimitiveType::IsSpecifiedBy(const std::string name,
		const TypeSpecifier& type_specifier) const {
	try {
		const PrimitiveTypeSpecifier& as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier&>(type_specifier);
		return m_type == as_primitive.GetBasicType();
	} catch (std::bad_cast& e) {
		return false;
	}
}

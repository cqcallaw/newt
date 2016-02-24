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

const_shared_ptr<void> PrimitiveType::GetDefaultValue(
		const_shared_ptr<std::string> type_name) const {
	return GetDefaultValue(m_type);
}

const_shared_ptr<Symbol> PrimitiveType::GetSymbol(const_shared_ptr<void> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	switch (m_type) {
	case BasicType::BOOLEAN:
		return make_shared<Symbol>(static_pointer_cast<const bool>(value));
	case BasicType::INT:
		return make_shared<Symbol>(static_pointer_cast<const int>(value));
	case BasicType::DOUBLE:
		return make_shared<Symbol>(static_pointer_cast<const double>(value));
	case BasicType::STRING:
		return make_shared<Symbol>(static_pointer_cast<const string>(value));
	default:
		assert(false);
		return nullptr;
	}
}

const_shared_ptr<void> PrimitiveType::GetDefaultValue(
		const BasicType& basic_type) {
	switch (basic_type) {
	case BasicType::BOOLEAN:
		static const_shared_ptr<void> default_boolean_value = const_shared_ptr<
				void>(new bool(false));
		return default_boolean_value;
	case BasicType::INT:
		static const_shared_ptr<void> default_int_value =
				const_shared_ptr<void>(new int(0));
		return default_int_value;
	case BasicType::DOUBLE:
		static const_shared_ptr<void> default_double_value = const_shared_ptr<
				void>(new double(0.0));
		return default_double_value;
	case BasicType::STRING:
		static const_shared_ptr<void> default_string_value = const_shared_ptr<
				void>(new string(""));
		return default_string_value;
	default:
		assert(false);
		return nullptr;
	}
}

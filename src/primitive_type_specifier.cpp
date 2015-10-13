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

#include <primitive_type_specifier.h>
#include <typeinfo>

const string PrimitiveTypeSpecifier::ToString(const void* value) const {
	ostringstream buffer;
	const BasicType type = GetBasicType();
	switch (type) {
	case BasicType::BOOLEAN:
	case BasicType::INT: {
		const int* default_value = (int*) value;
		buffer << *default_value;
		break;
	}
	case BasicType::DOUBLE: {
		const double* default_value = (double*) value;
		buffer << *default_value;
		break;
	}
	case BasicType::STRING: {
		const string* default_value = (string*) value;
		buffer << "\"" << *default_value << "\"";
		break;
	}
	default:
		assert(false);
	}

	return buffer.str();
}

const bool PrimitiveTypeSpecifier::IsAssignableTo(
		const TypeSpecifier* other) const {
	const PrimitiveTypeSpecifier* other_as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(other);
	if (other_as_primitive != nullptr) {
		const BasicType other_type = other_as_primitive->GetBasicType();
		return other_type != BasicType::NONE && m_basic_type <= other_type;
	}
	return false;
}

const void* PrimitiveTypeSpecifier::DefaultValue(
		const TypeTable* type_table) const {
	const BasicType basic_type = GetBasicType();

	switch (basic_type) {
	case BasicType::BOOLEAN:
		return DefaultBooleanValue;
	case BasicType::INT:
		return DefaultIntValue;
	case BasicType::DOUBLE:
		return DefaultDoubleValue;
	case BasicType::STRING:
		return DefaultStringValue;
	default:
		assert(false);
		return nullptr;
	}
}

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::GetNone() {
	const static PrimitiveTypeSpecifier* instance = new PrimitiveTypeSpecifier(
			BasicType::NONE);
	return instance;
}

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::GetBoolean() {
	const static PrimitiveTypeSpecifier* instance = new PrimitiveTypeSpecifier(
			BasicType::BOOLEAN);
	return instance;
}

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::GetInt() {
	const static PrimitiveTypeSpecifier* instance = new PrimitiveTypeSpecifier(
			BasicType::INT);
	return instance;
}

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::GetDouble() {
	const static PrimitiveTypeSpecifier* instance = new PrimitiveTypeSpecifier(
			BasicType::DOUBLE);
	return instance;
}

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::GetString() {
	const static PrimitiveTypeSpecifier* instance = new PrimitiveTypeSpecifier(
			BasicType::STRING);
	return instance;
}

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::FromBasicType(
		BasicType type) {
	switch (type) {
	case BOOLEAN:
		return GetBoolean();
	case INT:
		return GetInt();
	case DOUBLE:
		return GetDouble();
	case STRING:
		return GetString();
	default:
		return GetNone();
	}
}

bool PrimitiveTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const PrimitiveTypeSpecifier& as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier&>(other);
		return GetBasicType() == as_primitive.GetBasicType();
	} catch (std::bad_cast& e) {
		return false;
	}
}

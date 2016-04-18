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

#include <primitive_declaration_statement.h>
#include <primitive_type_specifier.h>
#include <nested_type_specifier.h>
#include <primitive_type.h>
#include <complex_type.h>
#include <typeinfo>
#include <expression.h>
#include <memory>

const string PrimitiveTypeSpecifier::ToString(
		const_shared_ptr<void> value) const {
	ostringstream buffer;
	const BasicType type = GetBasicType();
	switch (type) {
	case BasicType::BOOLEAN:
	case BasicType::INT: {
		const_shared_ptr<int> default_value = static_pointer_cast<const int>(
				value);
		buffer << *default_value;
		break;
	}
	case BasicType::DOUBLE: {
		const_shared_ptr<double> default_value = static_pointer_cast<
				const double>(value);
		buffer << *default_value;
		break;
	}
	case BasicType::STRING: {
		const_shared_ptr<string> default_value = static_pointer_cast<
				const string>(value);
		buffer << "\"" << *default_value << "\"";
		break;
	}
	default:
		assert(false);
	}

	return buffer.str();
}

const bool PrimitiveTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto resolved = NestedTypeSpecifier::Resolve(other, type_table);
	if (resolved->AnalyzeConversion(type_table, *this) == UNAMBIGUOUS) {
		return true;
	}

	auto other_as_primitive =
			dynamic_pointer_cast<const PrimitiveTypeSpecifier>(resolved);
	if (other_as_primitive) {
		const BasicType other_type = other_as_primitive->GetBasicType();
		return other_type != BasicType::NONE && m_basic_type <= other_type;
	}

	return false;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::GetNone() {
	static const_shared_ptr<PrimitiveTypeSpecifier> instance = const_shared_ptr<
			PrimitiveTypeSpecifier>(
			new PrimitiveTypeSpecifier(BasicType::NONE));
	return instance;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::GetBoolean() {
	static const_shared_ptr<PrimitiveTypeSpecifier> instance = const_shared_ptr<
			PrimitiveTypeSpecifier>(
			new PrimitiveTypeSpecifier(BasicType::BOOLEAN));
	return instance;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::GetInt() {
	static const_shared_ptr<PrimitiveTypeSpecifier> instance = const_shared_ptr<
			PrimitiveTypeSpecifier>(new PrimitiveTypeSpecifier(BasicType::INT));
	return instance;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::GetDouble() {
	static const_shared_ptr<PrimitiveTypeSpecifier> instance = const_shared_ptr<
			PrimitiveTypeSpecifier>(
			new PrimitiveTypeSpecifier(BasicType::DOUBLE));
	return instance;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::GetString() {
	static const_shared_ptr<PrimitiveTypeSpecifier> instance = const_shared_ptr<
			PrimitiveTypeSpecifier>(
			new PrimitiveTypeSpecifier(BasicType::STRING));
	return instance;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::FromBasicType(
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

const_shared_ptr<TypeDefinition> PrimitiveTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto basic_type = GetBasicType();
	if (basic_type != NONE) {
		//TODO: cache these, or populate the default type table with them
		return make_shared<PrimitiveType>(basic_type);
	}

	return nullptr;
}

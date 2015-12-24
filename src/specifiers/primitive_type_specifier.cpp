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
#include <primitive_declaration_statement.h>
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
		const_shared_ptr<TypeSpecifier> other) const {
	const_shared_ptr<PrimitiveTypeSpecifier> other_as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(other);
	if (other_as_primitive != nullptr) {
		const BasicType other_type = other_as_primitive->GetBasicType();
		return other_type != BasicType::NONE && m_basic_type <= other_type;
	}
	return false;
}

const_shared_ptr<void> PrimitiveTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	const BasicType basic_type = GetBasicType();

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

const_shared_ptr<DeclarationStatement> PrimitiveTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<PrimitiveDeclarationStatement>(position, type,
			type_position, name, name_position, initializer_expression);
}

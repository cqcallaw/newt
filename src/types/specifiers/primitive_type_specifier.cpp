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
#include <iomanip>

const string PrimitiveTypeSpecifier::ToString(
		const_shared_ptr<void> value) const {
	ostringstream buffer;
	const BasicType type = GetBasicType();
	switch (type) {
	case BasicType::BOOLEAN: {
		const_shared_ptr<bool> cast_value = static_pointer_cast<const bool>(
				value);
		if (*cast_value == true) {
			buffer << "true";
		} else {
			buffer << "false";
		}
		break;
	}
	case BasicType::BYTE: {
		const_shared_ptr<std::uint8_t> cast_value = static_pointer_cast<
				const std::uint8_t>(value);
		buffer << "0x" << std::setfill('0') << std::setw(2) << std::hex
				<< uppercase << unsigned(*cast_value);
		break;
	}
	case BasicType::INT: {
		const_shared_ptr<int> cast_value = static_pointer_cast<const int>(
				value);
		buffer << *cast_value;
		break;
	}
	case BasicType::DOUBLE: {
		const_shared_ptr<double> cast_value = static_pointer_cast<const double>(
				value);
		buffer << *cast_value;
		break;
	}
	case BasicType::STRING: {
		const_shared_ptr<string> cast_value = static_pointer_cast<const string>(
				value);
		buffer << "\"" << *cast_value << "\"";
		break;
	}
	default:
		assert(false);
	}

	return buffer.str();
}

const AnalysisResult PrimitiveTypeSpecifier::AnalyzeAssignmentTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	auto resolved_other_result = NestedTypeSpecifier::Resolve(other,
			type_table);

	if (ErrorList::IsTerminator(resolved_other_result.GetErrors())) {
		auto resolved_other = resolved_other_result.GetData();

		auto widening_analysis = resolved_other->AnalyzeWidening(type_table,
				*this);
		if (widening_analysis != AnalysisResult::INCOMPATIBLE) {
			return widening_analysis;
		}

		auto other_as_primitive = dynamic_pointer_cast<
				const PrimitiveTypeSpecifier>(resolved_other);
		if (other_as_primitive) {
			const BasicType other_type = other_as_primitive->GetBasicType();
			if (other_type != BasicType::NONE) {
				if (m_basic_type == other_type) {
					return AnalysisResult::EQUIVALENT;
				} else if (m_basic_type < other_type) {
					return AnalysisResult::UNAMBIGUOUS;
				}
			}
		}
	}

	return INCOMPATIBLE;
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

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::GetByte() {
	static const_shared_ptr<PrimitiveTypeSpecifier> instance = const_shared_ptr<
			PrimitiveTypeSpecifier>(
			new PrimitiveTypeSpecifier(BasicType::BYTE));
	return instance;
}

const_shared_ptr<PrimitiveTypeSpecifier> PrimitiveTypeSpecifier::FromBasicType(
		BasicType type) {
	switch (type) {
	case BOOLEAN:
		return GetBoolean();
	case BYTE:
		return GetByte();
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

const_shared_ptr<Result> PrimitiveTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	auto basic_type = GetBasicType();
	auto result_type = PrimitiveType::FromBasicType(basic_type);
	return make_shared<Result>(result_type, ErrorList::GetTerminator());
}

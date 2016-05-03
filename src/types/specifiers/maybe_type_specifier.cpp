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

#include <maybe_type_specifier.h>
#include <sum_type.h>

const_shared_ptr<std::string> MaybeTypeSpecifier::VARIANT_NAME = make_shared<
		std::string>("value");
const_shared_ptr<std::string> MaybeTypeSpecifier::EMPTY_NAME = make_shared<
		std::string>("empty");

MaybeTypeSpecifier::MaybeTypeSpecifier(
		const_shared_ptr<TypeSpecifier> type_specifier) :
		MaybeTypeSpecifier(type_specifier, nullptr) {
}

MaybeTypeSpecifier::MaybeTypeSpecifier(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<SumType> type) :
		m_type_specifier(type_specifier), m_type(type) {
}

MaybeTypeSpecifier::~MaybeTypeSpecifier() {
}

const std::string MaybeTypeSpecifier::ToString() const {
	return m_type_specifier->ToString() + "?";
}

const bool MaybeTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other,
		const TypeTable& type_table) const {
	return *this == *other
			|| m_type_specifier->IsAssignableTo(other, type_table)
			|| TypeTable::GetNilTypeSpecifier()->IsAssignableTo(other,
					type_table);
}

bool MaybeTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const MaybeTypeSpecifier& as_maybe =
				dynamic_cast<const MaybeTypeSpecifier&>(other);
		return *GetTypeSpecifier() == *as_maybe.GetTypeSpecifier();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const_shared_ptr<TypeDefinition> MaybeTypeSpecifier::GetType(
		const TypeTable& type_table, AliasResolution resolution) const {
	return m_type;
}

const ConversionResult MaybeTypeSpecifier::AnalyzeConversion(
		const TypeTable& type_table, const TypeSpecifier& other) const {
	if (m_type) {
		auto placeholder = ComplexTypeSpecifier(
				make_shared<std::string>(ToString()));
		return m_type->AnalyzeConversion(placeholder, other);
	}
	return INCOMPATIBLE;
}

const_shared_ptr<void> MaybeTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	return const_shared_ptr<void>();
}

const_shared_ptr<std::string> MaybeTypeSpecifier::MapSpecifierToVariant(
		const TypeSpecifier& type_specifier,
		const TypeTable& type_table) const {
	if (type_specifier.IsAssignableTo(m_type_specifier, type_table)) {
		return VARIANT_NAME;
	} else if (type_specifier.IsAssignableTo(TypeTable::GetNilTypeSpecifier(),
			type_table)) {
		return EMPTY_NAME;
	}
	return const_shared_ptr<std::string>();
}

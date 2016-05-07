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
#include <maybe_type.h>
#include <unit_type.h>
#include <sum.h>
#include <maybe_declaration_statement.h>

MaybeType::MaybeType(const_shared_ptr<TypeSpecifier> base_type_specifier) :
		m_base_type_specifier(base_type_specifier) {
}

MaybeType::~MaybeType() {
}

const_shared_ptr<void> MaybeType::GetDefaultValue(
		const TypeTable& type_table) const {
	auto value = TypeTable::GetNilType()->GetDefaultValue(type_table);
	return make_shared<Sum>(MaybeTypeSpecifier::EMPTY_NAME, value);
}

const std::string MaybeType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	return (indent + 1).ToString() + m_base_type_specifier->ToString() + "?";
}

const std::string MaybeType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	auto sum_instance = static_pointer_cast<const Sum>(value);
	//buffer << sum_instance->ToString(*this, *m_definition, indent);
	auto tag = sum_instance->GetTag();
	buffer << "{" << *tag << "}";
	if (*tag == *MaybeTypeSpecifier::EMPTY_NAME) {
	} else if (*tag == *MaybeTypeSpecifier::VARIANT_NAME) {
		auto type = m_base_type_specifier->GetType(type_table, RESOLVE);
		if (type) {
			auto inner_value = sum_instance->GetValue();
			buffer << type->GetValueSeparator(indent, inner_value.get());
			buffer << type->ValueToString(type_table, indent + 1, inner_value);
		} else {
			buffer << "<undefined type>";
		}
	} else {
		buffer << "<undefined variant>";
	}
	return buffer.str();
}

const std::string MaybeType::GetValueSeparator(const Indent& indent,
		const void* value) const {
	ostringstream buffer;
	auto sum_instance = static_cast<const Sum*>(value);

	auto tag = sum_instance->GetTag();
	if (*tag == *MaybeTypeSpecifier::VARIANT_NAME) {
		buffer << "\n" << indent + 1;
	} else if (*tag == *MaybeTypeSpecifier::EMPTY_NAME) {
		buffer << " ";
	}
	return buffer.str();
}

const_shared_ptr<TypeSpecifier> MaybeType::GetTypeSpecifier(
		const_shared_ptr<std::string> name,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	return make_shared<MaybeTypeSpecifier>(m_base_type_specifier);
}

const_shared_ptr<Symbol> MaybeType::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	auto cast = static_pointer_cast<const Sum>(value);
	auto specifier = make_shared<MaybeTypeSpecifier>(m_base_type_specifier);
	return make_shared<Symbol>(specifier, cast);
}

const std::string MaybeType::GetTagSeparator(const Indent& indent,
		const void* value) const {
	return "";
}

const_shared_ptr<DeclarationStatement> MaybeType::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<std::string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<MaybeDeclarationStatement>(position,
			static_pointer_cast<const MaybeTypeSpecifier>(type), type_position,
			name, name_position);
}

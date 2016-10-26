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

#include <alias_definition.h>
#include <type_table.h>
#include <primitive_type_specifier.h>
#include <array_type_specifier.h>
#include <record_type.h>
#include <symbol.h>
#include <primitive_type.h>

AliasDefinition::AliasDefinition(const_shared_ptr<TypeTable> origin_table,
		const_shared_ptr<TypeSpecifier> original, const AliasType alias_type,
		const_shared_ptr<void> default_value) :
		m_origin_table(origin_table), m_original(original), m_alias_type(
				alias_type), m_default_value(default_value) {
}

const_shared_ptr<void> AliasDefinition::GetDefaultValue(
		const TypeTable& type_table) const {
	if (m_default_value)
		return m_default_value;
	else {
		auto origin = GetOrigin();
		if (origin && m_alias_type == DIRECT) {
			return origin->GetDefaultValue(type_table);
		} else {
			return const_shared_ptr<void>();
		}
	}
}

const std::string AliasDefinition::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	auto origin = GetOrigin();
	if (origin && m_alias_type == DIRECT) {
		return origin->ValueToString(type_table, indent, value);
	} else {
		return "<No origin found for alias '" + m_original->ToString() + "'>";
	}
}

const std::string AliasDefinition::GetValueSeparator(const Indent& indent,
		const void* value) const {
	auto origin = GetOrigin();
	if (origin) {
		return origin->GetValueSeparator(indent, value);
	} else {
		return "<No origin found for alias '" + m_original->ToString() + "'>";
	}
}

const std::string AliasDefinition::GetTagSeparator(const Indent& indent,
		const void* value) const {
	auto origin = GetOrigin();
	if (origin) {
		return origin->GetValueSeparator(indent, value);
	} else {
		return "<No origin found for alias '" + m_original->ToString() + "'>";
	}
}

const_shared_ptr<TypeDefinition> AliasDefinition::GetOrigin() const {
	const_shared_ptr<TypeTable> ptr = m_origin_table.lock();
	assert(ptr);
	auto type_result = m_original->GetType(*ptr);

	if (ErrorList::IsTerminator(type_result->GetErrors())) {
		return type_result->GetData<TypeDefinition>();
	}

	return nullptr;
}

const_shared_ptr<Symbol> AliasDefinition::GetSymbol(const TypeTable& type_table,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value) const {
	auto origin = GetOrigin();
	if (origin) {
		return origin->GetSymbol(type_table, type_specifier, value);
	} else {
		return Symbol::GetDefaultSymbol();
	}
}

const std::string AliasDefinition::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	os << child_indent;
	os << m_original->ToString();

	if (m_default_value) {
		string default_value = ValueToString(type_table, child_indent,
				m_default_value);
		os << " (";

		auto origin = GetOrigin();
		auto as_record = dynamic_pointer_cast<const RecordType>(origin);
		if (as_record) {
			os << endl;
		}

		os << default_value;

		if (as_record) {
			os << child_indent;
		}
		os << ")";
	}

	return os.str();
}

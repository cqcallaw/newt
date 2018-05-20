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

#include <sum.h>
#include <sstream>
#include <indent.h>
#include <symbol.h>
#include <sum_type.h>
#include <record_type.h>
#include <unit_type.h>

Sum::Sum(const_shared_ptr<std::string> tag, const_shared_ptr<void> value) :
		m_tag(tag), m_value(value) {
	assert(m_tag && *m_tag != "");
	assert(m_value);
}

Sum::~Sum() {
}

const string Sum::ToString(const SumType& type, const TypeTable& type_table,
		const_shared_ptr<type_specifier_map> type_specifier_mapping,
		const Indent& indent) const {
	ostringstream buffer;
	auto type_definition = type.GetDefinition();
	auto variant_definition = type_definition->GetType<TypeDefinition>(*m_tag,
			SHALLOW, RETURN);
	buffer
			<< variant_definition->GetTagSeparator(indent,
					type_specifier_mapping, m_value.get());
	buffer << "{" << *m_tag << "}";
	buffer
			<< variant_definition->GetValueSeparator(indent,
					type_specifier_mapping, m_value.get());
	buffer
			<< variant_definition->ValueToString(type_table,
					type_specifier_mapping, indent, m_value);
	return buffer.str();
}

const_shared_ptr<Sum> Sum::GetDefaultInstance(const SumType& type,
		const_shared_ptr<type_specifier_map> type_specifier_mapping) {
	auto type_table = type.GetDefinition();
	auto variant_name = type.GetFirstVariantName();
	auto variant_type = type_table->GetType<TypeDefinition>(variant_name,
			SHALLOW, RETURN);
	assert(variant_type);
	auto default_value = variant_type->GetDefaultValue(*type_table,
			type_specifier_mapping);
	auto result = make_shared<Sum>(variant_name, default_value);
	return result;
}

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

Sum::Sum(const_shared_ptr<std::string> tag, const_shared_ptr<void> value) :
		m_tag(tag), m_value(value) {
	assert(m_tag && *m_tag != "");
	assert(m_value);
}

Sum::~Sum() {
}

const string Sum::ToString(const SumType& type, const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	auto type_definition = type.GetDefinition();
	auto variant_definition = type_definition->GetType<TypeDefinition>(*m_tag);
	buffer << variant_definition->ValueToString(type_table, indent, m_value);
	buffer << " {" << *m_tag << "}";
	return buffer.str();
}

const_shared_ptr<Sum> Sum::GetDefaultInstance(const SumType& type) {
	auto declaration = type.GetFirstDeclaration();
	return make_shared<Sum>(declaration->GetName(),
			declaration->GetTypeSpecifier()->DefaultValue(*type.GetDefinition()));
}

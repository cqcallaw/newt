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

#include <compound_type.h>
#include <member_definition.h>
#include <sstream>
#include <assert.h>
#include <type_specifier.h>

CompoundType::CompoundType(
		const map<const string, const MemberDefinition*>* definition,
		const Modifier::Type modifiers) :
		m_definition(definition), m_modifiers(modifiers) {
}

const MemberDefinition* CompoundType::GetMember(const string name) const {
	auto result = m_definition->find(name);
	if (result != m_definition->end()) {
		return result->second;
	} else {
		return MemberDefinition::GetDefaultMemberDefinition();
	}
}

CompoundType::~CompoundType() {
}

const string CompoundType::ToString(const TypeTable* type_table,
		const Indent indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	map<const string, const MemberDefinition*>::const_iterator type_iter;
	for (type_iter = m_definition->begin(); type_iter != m_definition->end();
			++type_iter) {
		const string member_name = type_iter->first;
		const MemberDefinition* member_definition = type_iter->second;
		const TypeSpecifier* member_type = member_definition->GetType();

		os << child_indent << member_type->ToString() << " " << member_name
				<< " (" << member_definition->ToString(type_table, child_indent)
				<< ")";

		os << endl;
	}
	return os.str();
}

const CompoundType* CompoundType::GetDefaultCompoundType() {
	const static CompoundType* instance = new CompoundType(
			new map<const string, const MemberDefinition*>(), Modifier::NONE);
	return instance;
}

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

const CompoundType* CompoundType::DefaultCompoundType = new CompoundType(
		new map<const string, const MemberDefinition*>());

CompoundType::CompoundType(
		const map<const string, const MemberDefinition*>* definition) :
		m_definition(definition) {
}

const MemberDefinition* CompoundType::GetMember(const string name) const {
	return m_definition->at(name);
}

CompoundType::~CompoundType() {
}

const string CompoundType::ToString() const {
	ostringstream os;
	map<const string, const MemberDefinition*>::const_iterator type_iter;
	for (type_iter = m_definition->begin(); type_iter != m_definition->end();
			++type_iter) {
		const string member_name = type_iter->first;
		const MemberDefinition* member_definition = type_iter->second;
		const BasicType member_type = member_definition->GetType();
		const void* member_default_value = member_definition->GetDefaultValue();

		os << "\t" << member_type << " " << member_name << " ("
				<< AsString(member_type, member_default_value) << ")" << endl;
	}
	return os.str();
}

std::ostream& operator <<(std::ostream& os, const CompoundType& type) {
	os << type.ToString();
	return os;
}

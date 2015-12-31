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

#ifndef COMPOUND_TYPE_H_
#define COMPOUND_TYPE_H_

#include <map>
#include <string>
#include <linked_list.h>
#include <symbol_context.h>
#include <modifier.h>

class MemberDefinition;

typedef map<const string, const_shared_ptr<MemberDefinition>> definition_map;

using namespace std;
class CompoundType {
public:
	CompoundType(const_shared_ptr<definition_map> definition,
			const Modifier::Type modifiers);
	virtual ~CompoundType();
	const_shared_ptr<MemberDefinition> GetMember(const string name) const;

	const string ToString(const TypeTable& type_table,
			const Indent indent) const;

	const_shared_ptr<definition_map> GetDefinition() const {
		return m_definition;
	}

	static const_shared_ptr<CompoundType> GetDefaultCompoundType();

	const Modifier::Type GetModifiers() const {
		return m_modifiers;
	}

private:
	const_shared_ptr<definition_map> m_definition;
	const Modifier::Type m_modifiers;
};

#endif /* COMPOUND_TYPE_H_ */

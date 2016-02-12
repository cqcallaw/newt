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

#ifndef ALIAS_DEFINITION_H_
#define ALIAS_DEFINITION_H_

#include <type_definition.h>

class AliasDefinition: public TypeDefinition {
public:
	AliasDefinition(const_shared_ptr<TypeDefinition> origin) :
			m_origin(origin) {
	}
	virtual ~AliasDefinition() {
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		return m_origin->ToString(type_table, indent);
	}

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const {
		return m_origin->ValueToString(type_table, indent, value);
	}

	virtual bool IsSpecifiedBy(const std::string name,
			const TypeSpecifier& type_specifier) const {
		return m_origin->IsSpecifiedBy(name, type_specifier);
	}
private:
	const_shared_ptr<TypeDefinition> m_origin;
};

#endif /* ALIAS_DEFINITION_H_ */

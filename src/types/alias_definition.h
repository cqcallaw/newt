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

#ifndef TYPES_ALIAS_DEFINITION_H_
#define TYPES_ALIAS_DEFINITION_H_

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

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const {
		return m_origin->IsSpecifiedBy(name, type_specifier);
	}

	virtual const_shared_ptr<void> GetDefaultValue(
			const_shared_ptr<std::string> type_name) const {
		return m_origin->GetDefaultValue(type_name);
	}

	virtual const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<void> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const {
		return m_origin->GetSymbol(value, container);
	}

	const_shared_ptr<TypeDefinition> GetOrigin() const {
		return m_origin;
	}

private:
	const_shared_ptr<TypeDefinition> m_origin;
};

#endif /* TYPES_ALIAS_DEFINITION_H_ */

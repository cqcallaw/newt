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

#include <concrete_type.h>

class AliasDefinition: public ConcreteType {
public:
	AliasDefinition(const_shared_ptr<TypeTable> origin_table,
			const_shared_ptr<std::string> origin_name) :
			m_origin_table(origin_table), m_origin_name(origin_name) {
	}

	virtual ~AliasDefinition() {
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->ToString(type_table, indent);
		} else {
			return "<No origin found for alias '" + *m_origin_name + "'";
		}
	}

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->ValueToString(type_table, indent, value);
		} else {
			return "<No origin found for alias '" + *m_origin_name + "'";
		}
	}

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->IsSpecifiedBy(name, type_specifier);
		} else {
			return false;
		}
	}

	virtual const_shared_ptr<void> GetDefaultValue(
			const_shared_ptr<std::string> type_name) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<void> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const;

	const_shared_ptr<TypeTable> getOriginTable() const {
		return m_origin_table;
	}

	const_shared_ptr<std::string> GetOriginName() const {
		return m_origin_name;
	}

	const_shared_ptr<ConcreteType> GetOrigin() const;

private:
	const_shared_ptr<TypeTable> m_origin_table;
	const_shared_ptr<std::string> m_origin_name;
};

#endif /* TYPES_ALIAS_DEFINITION_H_ */

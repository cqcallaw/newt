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

#ifndef PRIMITIVE_TYPE_H_
#define PRIMITIVE_TYPE_H_

#include <type_definition.h>
#include <type.h>

class PrimitiveType: public TypeDefinition {
public:
	PrimitiveType(const BasicType type) :
			m_type(type) {
	}
	virtual ~PrimitiveType();

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual bool IsSpecifiedBy(const std::string name,
			const TypeSpecifier& type_specifier) const;

	virtual const_shared_ptr<void> GetDefaultValue(
			const std::string& type_name) const;

	virtual const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<void> value) const;

	const BasicType GetType() const {
		return m_type;
	}

	static const_shared_ptr<void> GetDefaultValue(const BasicType& basic_type);

private:
	const BasicType m_type;
};

#endif /* PRIMITIVE_TYPE_H_ */

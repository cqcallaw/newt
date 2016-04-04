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

#ifndef TYPES_ARRAY_TYPE_H_
#define TYPES_ARRAY_TYPE_H_

#include <concrete_type.h>

class ArrayType: public ConcreteType {
public:
	ArrayType(const_shared_ptr<TypeSpecifier> member_type_specifier) :
			m_member_type_specifier(member_type_specifier) {
	}
	virtual ~ArrayType() {
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual const std::string GetValueSeperator(const Indent& indent,
			const_shared_ptr<void> value) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name) const;

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const;

	virtual const_shared_ptr<void> GetDefaultValue(
			const_shared_ptr<std::string> type_name,
			const TypeTable& type_table) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<void> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const;

	const const_shared_ptr<TypeSpecifier> GetMemberTypeSpecifier() const {
		return m_member_type_specifier;
	}

private:
	const_shared_ptr<TypeSpecifier> m_member_type_specifier;
};

#endif /* TYPES_ARRAY_TYPE_H_ */

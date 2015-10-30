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

#ifndef SPECIFIERS_COMPOUND_TYPE_SPECIFIER_H_
#define SPECIFIERS_COMPOUND_TYPE_SPECIFIER_H_

#include <compound_type_instance.h>
#include <type_specifier.h>
#include <type_table.h>

class SymbolContext;

class CompoundTypeSpecifier: public TypeSpecifier {
public:
	CompoundTypeSpecifier(const string type_name, const YYLTYPE location) :
			m_type_name(type_name), m_location(location) {
	}
	virtual ~CompoundTypeSpecifier() {
	}

	const string GetTypeName() const {
		return m_type_name;
	}

	virtual const string ToString() const {
		return m_type_name;
	}

	virtual const bool IsAssignableTo(const TypeSpecifier* other) const {
		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(other);
		return as_compound != nullptr
				&& as_compound->GetTypeName().compare(m_type_name) == 0;
	}

	virtual const void* DefaultValue(const TypeTable* type_table) const {
		return type_table->GetDefaultValue(m_type_name);
	}

	virtual bool operator==(const TypeSpecifier& other) const;

	virtual bool operator!=(const TypeSpecifier &other) const {
		return !(*this == other);
	}

	const YYLTYPE GetLocation() const {
		return m_location;
	}

private:
	const string m_type_name;
	const YYLTYPE m_location;
};

#endif /* SPECIFIERS_COMPOUND_TYPE_SPECIFIER_H_ */

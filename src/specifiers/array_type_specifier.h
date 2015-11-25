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

#ifndef ARRAY_TYPE_SPECIFIER_H_
#define ARRAY_TYPE_SPECIFIER_H_

#include <type_specifier.h>

class Expression;
class Statement;

using namespace std;
class ArrayTypeSpecifier: public TypeSpecifier {
public:
	ArrayTypeSpecifier(const TypeSpecifier* element_type_specifier) :
			m_element_type_specifier(element_type_specifier) {
	}

	virtual ~ArrayTypeSpecifier() {
	}

	virtual const string ToString() const;

	virtual const bool IsAssignableTo(const TypeSpecifier* other) const {
		const ArrayTypeSpecifier* as_array =
				dynamic_cast<const ArrayTypeSpecifier*>(other);
		return as_array != nullptr
				&& m_element_type_specifier->IsAssignableTo(
						as_array->GetElementTypeSpecifier());
	}

	virtual const void* DefaultValue(const TypeTable* type_table) const;

	virtual bool operator==(const TypeSpecifier& other) const;

	virtual bool operator!=(const TypeSpecifier &other) const {
		return !(*this == other);
	}

	const TypeSpecifier* GetElementTypeSpecifier() const {
		return m_element_type_specifier;
	}

	virtual const DeclarationStatement* GetDeclarationStatement(
			const YYLTYPE position, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* initializer_expression) const;

private:
	const TypeSpecifier* m_element_type_specifier;
};

#endif /* ARRAY_TYPE_SPECIFIER_H_ */

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

#include <specifiers/type_specifier.h>

class Expression;
class Statement;

using namespace std;
class ArrayTypeSpecifier: public TypeSpecifier {
public:
	ArrayTypeSpecifier(const_shared_ptr<TypeSpecifier> element_type_specifier) :
			m_element_type_specifier(element_type_specifier) {
	}

	virtual ~ArrayTypeSpecifier() {
	}

	virtual const string ToString() const;

	virtual const bool IsAssignableTo(
			const_shared_ptr<TypeSpecifier> other) const;

	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier& other) const;

	const_shared_ptr<TypeSpecifier> GetElementTypeSpecifier() const {
		return m_element_type_specifier;
	}

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<void> value) const;

private:
	const_shared_ptr<TypeSpecifier> m_element_type_specifier;
};

#endif /* ARRAY_TYPE_SPECIFIER_H_ */

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

class Expression;
class Statement;
class SymbolContext;

class CompoundTypeSpecifier: public TypeSpecifier {
public:
	CompoundTypeSpecifier(const string& type_name) :
			m_type_name(type_name) {
	}
	virtual ~CompoundTypeSpecifier() {
	}

	const string GetTypeName() const {
		return m_type_name;
	}

	virtual const string ToString() const {
		return m_type_name;
	}

	virtual const bool IsAssignableTo(
			const_shared_ptr<TypeSpecifier> other) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const {
		return type_table.GetDefaultValue(m_type_name); //this result cannot be cached because the type table is mutable
	}

	virtual bool operator==(const TypeSpecifier& other) const;

private:
	const string m_type_name;
};

#endif /* SPECIFIERS_COMPOUND_TYPE_SPECIFIER_H_ */

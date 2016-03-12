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

#ifndef SPECIFIERS_NESTED_TYPE_SPECIFIER_H_
#define SPECIFIERS_NESTED_TYPE_SPECIFIER_H_

#include <specifiers/type_specifier.h>

class ComplexTypeSpecifier;

class NestedTypeSpecifier: public TypeSpecifier {
public:
	NestedTypeSpecifier(const_shared_ptr<ComplexTypeSpecifier> parent,
			const_shared_ptr<std::string> member_name);
	virtual ~NestedTypeSpecifier();

	virtual const std::string ToString() const;
	virtual const bool IsAssignableTo(
			const_shared_ptr<TypeSpecifier> other) const;
	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<void> value) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	const_shared_ptr<ComplexTypeSpecifier> GetParent() const {
		return m_parent;
	}

	const_shared_ptr<std::string> GetMemberName() const {
		return m_member_name;
	}

private:
	const_shared_ptr<ComplexTypeSpecifier> m_parent;
	const_shared_ptr<std::string> m_member_name;
};

#endif /* SPECIFIERS_NESTED_TYPE_SPECIFIER_H_ */

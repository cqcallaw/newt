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

#ifndef SPECIFIERS_SUM_TYPE_SPECIFIER_H_
#define SPECIFIERS_SUM_TYPE_SPECIFIER_H_

#include <type_specifier.h>

enum ComparisonMode {
	ALLOW_WIDENING, STRICT
};

class SumTypeSpecifier: public TypeSpecifier {
public:
	SumTypeSpecifier(const TypeSpecifierListRef types);
	virtual ~SumTypeSpecifier();

	const const_shared_ptr<TypeSpecifier> GetDefaultMember() const {
		return m_types->GetData();
	}

	virtual const string ToString() const;
	virtual const bool IsAssignableTo(
			const_shared_ptr<TypeSpecifier> other) const;
	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	const bool ContainsType(const TypeSpecifier &other,
			ComparisonMode mode) const;

private:
	const TypeSpecifierListRef m_types;
};

#endif /* SPECIFIERS_SUM_TYPE_SPECIFIER_H_ */

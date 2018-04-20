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
class ArrayType;

using namespace std;
class ArrayTypeSpecifier: public TypeSpecifier {
public:
	ArrayTypeSpecifier(const_shared_ptr<TypeSpecifier> element_type_specifier);

	virtual ~ArrayTypeSpecifier();

	virtual const string ToString() const;

	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier& other) const;

	virtual const ErrorListRef ValidateDeclaration(const TypeTable& type_table,
			const TypeSpecifierListRef type_parameter_list,
			const yy::location position) const;

	const_shared_ptr<TypeSpecifier> GetElementTypeSpecifier() const {
		return m_element_type_specifier;
	}

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<TypeSpecifier> WithTypeArgumentList(
			TypeSpecifierListRef type_arguments) const;

private:
	const_shared_ptr<TypeSpecifier> m_element_type_specifier;
	const_shared_ptr<ArrayType> m_type;
};

#endif /* ARRAY_TYPE_SPECIFIER_H_ */

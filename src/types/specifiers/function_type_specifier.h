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

#ifndef FUNCTION_TYPE_SPECIFIER_H_
#define FUNCTION_TYPE_SPECIFIER_H_

#include <specifiers/type_specifier.h>

class Expression;
class Statement;
class Function;

class FunctionTypeSpecifier: public TypeSpecifier {
public:
	FunctionTypeSpecifier(TypeSpecifierListRef parameter_type_list,
			const_shared_ptr<TypeSpecifier> return_type,
			const yy::location return_type_location,
			const yy::location location = GetDefaultLocation());
	FunctionTypeSpecifier(const FunctionTypeSpecifier& other);
	virtual ~FunctionTypeSpecifier();

	virtual const string ToString() const;
	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual const_shared_ptr<TypeDefinition> GetType(
			const TypeTable& type_table, AliasResolution resolution =
					AliasResolution::RESOLVE) const;

	TypeSpecifierListRef GetParameterTypeList() const {
		return m_parameter_type_list;
	}

	const_shared_ptr<TypeSpecifier> GetReturnTypeSpecifier() const {
		return m_return_type;
	}

	const yy::location GetReturnTypeLocation() const {
		return m_return_type_location;
	}

private:
	TypeSpecifierListRef m_parameter_type_list;
	const_shared_ptr<TypeSpecifier> m_return_type;
	const yy::location m_return_type_location;
};

#endif /* FUNCTION_TYPE_SPECIFIER_H_ */

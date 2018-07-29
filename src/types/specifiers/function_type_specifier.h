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
	FunctionTypeSpecifier(const TypeSpecifierListRef parameter_type_list,
			const TypeSpecifierListRef type_parameter_list,
			const_shared_ptr<TypeSpecifier> return_type,
			const yy::location location);
	FunctionTypeSpecifier(const FunctionTypeSpecifier& other);
	virtual ~FunctionTypeSpecifier();

	virtual const string ToString() const;
	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other, const TypeTable& type_table,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const ErrorListRef ValidateDeclaration(const TypeTable& type_table,
			const TypeSpecifierListRef type_parameter_list,
			const yy::location position) const;

	const TypeSpecifierListRef GetParameterTypeList() const {
		return m_parameter_type_list;
	}

	const TypeSpecifierListRef GetTypeParameterList() const {
		return m_type_parameter_list;
	}

	virtual const_shared_ptr<TypeSpecifier> WithTypeArgumentList(
			TypeSpecifierListRef type_arguments) const;

	const_shared_ptr<TypeSpecifier> GetReturnTypeSpecifier() const {
		return m_return_type;
	}

private:
	const TypeSpecifierListRef m_parameter_type_list;
	const TypeSpecifierListRef m_type_parameter_list;
	const_shared_ptr<TypeSpecifier> m_return_type;
};

#endif /* FUNCTION_TYPE_SPECIFIER_H_ */

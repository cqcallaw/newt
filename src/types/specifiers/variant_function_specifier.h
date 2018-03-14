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

#ifndef TYPES_SPECIFIERS_VARIANT_FUNCTION_SPECIFIER_H_
#define TYPES_SPECIFIERS_VARIANT_FUNCTION_SPECIFIER_H_

#include <type_specifier.h>
#include <function_variant.h>
#include <expression.h>

class VariantFunctionSpecifier: public TypeSpecifier {
public:
	VariantFunctionSpecifier(const yy::location location,
			const FunctionVariantListRef variant_list);
	virtual ~VariantFunctionSpecifier();

	virtual const string ToString() const;

	virtual const AnalysisResult AnalyzeAssignmentTo(
			const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const ErrorListRef ValidateDeclaration(const TypeTable& type_table,
			const TypeSpecifierListRef type_parameter_list,
			const yy::location location) const;

	const FunctionVariantListRef GetVariantList() const {
		return m_variant_list;
	}

private:
	const FunctionVariantListRef m_variant_list;
};

#endif /* TYPES_SPECIFIERS_VARIANT_FUNCTION_SPECIFIER_H_ */

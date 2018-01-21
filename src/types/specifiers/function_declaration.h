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

#ifndef SPECIFIERS_FUNCTION_DECLARATION_H_
#define SPECIFIERS_FUNCTION_DECLARATION_H_

#include <declaration_statement.h>
#include <specifiers/function_type_specifier.h>

class FunctionDeclaration: public FunctionTypeSpecifier {
public:
	FunctionDeclaration(DeclarationListRef parameter_list,
			TypeSpecifierListRef type_parameter_list,
			const_shared_ptr<TypeSpecifier> return_type,
			const yy::location location);
	FunctionDeclaration(const FunctionDeclaration& other);
	virtual ~FunctionDeclaration();

	static const_shared_ptr<Result> FromTypeSpecifier(
			const FunctionTypeSpecifier& type_specifier,
			const TypeTable& type_table);

	virtual const_shared_ptr<Result> GetType(const TypeTable& type_table,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	DeclarationListRef GetParameterList() const {
		return m_parameter_list;
	}

	static TypeSpecifierListRef GetTypeList(DeclarationListRef parameter_list);

private:
	DeclarationListRef m_parameter_list;

};

#endif /* SPECIFIERS_FUNCTION_DECLARATION_H_ */

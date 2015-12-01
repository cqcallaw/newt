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

#include <function_type_specifier.h>
#include <declaration_list.h>

class FunctionDeclaration: public FunctionTypeSpecifier {
public:
	FunctionDeclaration(const DeclarationList* parameter_list,
			const TypeSpecifier* return_type);
	virtual ~FunctionDeclaration();

	const static FunctionDeclaration* FromTypeSpecifier(
			const FunctionTypeSpecifier* type_specifier);

	virtual const void* DefaultValue(const TypeTable* type_table) const;

	virtual const DeclarationStatement* GetDeclarationStatement(
			const YYLTYPE position, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* initializer_expression) const;

	const DeclarationList* GetParameterList() const {
		return m_parameter_list;
	}

private:
	const DeclarationList* m_parameter_list;
};

#endif /* SPECIFIERS_FUNCTION_DECLARATION_H_ */

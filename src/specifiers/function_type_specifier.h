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

#include <type_specifier.h>

class Expression;
class Statement;
class StatementBlock;
class Function;

class FunctionTypeSpecifier: public TypeSpecifier {
public:
	FunctionTypeSpecifier(const TypeSpecifierList* parameter_type_list,
			const TypeSpecifier* return_type);
	virtual ~FunctionTypeSpecifier();

	virtual const string ToString() const;
	virtual const bool IsAssignableTo(const TypeSpecifier* other) const;
	virtual const void* DefaultValue(const TypeTable* type_table) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual bool operator!=(const TypeSpecifier &other) const {
		return !(*this == other);
	}

	virtual const DeclarationStatement* GetDeclarationStatement(
			const YYLTYPE position, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* initializer_expression) const;

	const TypeSpecifierList* GetParameterTypeList() const {
		return m_parameter_type_list;
	}

	const TypeSpecifier* GetReturnType() const {
		return m_return_type;
	}
protected:
	static const StatementBlock* GetDefaultStatementBlock(
			const TypeSpecifier* return_type, const TypeTable* type_table);

	static const Function* GetDefaultFunction(
			const FunctionTypeSpecifier* type_specifier,
			const TypeTable* type_table);

private:
	const TypeSpecifierList* m_parameter_type_list;
	const TypeSpecifier* m_return_type;
};

#endif /* FUNCTION_TYPE_SPECIFIER_H_ */

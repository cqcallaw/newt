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
	FunctionTypeSpecifier(TypeSpecifierListRef parameter_type_list,
			const_shared_ptr<TypeSpecifier> return_type);
	FunctionTypeSpecifier(const FunctionTypeSpecifier& other);
	virtual ~FunctionTypeSpecifier();

	virtual const string ToString() const;
	virtual const bool IsAssignableTo(
			const_shared_ptr<TypeSpecifier> other) const;
	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier &other) const;

	virtual bool operator!=(const TypeSpecifier &other) const {
		return !(*this == other);
	}

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	TypeSpecifierListRef GetParameterTypeList() const {
		return m_parameter_type_list;
	}

	const_shared_ptr<TypeSpecifier> GetReturnType() const {
		return m_return_type;
	}
protected:
	static const_shared_ptr<StatementBlock> GetDefaultStatementBlock(
			const_shared_ptr<TypeSpecifier> return_type,
			const TypeTable& type_table);

	static const_shared_ptr<Function> GetDefaultFunction(
			const FunctionTypeSpecifier& type_specifier,
			const TypeTable& type_table);

private:
	TypeSpecifierListRef m_parameter_type_list;
	const_shared_ptr<TypeSpecifier> m_return_type;
};

#endif /* FUNCTION_TYPE_SPECIFIER_H_ */

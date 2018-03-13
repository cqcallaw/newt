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

#ifndef TYPES_FUNCTION_TYPE_H_
#define TYPES_FUNCTION_TYPE_H_

#include <type_definition.h>

class FunctionDeclaration;
class StatementBlock;

class FunctionType: public TypeDefinition {
public:
	FunctionType(DeclarationListRef parameter_type_list,
			const TypeSpecifierListRef type_parameter_list,
			const_shared_ptr<TypeSpecifier> return_type_specifier);

	virtual ~FunctionType() {
	}

	virtual const_shared_ptr<void> GetDefaultValue(const TypeTable& type_table,
			const_shared_ptr<type_parameter_map> type_mapping) const;

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent,
			const_shared_ptr<type_parameter_map> type_mapping) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value,
			const_shared_ptr<type_parameter_map> type_mapping) const;

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value,
			const_shared_ptr<type_parameter_map> type_mapping) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			yy::location location) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const TypeTable& type_table,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void>,
			const_shared_ptr<type_parameter_map> type_mapping) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	const_shared_ptr<FunctionDeclaration> GetTypeSpecifier() const {
		return m_type_specifier;
	}

	static const_shared_ptr<StatementBlock> GetDefaultStatementBlock(
			const_shared_ptr<TypeSpecifier> return_type,
			const TypeTable& type_table);

protected:
	static const_shared_ptr<Function> GetDefaultFunction(
			const_shared_ptr<FunctionDeclaration> declaration,
			const TypeTable& type_table);

private:
	const_shared_ptr<FunctionDeclaration> m_type_specifier;

};

#endif /* TYPES_FUNCTION_TYPE_H_ */

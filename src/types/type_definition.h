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

#ifndef TYPE_DEFINITION_H_
#define TYPE_DEFINITION_H_

#include <string>
#include <defaults.h>

class TypeTable;
class Indent;
class TypeSpecifier;
class ComplexTypeSpecifier;
class Symbol;
class DeclarationStatement;
class Expression;

class TypeDefinition {
public:
	TypeDefinition() {
	}

	virtual ~TypeDefinition() {
	}

	virtual const_shared_ptr<void> GetDefaultValue(
			const TypeTable& type_table) const = 0;

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const = 0;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const = 0;

	virtual const std::string GetValueSeperator(const Indent& indent,
			const_shared_ptr<void> value) const = 0;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container) const = 0;

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const = 0;

	virtual const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void> value) const = 0;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const = 0;
};

#endif /* TYPE_DEFINITION_H_ */

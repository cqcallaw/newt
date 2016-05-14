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

#ifndef TYPES_MAYBE_TYPE_H_
#define TYPES_MAYBE_TYPE_H_

#include <type_definition.h>

class UnitType;

class MaybeType: public TypeDefinition {
public:
	MaybeType(const_shared_ptr<TypeSpecifier> base_type_specifier);
	virtual ~MaybeType();

	virtual const_shared_ptr<void> GetDefaultValue(
			const TypeTable& type_table) const;

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value) const;

	virtual const std::string GetTagSeparator(const Indent& indent,
			const void* value) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const TypeTable& type_table,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void> value) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	const_shared_ptr<TypeSpecifier> GetBaseTypeSpecifier() const {
		return m_base_type_specifier;
	}

private:
	const_shared_ptr<TypeSpecifier> m_base_type_specifier;
	const_shared_ptr<UnitType> m_empty;
};

#endif /* TYPES_MAYBE_TYPE_H_ */
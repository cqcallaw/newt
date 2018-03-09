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

#ifndef TYPES_ALIAS_DEFINITION_H_
#define TYPES_ALIAS_DEFINITION_H_

#include <type_definition.h>
#include <primitive_type_specifier.h>
#include <indent.h>

enum AliasType {
	DIRECT = 0, RECURSIVE
};

class AliasDefinition: public TypeDefinition {
public:
	AliasDefinition(const_shared_ptr<TypeTable> origin_table,
			const_shared_ptr<TypeSpecifier> original_type_specifier,
			const AliasType alias_type, const_shared_ptr<void> default_value =
					nullptr);

	virtual ~AliasDefinition() {
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value) const;

	virtual const std::string GetTagSeparator(const Indent& indent,
			const void* value) const;

	virtual const_shared_ptr<void> GetDefaultValue(const TypeTable& type_table,
			const_shared_ptr<type_parameter_map> type_mapping) const;

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

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			yy::location location) const {
		return m_original_type_specifier;
	}

	const weak_ptr<const TypeTable> GetOriginTable() const {
		return m_origin_table;
	}

	const_shared_ptr<TypeSpecifier> GetOriginal() const {
		return m_original_type_specifier;
	}

	const const_shared_ptr<void> GetDefaultValue() const {
		return m_default_value;
	}

	const AliasType GetAliasType() const {
		return m_alias_type;
	}

	const_shared_ptr<TypeDefinition> GetOriginalType(
			const_shared_ptr<type_parameter_map> type_mapping) const;

private:
	const weak_ptr<const TypeTable> m_origin_table;
	const_shared_ptr<TypeSpecifier> m_original_type_specifier;
	const AliasType m_alias_type;
	const_shared_ptr<void> m_default_value;
};

#endif /* TYPES_ALIAS_DEFINITION_H_ */

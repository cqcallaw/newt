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

#ifndef TYPES_VARIANT_FUNCTION_TYPE_H_
#define TYPES_VARIANT_FUNCTION_TYPE_H_

#include <type_definition.h>
#include <function_variant.h>

class Function;

class VariantFunctionType: public TypeDefinition {
public:
	VariantFunctionType(const FunctionVariantListRef variant_list);
	virtual ~VariantFunctionType();

	virtual const_shared_ptr<void> GetDefaultValue(const TypeTable& type_table,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			yy::location location) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const TypeTable& type_table,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void> value,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location location, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_location,
			const_shared_ptr<Expression> initializer_expression) const;
protected:
	static const_shared_ptr<Function> GetDefaultFunction(
			const FunctionVariantListRef variant_list,
			const TypeTable& type_table,
			const_shared_ptr<type_specifier_map> type_specifier_mapping);

private:
	const FunctionVariantListRef m_variant_list;
};

#endif /* TYPES_VARIANT_FUNCTION_TYPE_H_ */

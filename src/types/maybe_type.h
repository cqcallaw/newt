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

#include <sum_type.h>

class UnitType;
class PrimitiveTypeSpecifier;

class MaybeType: public SumType {
public:
	virtual ~MaybeType();

	virtual const_shared_ptr<void> GetDefaultValue(
			const TypeTable& type_table) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value) const;

	virtual const std::string GetTagSeparator(const Indent& indent,
			const void* value) const;

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			yy::location location) const;

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

	static const_shared_ptr<Result> Build(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<TypeSpecifier> base_type_specifier);

	static const_shared_ptr<Result> Build(
			const shared_ptr<TypeTable> base_type_table,
			const_shared_ptr<TypeSpecifier> base_type_specifier);

	static const_shared_ptr<Result> Build(
			const_shared_ptr<PrimitiveTypeSpecifier> base_type_specifier);

private:
	MaybeType(const_shared_ptr<TypeTable> type_table,
			const_shared_ptr<string> first_variant_name,
			const_shared_ptr<SymbolTable> constructors,
			const_shared_ptr<TypeSpecifier> base_type_specifier);

	const_shared_ptr<TypeSpecifier> m_base_type_specifier;
};

#endif /* TYPES_MAYBE_TYPE_H_ */

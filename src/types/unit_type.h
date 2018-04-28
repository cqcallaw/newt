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

#ifndef TYPES_UNIT_TYPE_H_
#define TYPES_UNIT_TYPE_H_

#include <complex_type.h>
#include <unit.h>

class UnitType: public ComplexType {
public:
	UnitType(const_shared_ptr<MaybeType> maybe_type);
	virtual ~UnitType();

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
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual const_shared_ptr<void> GetMemberDefaultValue(
			const_shared_ptr<std::string> member_name) const;

	virtual const_shared_ptr<TypeTable> GetDefinition() const;

	virtual const_shared_ptr<MaybeType> GetMaybeType() const;

	virtual const AnalysisResult AnalyzeConversion(
			const ComplexTypeSpecifier& current,
			const TypeSpecifier& unaliased_other) const;

	const_shared_ptr<Unit> GetValue() const {
		return m_value;
	}

protected:
	virtual const_shared_ptr<Result> PreprocessSymbolCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const;

	virtual const SetResult InstantiateCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<TypeSpecifier> value_type_specifier,
			const std::string& instance_name,
			const_shared_ptr<void> data) const;
private:
	const_shared_ptr<Unit> m_value;
	const_shared_ptr<MaybeType> m_maybe_type;
	const_shared_ptr<TypeTable> m_table;
};

#endif /* TYPES_UNIT_TYPE_H_ */

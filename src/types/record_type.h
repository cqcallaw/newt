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

#ifndef COMPOUND_TYPE_H_
#define COMPOUND_TYPE_H_

#include <map>
#include <string>
#include <linked_list.h>
#include <symbol_context.h>
#include <modifier.h>
#include <complex_type.h>
#include <declaration_statement.h>

class MemberDefinition;
class Result;
class ComplexTypeSpecifier;
class TypeTable;

using namespace std;
class RecordType: public ComplexType {
public:
	RecordType(const_shared_ptr<TypeTable> definition,
			const Modifier::Type modifiers,
			const TypeSpecifierListRef type_parameter_list,
			const_shared_ptr<MaybeType> maybe_type);

	virtual ~RecordType();

	const_shared_ptr<TypeDefinition> GetMember(const std::string& name) const;

	virtual const_shared_ptr<void> GetMemberDefaultValue(
			const_shared_ptr<std::string> member_name) const;

	const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value) const;

	virtual const std::string GetTagSeparator(const Indent& indent,
			const void* value) const;

	virtual const_shared_ptr<void> GetDefaultValue(
			const TypeTable& type_table) const;

	virtual const AnalysisResult AnalyzeConversion(
			const ComplexTypeSpecifier& current,
			const TypeSpecifier& unaliased_other) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const TypeTable& type_table,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void>) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual const_shared_ptr<TypeTable> GetDefinition() const {
		return m_definition;
	}

	static const ErrorListRef Build(const_shared_ptr<string> name,
			const shared_ptr<ExecutionContext> output,
			const shared_ptr<ExecutionContext> closure,
			const Modifier::Type modifiers,
			const DeclarationListRef member_declarations,
			const_shared_ptr<RecordTypeSpecifier> type_specifier,
			const TypeSpecifierListRef type_parameter_list);

	const Modifier::Type GetModifiers() const {
		return m_modifiers;
	}

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container,
			yy::location location) const;

	const_shared_ptr<MaybeType> GetMaybeType() const {
		return m_maybe_type;
	}

	const TypeSpecifierListRef GetTypeParameterList() const {
		return m_type_parameter_list;
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
	const_shared_ptr<TypeTable> m_definition;
	const Modifier::Type m_modifiers;
	const TypeSpecifierListRef m_type_parameter_list;
	const_shared_ptr<MaybeType> m_maybe_type;
};

#endif /* COMPOUND_TYPE_H_ */

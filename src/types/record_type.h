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

typedef map<const std::string, const_shared_ptr<MemberDefinition>> definition_map;

using namespace std;
class RecordType: public ComplexType {
public:
	RecordType(const_shared_ptr<definition_map> definition,
			const Modifier::Type modifiers);
	virtual ~RecordType();
	const_shared_ptr<MemberDefinition> GetMember(const std::string& name) const;

	virtual const_shared_ptr<TypeSpecifier> GetMemberType(
			const std::string& member_name) const;

	virtual const_shared_ptr<void> GetMemberDefaultValue(
			const_shared_ptr<std::string> member_name) const;

	const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	virtual const_shared_ptr<void> GetDefaultValue(
			const_shared_ptr<std::string> type_name) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<void> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const;

	const_shared_ptr<definition_map> GetDefinition() const {
		return m_definition;
	}

	static const_shared_ptr<Result> Build(
			const_shared_ptr<ExecutionContext> context,
			const Modifier::Type modifiers,
			const DeclarationListRef member_declarations);

	const Modifier::Type GetModifiers() const {
		return m_modifiers;
	}

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const;

protected:
	virtual const_shared_ptr<Result> PreprocessSymbolCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const;

	virtual const SetResult InstantiateCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const std::string& instance_name,
			const_shared_ptr<void> data) const;

private:
	const_shared_ptr<definition_map> m_definition;
	const Modifier::Type m_modifiers;
};

#endif /* COMPOUND_TYPE_H_ */

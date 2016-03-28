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

#ifndef SPECIFIERS_RECORD_TYPE_SPECIFIER_H_
#define SPECIFIERS_RECORD_TYPE_SPECIFIER_H_

#include <namespace_qualifier.h>
#include <specifiers/complex_type_specifier.h>
#include <type_table.h>

class Expression;
class Statement;
class SymbolContext;

class RecordTypeSpecifier: public ComplexTypeSpecifier {
public:
	RecordTypeSpecifier(const_shared_ptr<std::string> type_name) :
			RecordTypeSpecifier(type_name,
					NamespaceQualifierList::GetTerminator()) {
	}

	RecordTypeSpecifier(const_shared_ptr<std::string> type_name,
			const NamespaceQualifierListRef space) :
			m_type_name(type_name), m_space(space) {
	}

	virtual ~RecordTypeSpecifier() {
	}

	virtual const bool IsAssignableTo(const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position, const_shared_ptr<string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const;

	virtual const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<void> value,
			const TypeTable& container) const;

	virtual const_shared_ptr<void> DefaultValue(
			const TypeTable& type_table) const;

	virtual bool operator==(const TypeSpecifier& other) const;

	virtual const_shared_ptr<TypeDefinition> GetType(
			const TypeTable& type_table, AliasResolution resolution =
					AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<std::string> GetTypeName() const {
		return m_type_name;
	}

	virtual const std::string ToString() const {
		return *m_type_name;
	}

	virtual const NamespaceQualifierListRef GetNamespace() const {
		return m_space;
	}

private:
	const_shared_ptr<std::string> m_type_name;
	const NamespaceQualifierListRef m_space;
};

#endif /* SPECIFIERS_RECORD_TYPE_SPECIFIER_H_ */

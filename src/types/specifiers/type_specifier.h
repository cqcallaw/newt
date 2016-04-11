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

#ifndef SPECIFIERS_TYPE_SPECIFIER_H_
#define SPECIFIERS_TYPE_SPECIFIER_H_

#include <defaults.h>
#include <string>
#include <linked_list.h>
#include <alias_resolution.h>
#include <type_definition.h>

class Expression;
class DeclarationStatement;
class TypeTable;
class Symbol;

class TypeSpecifier {
public:
	virtual ~TypeSpecifier() {
	}

	virtual const std::string ToString() const = 0;
	virtual const bool IsAssignableTo(const_shared_ptr<TypeSpecifier> other,
			const TypeTable& type_table) const = 0;

	virtual bool operator==(const TypeSpecifier &other) const = 0;

	virtual bool operator!=(const TypeSpecifier &other) const {
		return !(*this == other);
	}

	virtual const_shared_ptr<TypeDefinition> GetType(
			const TypeTable& type_table, AliasResolution resolution =
					AliasResolution::RESOLVE) const = 0;

	virtual const WideningResult AnalyzeConversion(const TypeTable& type_table,
			const TypeSpecifier& other) const {
		return INCOMPATIBLE;
	}

	const_shared_ptr<void> DefaultValue(const TypeTable& type_table) const {
		auto type = GetType(type_table, RESOLVE);
		if (type) {
			return type->GetDefaultValue(type_table);
		}

		return nullptr;
	}
};

typedef const LinkedList<const TypeSpecifier, ALLOW_DUPLICATES> TypeSpecifierList;
typedef shared_ptr<TypeSpecifierList> TypeSpecifierListRef;

#endif /* SPECIFIERS_TYPE_SPECIFIER_H_ */

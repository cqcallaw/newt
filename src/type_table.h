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

#ifndef TYPE_TABLE_H_
#define TYPE_TABLE_H_

#include <member_declaration.h>
#include <type.h>
#include <map>
#include <set>
#include <alias_definition.h>

class TypeDefinition;
class RecordType;
class ComplexTypeSpecifier;

typedef map<const string, const_shared_ptr<TypeDefinition>> type_map;

class Indent;

class TypeTable {
public:
	TypeTable();
	virtual ~TypeTable();

	void AddType(const string& name,
			const_shared_ptr<TypeDefinition> definition);

	template<class T> const shared_ptr<const T> GetType(
			const_shared_ptr<std::string> name) const {
		return GetType<T>(*name);
	}

	template<class T> const shared_ptr<const T> GetType(
			const std::string& name) const {
		auto result = m_table->find(name);

		if (result != m_table->end()) {
			shared_ptr<const TypeDefinition> value = result->second;

			// handle aliases
			auto as_alias = std::dynamic_pointer_cast<const AliasDefinition>(
					value);
			if (as_alias) {
				value = as_alias->GetOrigin();
			}

			auto cast = dynamic_pointer_cast<const T>(value);
			if (cast) {
				return cast;
			}
		}

		return shared_ptr<const T>();
	}

	const bool ContainsType(const ComplexTypeSpecifier& type_specifier);

	const bool ContainsType(const string& name);

	const void print(ostream &os, const Indent& indent) const;

	const static string DefaultTypeName;

	static volatile_shared_ptr<TypeTable> GetDefault();

	const uint CountEntriesOfType(const TypeSpecifier& type_specifier) const;

	/**
	 * Return the _first_ name that is mapped to the given specifier.
	 */
	const std::string MapSpecifierToName(
			const TypeSpecifier& type_specifier) const;

	const_shared_ptr<std::set<std::string>> GetTypeNames() const;

private:
	const shared_ptr<type_map> m_table;
};

#endif /* TYPE_TABLE_H_ */

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

#include <type.h>
#include <map>
#include <set>
#include <alias_definition.h>
#include <complex_type_specifier.h>
#include <search_type.h>
#include <modifier.h>

class SymbolContext;
class UnitType;

typedef map<const string, const_shared_ptr<TypeDefinition>> type_map;

class Indent;

/**
 * A map from names to type definitions
 *
 * Type tables are meant to be immutable, as they define the interior structure of complex types.
 * The RemoveTypeDefinition logic must be used with care.
 */
class TypeTable {
public:
	TypeTable(const shared_ptr<TypeTable> parent = nullptr);
	TypeTable(const shared_ptr<type_map> table,
			const shared_ptr<TypeTable> parent = nullptr);

	virtual ~TypeTable();

	const shared_ptr<TypeTable> Clone() const;

	const shared_ptr<TypeTable> WithParent(
			const shared_ptr<TypeTable> parent) const;

	void AddType(const string& name,
			const_shared_ptr<TypeDefinition> definition);

	template<class T> const shared_ptr<const T> GetType(
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const SearchType search_type,
			AliasResolution resolution = RESOLVE) const {
		return GetType<T>(*type_specifier, search_type, resolution);
	}

	template<class T> const shared_ptr<const T> GetType(
			const ComplexTypeSpecifier& type_specifier,
			const SearchType search_type,
			AliasResolution resolution = RESOLVE) const {
		return GetType<T>(type_specifier.GetTypeName(), search_type, resolution);
	}

	template<class T> const shared_ptr<const T> GetType(
			const_shared_ptr<std::string> name, const SearchType search_type,
			AliasResolution resolution = RESOLVE) const {
		return GetType<T>(*name, search_type, resolution);
	}

	template<class T> const shared_ptr<const T> GetType(const std::string& name,
			const SearchType search_type,
			AliasResolution resolution = RESOLVE) const {
		if (name == *GetNilName()) {
			auto value = GetNilType();
			auto cast = dynamic_pointer_cast<const T>(value);
			if (cast) {
				return cast;
			}
		}

		auto result = m_table->find(name);
		if (result != m_table->end()) {
			shared_ptr<const TypeDefinition> value = result->second;

			if (resolution == RESOLVE) {
				// handle aliases
				auto as_alias =
						std::dynamic_pointer_cast<const AliasDefinition>(value);
				if (as_alias) {
					value = as_alias->GetOriginalType(nullptr);
				}
			}

			auto cast = dynamic_pointer_cast<const T>(value);
			if (cast) {
				return cast;
			}
		} else if (search_type == SearchType::DEEP) {
			if (auto lock = m_parent.lock()) {
				return lock->GetType<T>(name, search_type, resolution);
			}
		}

		return shared_ptr<const T>();
	}

	/**
	 * If a member with the specified name C++ type exists, replace it with the specified type definition.
	 */
	template<class T> void ReplaceTypeDefinition(const string& name,
			const_shared_ptr<TypeDefinition> definition) {
		auto existing = m_table->find(name);
		if (std::dynamic_pointer_cast<const T>(existing->second)) {
			m_table->erase(existing);
			AddType(name, definition);
		}
	}

	template<class T> void ReplaceTypeDefinition(
			const_shared_ptr<std::string> name,
			const_shared_ptr<TypeDefinition> definition) {
		ReplaceTypeDefinition<T>(*name, definition);
	}

	template<class T> void RemoveTypeDefinition(const string& name) {
		auto existing = m_table->find(name);
		if (std::dynamic_pointer_cast<const T>(existing->second)) {
			m_table->erase(existing);
		}
	}

	template<class T> void RemoveTypeDefinition(
			const_shared_ptr<std::string> name) {
		RemoveTypeDefinition<T>(*name);
	}

	volatile_shared_ptr<SymbolContext> GetDefaultSymbolContext(
			const Modifier::Type modifiers,
			const_shared_ptr<ComplexTypeSpecifier> container,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	const bool ContainsType(const ComplexTypeSpecifier& type_specifier);

	const bool ContainsType(const string& name);

	const void print(ostream &os, const Indent& indent,
			const SearchType search_type = SHALLOW) const;

	const static string DefaultTypeName;

	static volatile_shared_ptr<TypeTable> GetDefault();

	const uint CountEntriesOfType(const ComplexTypeSpecifier& current,
			const TypeSpecifier& type_specifier) const;

	/**
	 * Return the _first_ name that is mapped to the given specifier.
	 */
	const std::string MapSpecifierToName(const ComplexTypeSpecifier& current,
			const TypeSpecifier& other) const;

	const_shared_ptr<std::set<std::string>> GetTypeNames() const;

	static const_shared_ptr<std::string> GetNilName();
	static const_shared_ptr<UnitType> GetNilType();
	static const_shared_ptr<ComplexTypeSpecifier> GetNilTypeSpecifier();

	const weak_ptr<TypeTable> GetParent() const {
		return m_parent;
	}

private:
	const shared_ptr<type_map> m_table;
	const weak_ptr<TypeTable> m_parent;
};

#endif /* TYPE_TABLE_H_ */

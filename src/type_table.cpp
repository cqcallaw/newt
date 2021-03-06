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

#include <type_table.h>
#include <record_type.h>
#include <primitive_type.h>
#include <complex_type_specifier.h>
#include <placeholder_type.h>
#include <unit_type.h>
#include <maybe_type.h>
#include <memory>
#include <symbol_context.h>

TypeTable::TypeTable(const shared_ptr<TypeTable> parent) :
		TypeTable(make_shared<type_map>(), parent) {
}

TypeTable::TypeTable(const shared_ptr<type_map> table,
		const shared_ptr<TypeTable> parent) :
		m_table(table), m_parent(parent) {
}

TypeTable::~TypeTable() {
}

const shared_ptr<TypeTable> TypeTable::Clone() const {
	auto new_map = make_shared<type_map>();
	new_map->insert(m_table->begin(), m_table->end());
	return make_shared<TypeTable>(TypeTable(new_map, m_parent.lock()));
}

const shared_ptr<TypeTable> TypeTable::WithParent(
		const shared_ptr<TypeTable> new_parent) const {
	assert(new_parent.get() != this);
	return make_shared<TypeTable>(TypeTable(m_table, new_parent));
}

void TypeTable::AddType(const std::string& name,
		const_shared_ptr<TypeDefinition> definition) {
	auto existing = m_table->find(name);

	if (existing == m_table->end()) {
		m_table->insert(
				pair<const std::string, const_shared_ptr<TypeDefinition>>(name,
						definition));
	} else if (std::dynamic_pointer_cast<const PlaceholderType>(
			existing->second)) {
		m_table->erase(existing);
		m_table->insert(
				pair<const std::string, const_shared_ptr<TypeDefinition>>(name,
						definition));
	} else {
		assert(false);
	}
}

const void TypeTable::print(ostream& os, const Indent& indent,
		const SearchType search_type) const {
	type_map::iterator iter;
	for (iter = m_table->begin(); iter != m_table->end(); ++iter) {
		os << indent;
		os << iter->first << ":" << endl;
		const_shared_ptr<TypeDefinition> type = iter->second;
		os << type->ToString(*this, indent);
		os << endl;
	}

	if (search_type == DEEP) {
		if (auto parent = m_parent.lock()) {
			os << indent + 1 << "--> " << parent << endl;
			parent->print(os, indent + 1, search_type);
		}
	}
}

volatile_shared_ptr<TypeTable> TypeTable::GetDefault() {
	static volatile_shared_ptr<TypeTable> instance = make_shared<TypeTable>();
	return instance;
}

const uint TypeTable::CountEntriesOfType(const ComplexTypeSpecifier& current,
		const TypeSpecifier& other) const {
	plain_shared_ptr<string> other_complex_name = nullptr;
	try {
		auto other_as_complex = dynamic_cast<const ComplexTypeSpecifier&>(other);
		plain_shared_ptr<ComplexTypeSpecifier> other_container =
				other_as_complex.GetContainer();
		other_complex_name = other_as_complex.GetTypeName();
		if (other_container && *other_container != current) {
			return 0; //we have a mis-match between containers
		}
	} catch (std::bad_cast& e) {
		//swallow
	}

	uint count = 0;
	for (const auto &entry : *m_table) {
		auto name = entry.first;
		auto definition = entry.second;

		auto as_alias = dynamic_pointer_cast<const AliasDefinition>(definition);
		if (as_alias && *as_alias->GetOriginal() == other) {
			count++;
		} else if (other_complex_name && *other_complex_name == name) {
			count++;
		}
	}
	return count;
}

const std::string TypeTable::MapSpecifierToName(
		const ComplexTypeSpecifier& current, const TypeSpecifier& other) const {
	plain_shared_ptr<string> other_complex_name = nullptr;
	try {
		auto other_as_complex = dynamic_cast<const ComplexTypeSpecifier&>(other);
		plain_shared_ptr<ComplexTypeSpecifier> other_container =
				other_as_complex.GetContainer();
		other_complex_name = other_as_complex.GetTypeName();
		auto debug = other.ToString();
		auto debug2 = current.ToString();
		if (other_container) {
			if (*other_container == current) {
				//the container is the same, so we can just use the other type name
				auto name = other_as_complex.GetTypeName();
				if (GetType<TypeDefinition>(name, SHALLOW, RETURN)) {
					return *name;
				}
			}
			//we have a mis-match between containers, or the member name doesn't exist on this type
			return "";
		}
	} catch (std::bad_cast& e) {
		//swallow
	}

	for (const auto &entry : *m_table) {
		auto name = entry.first;
		auto definition = entry.second;

		auto as_alias = dynamic_pointer_cast<const AliasDefinition>(definition);
		if (as_alias && *as_alias->GetOriginal() == other) {
			return name;
		} else if (other_complex_name && *other_complex_name == name) {
			return name;
		}
	}

	return "";
}

const bool TypeTable::ContainsType(const ComplexTypeSpecifier& type_specifier) {
	return ContainsType(*type_specifier.GetTypeName());
}

const bool TypeTable::ContainsType(const string& name) {
	return (bool) GetType<const TypeDefinition>(name, SHALLOW, RETURN);
}

volatile_shared_ptr<SymbolContext> TypeTable::GetDefaultSymbolContext(
		const Modifier::Type modifiers,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	volatile_shared_ptr<SymbolTable> result = make_shared<SymbolTable>(
			modifiers);

	for (const auto &entry : *m_table) {
		auto name = entry.first;
		auto type = entry.second;

		auto default_value = type->GetDefaultValue(*this);
		auto type_specifier = type->GetTypeSpecifier(make_shared<string>(name),
				container, GetDefaultLocation());
		auto default_symbol = type->GetSymbol(*this, type_specifier,
				default_value);

		InsertResult insert_result = result->InsertSymbol(name, default_symbol);
		assert(insert_result == INSERT_SUCCESS);
	}

	return result;
}

const_shared_ptr<std::set<std::string>> TypeTable::GetTypeNames() const {
	shared_ptr<std::set<std::string>> result =
			make_shared<std::set<std::string>>();
	for (const auto &entry : *m_table) {
		auto name = entry.first;
		result->insert(name);
	}

	return result;
}

const_shared_ptr<std::string> TypeTable::GetNilName() {
	const static const_shared_ptr<std::string> value = make_shared<string>(
			"nil");
	return value;
}

const_shared_ptr<UnitType> TypeTable::GetNilType() {
	const static const_shared_ptr<UnitType> value = make_shared<UnitType>(
			MaybeType::Build(TypeTable::GetDefault(),
					TypeTable::GetNilTypeSpecifier())->GetData<MaybeType>());
	return value;
}

const_shared_ptr<ComplexTypeSpecifier> TypeTable::GetNilTypeSpecifier() {
	const static const_shared_ptr<ComplexTypeSpecifier> value = make_shared<
			ComplexTypeSpecifier>(GetNilName());
	return value;
}

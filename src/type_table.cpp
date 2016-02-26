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

TypeTable::TypeTable() :
		m_table(make_shared<type_map>()) {
}

TypeTable::~TypeTable() {
}

void TypeTable::AddType(const std::string& name,
		const_shared_ptr<TypeDefinition> definition) {
	m_table->insert(
			pair<const std::string, const_shared_ptr<TypeDefinition>>(name,
					definition));
}

const void TypeTable::print(ostream& os, const Indent& indent) const {
	type_map::iterator iter;
	for (iter = m_table->begin(); iter != m_table->end(); ++iter) {
		os << indent;
		os << iter->first << ":" << endl;
		const_shared_ptr<TypeDefinition> type = iter->second;
		os << type->ToString(*this, indent);
	}
}

volatile_shared_ptr<TypeTable> TypeTable::GetDefault() {
	static volatile_shared_ptr<TypeTable> instance = make_shared<TypeTable>();
	return instance;
}

const uint TypeTable::CountEntriesOfType(
		const TypeSpecifier& type_specifier) const {
	uint count = 0;
	for (const auto &entry : *m_table) {
		auto name = entry.first;
		auto definition = entry.second;
		if (definition->IsSpecifiedBy(name, type_specifier)) {
			count++;
		}
	}
	return count;
}

const std::string TypeTable::MapSpecifierToName(
		const TypeSpecifier& type_specifier) const {
	for (const auto &entry : *m_table) {
		auto name = entry.first;
		auto definition = entry.second;
		if (definition->IsSpecifiedBy(name, type_specifier)) {
			return name;
		}
	}

	return "";
}

const bool TypeTable::ContainsType(const ComplexTypeSpecifier& type_specifier) {
	return m_table->find(*type_specifier.GetTypeName()) != m_table->end();
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

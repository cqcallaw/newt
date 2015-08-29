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

TypeTable::TypeTable() {
	table = new map<const string, const CompoundType*>();
}

TypeTable::~TypeTable() {
	delete (table);
}

void TypeTable::AddType(const string name,
		const map<const string, const PrimitiveType>* definition) {
}

const CompoundType* TypeTable::GetType(const string name) const {
	auto result = table->find(name);

	if (result != table->end()) {
		return result->second;
	}

	return CompoundType::DefaultCompoundType;
}

const void TypeTable::print(ostream& os) const {
	map<const string, const CompoundType*>::iterator iter;
	for (iter = table->begin(); iter != table->end(); ++iter) {
		os << iter->first << ": " << endl;
		CompoundType* type = (CompoundType *) iter->second;
		os << *type;
		os << endl;
	}
}

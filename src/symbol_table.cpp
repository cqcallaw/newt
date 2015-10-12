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

#include <compound_type_instance.h>
#include <iostream>
#include <sstream>
#include <defaults.h>
#include "symbol_table.h"

#include "type.h"
#include "utils.h"

using namespace std;

SymbolTable::SymbolTable(const LinkedList<SymbolContext*>* parent) :
		SymbolContext(Modifier::NONE, parent) {
}

SymbolTable::SymbolTable(const Modifier::Type modifiers,
		const LinkedList<SymbolContext*>* parent_context,
		map<const string, const Symbol*, comparator>* values) :
		SymbolContext(modifiers, parent_context, values) {
}

InsertResult SymbolTable::InsertSymbol(const Symbol* symbol) {
	std::map<const string, const Symbol*>::iterator search_result;

	auto table = GetTable();
	search_result = table->find(symbol->GetName());

	if (search_result != table->end()) {
		return SYMBOL_EXISTS;
	} else {
		table->insert(
				std::pair<const string, const Symbol*>(symbol->GetName(),
						symbol));

		return INSERT_SUCCESS;
	}
}

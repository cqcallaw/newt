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

#include <iostream>
#include <sstream>
#include <defaults.h>
#include "symbol_table.h"

#include "type.h"
#include "utils.h"

using namespace std;

SymbolTable * SymbolTable::m_instance = NULL;

SymbolTable::SymbolTable() {
	table = new map<const string, const Symbol*, comparator>();
}

SymbolTable* SymbolTable::instance() {
	if (SymbolTable::m_instance == NULL)
		SymbolTable::m_instance = new SymbolTable();
	return SymbolTable::m_instance;
}

const Symbol* SymbolTable::GetSymbol(const string identifier) {
	auto result = table->find(identifier);

	if (result != table->end()) {
		return result->second;
	}

	return Symbol::DefaultSymbol;
}

const Symbol* SymbolTable::GetSymbol(const string* identifier) {
	return GetSymbol(*identifier);
}
InsertResult SymbolTable::InsertSymbol(const Symbol* symbol) {
	std::map<const string, const Symbol*>::iterator search_result;

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

SetResult SymbolTable::SetSymbol(const string identifier, const bool* value) {
	return SetSymbol(identifier, BOOLEAN, (void*) value);
}
SetResult SymbolTable::SetSymbol(const string identifier, const int* value) {
	return SetSymbol(identifier, INT, (void*) value);
}
SetResult SymbolTable::SetSymbol(const string identifier,
		const double* value) {
	return SetSymbol(identifier, DOUBLE, (void*) value);
}
SetResult SymbolTable::SetSymbol(const string identifier,
		const string* value) {
	return SetSymbol(identifier, STRING, (void*) value);
}

SetResult SymbolTable::SetSymbol(const string identifier, Type type,
		const void* value) {
	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	} else if ((symbol->GetType() < STRING && symbol->GetType() > type)
			|| (symbol->GetType() != type)) {
		return INCOMPATIBLE_TYPE;
	}

	//TODO: error checking
	//TODO: free memory from old symbols
	const Symbol* new_symbol = symbol->WithValue(type, value);
	table->erase(identifier);
	table->insert(
			std::pair<const string, const Symbol*>(identifier, new_symbol));

	return SET_SUCCESS;
}

SetResult SymbolTable::SetSymbol(const string identifier, int index,
		const bool* value) {
	return SetArraySymbol(identifier, BOOLEAN, index, (void*) value);
}
SetResult SymbolTable::SetSymbol(const string identifier, int index,
		const int* value) {
	return SetArraySymbol(identifier, INT, index, (void*) value);
}
SetResult SymbolTable::SetSymbol(const string identifier, int index,
		const double* value) {
	return SetArraySymbol(identifier, DOUBLE, index, (void*) value);
}
SetResult SymbolTable::SetSymbol(const string identifier, int index,
		const string* value) {
	return SetArraySymbol(identifier, STRING, index, (void*) value);
}

SetResult SymbolTable::SetArraySymbol(const string identifier, Type type,
		int index, const void* value) {
	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	}

	Symbol* new_symbol;
	const ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;
	switch (symbol->GetType()) {
	case INT_ARRAY: {
		int* new_value = (int*) malloc(sizeof(int));
		if (type == BOOLEAN) {
			*new_value = *((bool*) value);
		} else if (type == INT) {
			*new_value = *((int*) value);
		} else {
			return INCOMPATIBLE_TYPE;
		}

		new_symbol = (Symbol*) as_array_symbol->WithValue(index, new_value);
		break;
	}
	case DOUBLE_ARRAY: {
		double* new_value = (double*) malloc(sizeof(double));
		if (type == BOOLEAN) {
			*new_value = *((bool*) value);
		} else if (type == INT) {
			*new_value = *((int*) value);
		} else if (type == DOUBLE) {
			*new_value = *((double*) value);
		} else {
			return INCOMPATIBLE_TYPE;
		}

		new_symbol = (Symbol*) as_array_symbol->WithValue(index, new_value);
		break;
	}
	case STRING_ARRAY: {
		string* new_value;
		if (type == BOOLEAN) {
			new_value = AsString((bool*) value);
		} else if (type == INT) {
			new_value = AsString((int*) value);
		} else if (type == DOUBLE) {
			new_value = AsString((double*) value);
		} else if (type == STRING) {
			new_value = (string*) value;
		} else {
			return INCOMPATIBLE_TYPE;
		}

		new_symbol = (Symbol*) as_array_symbol->WithValue(index, new_value);
		break;
	}
	default:
		return INCOMPATIBLE_TYPE;
	}

	//TODO: error checking
	//TODO: free memory from old symbols
	table->erase(identifier);
	table->insert(
			std::pair<const string, const Symbol*>(identifier, new_symbol));

	return SET_SUCCESS;
}

void SymbolTable::print(ostream &os) {
	std::map<const string, const Symbol*>::iterator iter;
	for (iter = table->begin(); iter != table->end(); ++iter) {
		Symbol* symbol = (Symbol *) iter->second;
		os << *symbol;
	}
}

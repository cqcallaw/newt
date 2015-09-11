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
#include <symbol_context.h>
#include "symbol_table.h"

#include "type.h"
#include "utils.h"

SymbolContext::SymbolContext(const Modifier::Type modifiers,
		const LinkedList<const SymbolContext*>* parent) :
		m_modifiers(modifiers), m_parent(parent), table(
				new map<const string, const Symbol*, comparator>()) {
}

SymbolContext::~SymbolContext() {
}

const Symbol* SymbolContext::GetSymbol(const string identifier) const {
	auto result = table->find(identifier);

	if (result != table->end()) {
		return result->second;
	}

	return Symbol::DefaultSymbol;
}

const Symbol* SymbolContext::GetSymbol(const string* identifier) const {
	return GetSymbol(*identifier);
}

SymbolContext::SymbolContext(const Modifier::Type modifiers,
		const LinkedList<const SymbolContext*>* parent_context,
		const map<const string, const Symbol*>* values) :
		SymbolContext(modifiers, parent_context) {
	map<const string, const Symbol*>::const_iterator values_iter;
	for (values_iter = values->begin(); values_iter != values->end();
			++values_iter) {
		table->insert(
				pair<const string, const Symbol*>(values_iter->first,
						values_iter->second));
	}
}

const void SymbolContext::print(ostream &os, const Indent indent) const {
	std::map<const string, const Symbol*>::iterator iter;
	for (iter = table->begin(); iter != table->end(); ++iter) {
		os << indent;
		const Symbol* symbol = iter->second;
		os << *symbol;
		os << endl;
	}
}

SetResult SymbolContext::SetSymbol(const string identifier, const bool* value) {
	return SetSymbol(identifier, BOOLEAN, (void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier, const int* value) {
	return SetSymbol(identifier, INT, (void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier,
		const double* value) {
	return SetSymbol(identifier, DOUBLE, (void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier,
		const string* value) {
	return SetSymbol(identifier, STRING, (void*) value);
}

SetResult SymbolContext::SetSymbol(const string identifier,
		const CompoundTypeInstance* value) {
	return SetSymbol(identifier, COMPOUND, (void*) value);
}

SetResult SymbolContext::SetSymbol(const string identifier,
		const BasicType type, const void* value) {
	if (m_modifiers & Modifier::READONLY) {
		return MUTATION_DISALLOWED;
	}

	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	} else if ((symbol->GetType() < STRING && symbol->GetType() > type)
			|| (symbol->GetType() != type)
			|| !(type & (BOOLEAN | INT | DOUBLE | STRING | COMPOUND))) {
		return INCOMPATIBLE_TYPE;
	}

	const Symbol* new_symbol = symbol->WithValue(type, value);

	//TODO: error checking
	//TODO: free memory from old symbols
	table->erase(identifier);
	table->insert(
			std::pair<const string, const Symbol*>(identifier, new_symbol));

	return SET_SUCCESS;
}

SetResult SymbolContext::SetSymbol(const string identifier, const int index,
		const bool* value) {
	return SetArraySymbolIndex(identifier, BOOLEAN, index, (void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier, const int index,
		const int* value) {
	return SetArraySymbolIndex(identifier, INT, index, (void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier, const int index,
		const double* value) {
	return SetArraySymbolIndex(identifier, DOUBLE, index, (void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier, const int index,
		const string* value) {
	return SetArraySymbolIndex(identifier, STRING, index, (void*) value);
}

SetResult SymbolContext::SetArraySymbol(const string identifier,
		const ArraySymbol* new_symbol) {
	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	} else if (symbol->GetType() != new_symbol->GetType()
			|| !(symbol->GetType() & (ARRAY))) {
		return INCOMPATIBLE_TYPE;
	}

	//TODO: error checking
	//TODO: free memory from old symbols
	table->erase(identifier);
	table->insert(
			std::pair<const string, const Symbol*>(identifier, new_symbol));

	return SET_SUCCESS;
}

/*SetResult SymbolContext::SetMember(const string identifier,
 const string member_name, const BasicType member_type,
 const void* value) {
 const Symbol* symbol = GetSymbol(identifier);

 if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
 return UNDEFINED_SYMBOL;
 } else {
 if (symbol->GetType() != STRUCT) {
 return INCOMPATIBLE_TYPE;
 }

 Struct* current = (Struct*) symbol->GetValue();
 SymbolContext* context = current->GetDefinition();
 map<const string, const Symbol*, comparator>* table =
 context->GetTable();
 const map<const string, const Symbol*, comparator>::const_iterator find_result =
 table->find(member_name);
 if (find_result == table->end()) {
 return UNDEFINED_MEMBER;
 }

 const Symbol* member_symbol = find_result->second;
 const BasicType existing_member_type = member_symbol->GetType();

 return context->SetSymbol(member_name, member_type, value);
 }
 }*/

SetResult SymbolContext::SetArraySymbolIndex(const string identifier,
		BasicType type, int index, const void* value) {
	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	}

	Symbol* new_symbol;
	const ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;
	switch (as_array_symbol->GetElementType()) {
	case INT: {
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
	case DOUBLE: {
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
	case STRING: {
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

		new_symbol = (Symbol*) as_array_symbol->WithValue<const string*>(index,
				new_value);
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

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

#include <sstream>

#include "assert.h"
#include "array_symbol.h"
#include "symbol_table.h"
#include "error.h"
#include "expression.h"
#include <execution_context.h>

const std::string ArraySymbol::DefaultArraySymbolName = std::string(
		"[!!_DEFAULT_ARRAY_SYMBOL_!!]");

const ArraySymbol* ArraySymbol::DefaultArraySymbol = new ArraySymbol(
		DefaultArraySymbolName, new int[0](), 0, false);

ArraySymbol::ArraySymbol(const string name, const int value[], const int size,
		const bool initialized) :
		Symbol(INT_ARRAY, name, (const void*) value), m_size(size), m_initialized(
				initialized) {
}

ArraySymbol::ArraySymbol(const string name, const double value[],
		const int size, const bool initialized) :
		Symbol(DOUBLE_ARRAY, name, (const void*) value), m_size(size), m_initialized(
				initialized) {
}

ArraySymbol::ArraySymbol(const string name, const string* value[],
		const int size, const bool initialized) :
		Symbol(STRING_ARRAY, name, (const void*) value), m_size(size), m_initialized(
				initialized) {
}

ArraySymbol::ArraySymbol(const string* name, const int value[], const int size,
		const bool initialized) :
		ArraySymbol(*name, value, size, initialized) {
}

ArraySymbol::ArraySymbol(const string* name, const double value[],
		const int size, const bool initialized) :
		ArraySymbol(*name, value, size, initialized) {
}

ArraySymbol::ArraySymbol(const string* name, const string* value[],
		const int size, const bool initialized) :
		ArraySymbol(*name, value, size, initialized) {
}

const void* ArraySymbol::GetValue(const int index) const {
	switch (Symbol::GetType()) {
	case INT_ARRAY:
		return (void *) &((int*) Symbol::GetValue())[index];
		break;
	case DOUBLE_ARRAY:
		return (void *) &((double*) Symbol::GetValue())[index];
		break;
	case STRING_ARRAY: {
		string** strings = (string**) Symbol::GetValue();
		return (void *) strings[index];
	}
		break;
		break;
	default:
		assert(false);
		return NULL;
	}
}

string ArraySymbol::ToString() const {
	ostringstream os;

	Type type = GetType();
	const string name = GetName();

	os << type << " " << name << ":" << endl;
	switch (type) {
	case INT_ARRAY:
		for (int i = 0; i < m_size; i++) {
			os << "  [" << i << "] " << *((int *) GetValue(i)) << endl;
		}
		break;
	case DOUBLE_ARRAY:
		for (int i = 0; i < m_size; i++) {
			os << "  [" << i << "] " << *((double *) GetValue(i)) << endl;
		}
		break;
	case STRING_ARRAY:
		for (int i = 0; i < m_size; i++) {
			os << "  [" << i << "] \"" << *((string *) GetValue(i)) << "\""
					<< endl;
		}
		break;
	default:
		assert(false);
	}
	os << "end array " << name << endl;

	return os.str();
}

const ArraySymbol* ArraySymbol::WithValue(const int index, int* value) const {
	if (!m_initialized) {
		assert(false);
	}

	if (GetType() != INT_ARRAY) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	int* existing = (int*) as_symbol->GetValue();
	*(existing + index) = *value;
	return new ArraySymbol(GetName(), existing, GetSize(), m_initialized);
}

const ArraySymbol* ArraySymbol::WithValue(const int index,
		double* value) const {
	if (!m_initialized) {
		assert(false);
	}

	if (GetType() != DOUBLE_ARRAY) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	double* existing = (double*) as_symbol->GetValue();
	*(existing + index) = *value;
	return new ArraySymbol(GetName(), existing, GetSize(), m_initialized);
}

const ArraySymbol* ArraySymbol::WithValue(const int index,
		string* value) const {
	if (!m_initialized) {
		assert(false);
	}

	if (GetType() != STRING_ARRAY) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	const string** existing = (const string**) as_symbol->GetValue();
	existing[index] = value;
	return new ArraySymbol(GetName(), existing, GetSize(), m_initialized);
}

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

ArraySymbol::ArraySymbol(const string name, const int value[], const int size,
		const bool initialized) :
		Symbol(ARRAY, name, (const void*) value), m_element_type(INT), m_size(
				size), m_initialized(initialized) {
}

ArraySymbol::ArraySymbol(const string name, const double value[],
		const int size, const bool initialized) :
		Symbol(ARRAY, name, (const void*) value), m_element_type(DOUBLE), m_size(
				size), m_initialized(initialized) {
}

ArraySymbol::ArraySymbol(const string name, const string* value[],
		const int size, const bool initialized) :
		Symbol(ARRAY, name, (const void*) value), m_element_type(STRING), m_size(
				size), m_initialized(initialized) {
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
	switch (m_element_type) {
	case INT:
		return (void *) &((int*) Symbol::GetValue())[index];
		break;
	case DOUBLE:
		return (void *) &((double*) Symbol::GetValue())[index];
		break;
	case STRING: {
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

	BasicType type = GetElementType();
	const string name = GetName();

	os << type << " array " << name << ":" << endl;
	switch (m_element_type) {
	case INT:
		for (int i = 0; i < m_size; i++) {
			os << "\t[" << i << "] " << *((int *) GetValue(i)) << endl;
		}
		break;
	case DOUBLE:
		for (int i = 0; i < m_size; i++) {
			os << "\t[" << i << "] " << *((double *) GetValue(i)) << endl;
		}
		break;
	case STRING:
		for (int i = 0; i < m_size; i++) {
			os << "\t[" << i << "] \"" << *((string *) GetValue(i)) << "\""
					<< endl;
		}
		break;
	default:
		assert(false);
	}
	os << "end array " << name;

	return os.str();
}

const ArraySymbol* ArraySymbol::WithValue(const int index, int* value) const {
	if (!m_initialized) {
		assert(false);
	}

	if (m_element_type != INT) {
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

	if (m_element_type != DOUBLE) {
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

	if (m_element_type != STRING) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	const string** existing = (const string**) as_symbol->GetValue();
	existing[index] = value;
	return new ArraySymbol(GetName(), existing, GetSize(), m_initialized);
}

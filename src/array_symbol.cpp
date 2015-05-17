/*
 * array_symbol.cpp
 *
 *  Created on: May 17, 2015
 *      Author: caleb
 */

#include <sstream>

#include "assert.h"
#include "array_symbol.h"

ArraySymbol::ArraySymbol(const string name, const int value[], int size) :
		Symbol(INT_ARRAY, name, (const void*) value) {
	this->size = size;
}

ArraySymbol::ArraySymbol(const string name, const double value[], int size) :
		Symbol(DOUBLE_ARRAY, name, (const void*) value) {
	this->size = size;
}

ArraySymbol::ArraySymbol(const string name, const string* value[], int size) :
		Symbol(STRING_ARRAY, name, (const void*) value) {
	this->size = size;
}

ArraySymbol::ArraySymbol(const string* name, const int value[], int size) :
		ArraySymbol(*name, value, size) {
}

ArraySymbol::ArraySymbol(const string* name, const double value[], int size) :
		ArraySymbol(*name, value, size) {
}

ArraySymbol::ArraySymbol(const string* name, const string* value[], int size) :
		ArraySymbol(*name, value, size) {
}

const int ArraySymbol::GetSize() const {
	return size;
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

	switch (type) {
	case INT_ARRAY:
		for (int i = 0; i < size; i++) {
			os << type << " " << name << "[" << i << "] "
					<< *((int *) GetValue(i)) << "\n";
		}
		break;
	case DOUBLE_ARRAY:
		for (int i = 0; i < size; i++) {
			os << type << " " << name << "[" << i << "] "
					<< *((double *) GetValue(i)) << "\n";
		}
		break;
	case STRING_ARRAY:
		for (int i = 0; i < size; i++) {
			os << type << " " << name << "[" << i << "] \""
					<< *((string *) GetValue(i)) << "\"\n";
		}
		break;
		/*case GAME_OBJECT_ARRAY:
		 for (int i = 0; i < size; i++) {
		 os << type << " " << name << "[" << i << "]\n";
		 indent++;
		 os << *((Game_object *) GetValue(i)) << "\n";
		 indent--;
		 }
		 break;*/
	default:
		assert(false);
	}

	return os.str();
}

const ArraySymbol* ArraySymbol::WithValue(const int index, int* value) const {
	if (GetType() != INT_ARRAY) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	int* existing = (int*) as_symbol->GetValue();
	*(existing + index) = *value;
	return new ArraySymbol(GetName(), existing, GetSize());
}

const ArraySymbol* ArraySymbol::WithValue(const int index,
		double* value) const {
	if (GetType() != DOUBLE_ARRAY) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	double* existing = (double*) as_symbol->GetValue();
	*(existing + index) = *value;
	return new ArraySymbol(GetName(), existing, GetSize());
}

const ArraySymbol* ArraySymbol::WithValue(const int index,
		string* value) const {
	if (GetType() != STRING_ARRAY) {
		assert(false);
		return this;
	}

	Symbol* as_symbol = (Symbol*) this;
	const string** existing = (const string**) as_symbol->GetValue();
	existing[index] = value;
	return new ArraySymbol(GetName(), existing, GetSize());
}

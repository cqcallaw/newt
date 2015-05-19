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

const std::string ArraySymbol::DefaultArraySymbolName = std::string(
		"[!!_DEFAULT_ARRAY_SYMBOL_!!]");

const ArraySymbol* ArraySymbol::DefaultArraySymbol = new ArraySymbol(
		DefaultArraySymbolName, new int[0](), 0);

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

const ArraySymbol* ArraySymbol::GetSymbol(const Type type, const string* name,
		const Expression* size_expression) {
	ArraySymbol* result = (ArraySymbol*) DefaultArraySymbol;

	const Symbol* existing_symbol = Symbol_table::instance()->GetSymbol(name);

	if (existing_symbol != Symbol::DefaultSymbol) {
		Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, *name);
	} else if (size_expression->GetType() != INT) {
		Error::error(Error::INVALID_ARRAY_SIZE, *name,
				*(size_expression->ToString()));
	} else {
		const void* evaluation = size_expression->Evaluate();
		if (evaluation != NULL) {
			int array_size = *((int*) (evaluation));

			if (array_size <= 0) {
				ostringstream convert;
				convert << array_size;
				Error::error(Error::INVALID_ARRAY_SIZE, *name, convert.str());
			} else {
				switch (type) {
				case INT: {
					int* array = new int[array_size]();

					for (int i = 0; i < array_size; i++) {
						array[i] = 0;
					}

					result = new ArraySymbol(name, array, array_size);
					break;
				}
				case DOUBLE: {
					double* array = new double[array_size]();

					for (int i = 0; i < array_size; i++) {
						array[i] = 0.0;
					}

					result = new ArraySymbol(name, array, array_size);
					break;
				}
				case STRING: {
					const string** array =
							(const string**) new string*[array_size]();

					for (int i = 0; i < array_size; i++) {
						array[i] = new string("");
					}

					result = new ArraySymbol(name, array, array_size);
					break;
				}
				default:
					break;
				}
			}
		}
	}

	return result;
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

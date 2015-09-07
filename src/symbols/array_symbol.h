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

#ifndef ARRAY_SYMBOL_H_
#define ARRAY_SYMBOL_H_

#include "symbol.h"
#include "yyltype.h"
#include <vector>
#include <sstream>
#include <type.h>
#include <error.h>
#include <assert.h>

class ArraySymbol: public Symbol {
public:
	ArraySymbol(const string name, const BasicType element_type) :
			ArraySymbol(name, element_type, GetStorage(element_type, 0), true,
					false) {
	}

	ArraySymbol(const string name, const BasicType element_type, const int size,
			const bool initialized) :
			ArraySymbol(name, element_type, GetStorage(element_type, size),
					initialized, true) {
	}

	string ToString() const override {
		ostringstream os;

		BasicType type = GetElementType();
		const string name = GetName();

		int size = GetSize();
		os << type << " array " << name << ":" << endl;
		switch (m_element_type) {
		case INT: {
			for (int i = 0; i < size; i++) {
				os << "\t[" << i << "] " << *((int *) GetValue<const int*>(i))
						<< endl;
			}
			break;
		}
		case DOUBLE: {
			for (int i = 0; i < size; i++) {
				os << "\t[" << i << "] "
						<< *((double *) GetValue<const double*>(i)) << endl;
			}
			break;
		}
		case STRING: {
			for (int i = 0; i < size; i++) {
				os << "\t[" << i << "] \""
						<< *((string *) GetValue<const string*>(i)) << "\""
						<< endl;
			}
			break;
		}
		default:
			break;
		}
		os << "end array " << name;

		return os.str();
	}

	template<class T> const T GetValue(const int index) const {
		if (0 <= index && index < GetSize()) {
			const T result = GetVector<T>()->at(index);
			return result;
		} else {
			return (T) DefaultTypeValue(m_element_type);
		}
	}

	template<class T> const ArraySymbol* WithValue(const int index,
			const T value) const {
		vector<T>* new_vector = new vector<T>(*(GetVector<T>()));
		if (index < GetSize()) {
			new_vector->at(index) = value;
		} else {
			new_vector->resize(index, (T) DefaultTypeValue(m_element_type));
			new_vector->insert(new_vector->end(), value);
		}
		return new ArraySymbol(GetName(), GetElementType(), (void*) new_vector,
				m_initialized, m_fixed_size);
	}

	template<class T> const ArraySymbol* WithAppendedValue(
			const T value) const {
		vector<T>* new_vector = new vector<T>(GetVector<T>());
		new_vector->push_back(value);
		return new ArraySymbol(GetName(), GetType(), new_vector);
	}

	const int GetSize() const {
		return ((vector<const void*>*) Symbol::GetValue())->size();
	}

	const BasicType GetElementType() const {
		return m_element_type;
	}

	const bool IsFixedSize() const {
		return m_fixed_size;
	}

	const bool IsInitialized() const {
		return m_initialized;
	}

private:
	ArraySymbol(const string name, const BasicType element_type,
			const void* value, const bool initialized, const bool fixed_size) :
			Symbol(ARRAY, name, value), m_element_type(element_type), m_initialized(
					initialized), m_fixed_size(fixed_size) {
	}

	const static void* GetStorage(const BasicType element_type,
			const int initial_size) {
		switch (element_type) {
		case INT:
			return new vector<const int*>(initial_size,
					(int*) DefaultTypeValue(element_type));
		case DOUBLE:
			return new vector<const double*>(initial_size,
					(double*) DefaultTypeValue(element_type));
		case STRING:
			return new vector<const string*>(initial_size,
					(string*) DefaultTypeValue(element_type));
		case STRUCT:
			return new vector<const Struct*>(initial_size,
					(Struct*) DefaultTypeValue(element_type));
		default:
			assert(false);
			return nullptr;
		}
	}

	const BasicType m_element_type;
	const bool m_initialized;
	const bool m_fixed_size;

	template<class T> const vector<T>* GetVector() const {
		return ((const vector<T>*) Symbol::GetValue());
	}
};

template<class T> std::ostream &operator<<(std::ostream &os,
		const ArraySymbol &symbol);

#endif /* ARRAY_SYMBOL_H_ */

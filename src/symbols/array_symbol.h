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
#include <type_table.h>
#include <error.h>
#include <assert.h>
#include <type_specifier.h>
#include <primitive_type_specifier.h>
#include <compound_type_specifier.h>
#include <array_type_specifier.h>

class ArraySymbol: public Symbol {
public:
	ArraySymbol(const string name, const TypeSpecifier* element_specifier,
			const TypeTable* type_table) :
			ArraySymbol(name, element_specifier,
					GetStorage(element_specifier, 0, type_table), true, false) {
	}

	ArraySymbol(const string name, const TypeSpecifier* element_specifier,
			const TypeTable* type_table, const int size, const bool initialized) :
			ArraySymbol(name, element_specifier,
					GetStorage(element_specifier, size, type_table),
					initialized, true) {
	}

	const string ToString(const TypeTable* type_table,
			const Indent indent) const override {
		ostringstream os;

		//TODO: recursive dump
		/*BasicType type = GetElementType();
		 const string name = GetName();

		 int size = GetSize();
		 os << type << " array " << name << ":" << endl;
		 switch (m_element_specifier) {
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
		 os << "end array " << name;*/

		return os.str();
	}

	template<class T> const void* GetValue(const int index,
			const TypeTable* type_table) const {
		if (0 <= index && index < GetSize()) {
			const void* result = GetVector<T>()->at(index);
			return result;
		} else {
			return m_element_specifier->DefaultValue(type_table);
		}
	}

	template<class T> const ArraySymbol* WithValue(const int index,
			const T value, const TypeTable* type_table) const {
		vector<T>* new_vector = new vector<T>(*(GetVector<T>()));
		if (index < GetSize()) {
			new_vector->at(index) = value;
		} else {
			new_vector->resize(index,
					(T) m_element_specifier->DefaultValue(type_table));
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

	const TypeSpecifier* GetElementType() const {
		return m_element_specifier;
	}

	const bool IsFixedSize() const {
		return m_fixed_size;
	}

	const bool IsInitialized() const {
		return m_initialized;
	}

private:
	ArraySymbol(const string name, const TypeSpecifier* element_specifier,
			const void* value, const bool initialized, const bool fixed_size) :
			Symbol(PrimitiveTypeSpecifier::ARRAY, name, value), m_element_specifier(
					element_specifier), m_initialized(initialized), m_fixed_size(
					fixed_size) {
	}

	const static void* GetStorage(const TypeSpecifier* element_specifier,
			const int initial_size, const TypeTable* type_table) {
		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(element_specifier);

		if (as_primitive != nullptr) {
			const BasicType element_type = as_primitive->GetBasicType();
			switch (element_type) {
			case INT:
				return new vector<const int*>(initial_size,
						(int*) element_specifier->DefaultValue(type_table));
			case DOUBLE:
				return new vector<const double*>(initial_size,
						(double*) element_specifier->DefaultValue(type_table));
			case STRING:
				return new vector<const string*>(initial_size,
						(string*) element_specifier->DefaultValue(type_table));
			default:
				assert(false);
				return nullptr;
			}
		}

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(element_specifier);
		if (as_compound != nullptr) {
			//TODO: handle arrays of arrays
			//const string* type_name = as_compound->GetTypeName();
			//const CompoundType* type = type_table->GetType(*type_name);
			vector<const CompoundTypeInstance*>* result = new vector<
					const CompoundTypeInstance*>(initial_size);
			for (int i = 0; i < initial_size; ++i) {
				result->at(i) =
						(const CompoundTypeInstance*) element_specifier->DefaultValue(
								type_table);
			}

			return result;
		}

		assert(false);
		return nullptr;
	}

	const TypeSpecifier* m_element_specifier;
	const bool m_initialized;
	const bool m_fixed_size;

	template<class T> const vector<T>* GetVector() const {
		return ((const vector<T>*) Symbol::GetValue());
	}
};

template<class T> std::ostream &operator<<(std::ostream &os,
		const ArraySymbol &symbol);

#endif /* ARRAY_SYMBOL_H_ */

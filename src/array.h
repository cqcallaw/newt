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
#include <symbol.h>

class Array {
public:
	Array(const TypeSpecifier* element_specifier, const TypeTable* type_table) :
			Array(element_specifier,
					GetStorage(element_specifier, 0, type_table), true, false) {
	}

	Array(const TypeSpecifier* element_specifier, const TypeTable* type_table,
			const int size, const bool initialized) :
			Array(element_specifier,
					GetStorage(element_specifier, size, type_table),
					initialized, true) {
	}

	const string ToString(const TypeTable* type_table,
			const Indent indent) const {
		ostringstream os;

		const TypeSpecifier* type = GetElementType();
		int size = GetSize();

		Indent child_indent = indent + 1;

		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(type);
		if (as_primitive != nullptr) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case INT: {
				for (int i = 0; i < size; i++) {
					os << child_indent << "[" << i << "] "
							<< *((int *) GetValue<const int*>(i, type_table))
							<< endl;
				}
				break;
			}
			case DOUBLE: {
				for (int i = 0; i < size; i++) {
					os << child_indent << "[" << i << "] "
							<< *((double *) GetValue<const double*>(i,
									type_table)) << endl;
				}
				break;
			}
			case STRING: {
				for (int i = 0; i < size; i++) {
					os << child_indent << "[" << i << "] \""
							<< *((string *) GetValue<const string*>(i,
									type_table)) << "\"" << endl;
				}
				break;
			}
			default:
				break;
			}
			os << "end array";
		}

		//TODO: array element types

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(type);
		if (as_compound != nullptr) {
			for (int i = 0; i < size; i++) {
				const CompoundTypeInstance* instance =
						(const CompoundTypeInstance*) GetValue<
								const CompoundTypeInstance*>(i, type_table);
				os << child_indent << "[" << i << "]: " << endl
						<< instance->ToString(type_table, child_indent + 1);
			}
		}

		return os.str();
	}

	template<class T> const void* GetValue(const int index,
			const TypeTable* type_table) const {
		if (0 <= index && index < GetSize()) {
			const void* result = GetValue<T>()->at(index);
			return result;
		} else {
			return m_element_specifier->DefaultValue(type_table);
		}
	}

	template<class T> const Array* WithValue(const int index, const T value,
			const TypeTable* type_table) const {
		vector<T>* new_vector = new vector<T>(*(GetValue<T>()));
		if (index < GetSize()) {
			new_vector->at(index) = value;
		} else {
			new_vector->resize(index,
					(T) m_element_specifier->DefaultValue(type_table));
			new_vector->insert(new_vector->end(), value);
		}
		return new Array(GetElementType(), (void*) new_vector, m_initialized,
				m_fixed_size);
	}

	template<class T> const Array* WithAppendedValue(const T value) const {
		vector<T>* new_vector = new vector<T>(GetValue<T>());
		new_vector->push_back(value);
		return new Array(m_element_specifier, new_vector);
	}

	const int GetSize() const {
		int size = ((vector<const void*>*) m_value)->size();
		return size;
	}

	const TypeSpecifier* GetTypeSpecifier() const {
		return m_type_specifier;
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
	Array(const TypeSpecifier* element_specifier, const void* value,
			const bool initialized, const bool fixed_size) :
			m_type_specifier(
					new ArrayTypeSpecifier(element_specifier, fixed_size)), m_element_specifier(
					element_specifier), m_value(value), m_initialized(
					initialized), m_fixed_size(fixed_size) {
	}

	const static void* GetStorage(const TypeSpecifier* element_specifier,
			const int initial_size, const TypeTable* type_table) {
		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(element_specifier);

		if (as_primitive != nullptr) {
			const BasicType element_type = as_primitive->GetBasicType();
			switch (element_type) {
			case INT: {
				auto result = new vector<const int*>(initial_size,
						(int*) element_specifier->DefaultValue(type_table));
				return result;
			}
			case DOUBLE: {
				auto result = new vector<const double*>(initial_size,
						(double*) element_specifier->DefaultValue(type_table));
				return result;
			}
			case STRING: {
				auto result = new vector<const string*>(initial_size,
						(string*) element_specifier->DefaultValue(type_table));
				return result;
			}
			default:
				assert(false);
				return nullptr;
			}
		}

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(element_specifier);
		if (as_compound != nullptr) {
			//TODO: handle arrays of arrays
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

	const TypeSpecifier* m_type_specifier;
	const TypeSpecifier* m_element_specifier;
	const void* m_value;
	const bool m_initialized;
	const bool m_fixed_size;

	template<class T> const vector<T>* GetValue() const {
		return ((const vector<T>*) m_value);
	}
};

template<class T> std::ostream &operator<<(std::ostream &os,
		const Array &symbol);

#endif /* ARRAY_SYMBOL_H_ */

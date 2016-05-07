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

#ifndef ARRAY_H_
#define ARRAY_H_

#include <vector>
#include <sstream>
#include <type.h>
#include <type_table.h>
#include <error.h>
#include <assert.h>
#include <specifiers/array_type_specifier.h>
#include <specifiers/primitive_type_specifier.h>
#include <specifiers/record_type_specifier.h>
#include <specifiers/type_specifier.h>
#include <symbol.h>

class Array {
public:
	Array(const_shared_ptr<TypeSpecifier> element_specifier,
			const TypeTable& type_table) :
			Array(element_specifier,
					GetStorage(element_specifier, 0, type_table)) {
	}

	const string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	template<class T> const std::shared_ptr<const T> GetValue(const int index,
			const TypeTable& type_table) const {
		if (0 <= index && index < GetSize()) {
			const shared_ptr<const T> result = static_pointer_cast<const T>(
					m_value->at(index));
			return result;
		} else {
			return static_pointer_cast<const T>(
					GetElementTypeSpecifier()->DefaultValue(type_table));
		}
	}

	template<class T> const_shared_ptr<Array> WithValue(const int index,
			shared_ptr<const T> value, const TypeTable& type_table) const {
		auto new_vector = new vector<shared_ptr<const void>>(*m_value);

		if (index < GetSize()) {
			new_vector->at(index) = value;
		} else {
			int old_size = new_vector->size();
			new_vector->resize(index);

			//fill with default values
			for (int i = old_size; i < new_vector->size(); i++) {
				const_shared_ptr<void> default_value =
						GetElementTypeSpecifier()->DefaultValue(type_table);
				new_vector->at(i) = default_value;
			}

			new_vector->insert(new_vector->end(), value);
		}

		const shared_ptr<const vector<shared_ptr<const void>>> wrapper = shared_ptr<const vector<shared_ptr<const void>>>(new_vector);
		return const_shared_ptr<Array>(new Array(GetElementTypeSpecifier(), wrapper));
	}

	const int GetSize() const {
		int size = m_value->size();
		return size;
	}

	const_shared_ptr<TypeSpecifier> GetTypeSpecifier() const {
		return m_type_specifier;
	}

	const_shared_ptr<TypeSpecifier> GetElementTypeSpecifier() const {
		return m_type_specifier->GetElementTypeSpecifier();
	}

private:
	Array(const_shared_ptr<TypeSpecifier> element_specifier,
			const shared_ptr<const vector<shared_ptr<const void>>> value) :
			m_type_specifier(
			const_shared_ptr<ArrayTypeSpecifier>(
					new ArrayTypeSpecifier(element_specifier))), m_value(
			value) {
			}

			static const shared_ptr<const vector<shared_ptr<const void>>> GetStorage(
			const_shared_ptr<TypeSpecifier> element_specifier,
			const int initial_size, const TypeTable& type_table);

			const_shared_ptr<ArrayTypeSpecifier> m_type_specifier;
			const shared_ptr<const vector<shared_ptr<const void>>> m_value;
		};

#endif /* ARRAY_H_ */

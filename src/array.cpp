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

#include <array.h>

const string Array::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;

	const_shared_ptr<TypeSpecifier> element_type = GetElementType();
	int size = GetSize();

	Indent child_indent = indent + 1;

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
					element_type);
	if (as_primitive) {
		const BasicType basic_type = as_primitive->GetBasicType();
		switch (basic_type) {
		case INT: {
			for (int i = 0; i < size; i++) {
				os << child_indent << "[" << i << "] "
						<< *(GetValue<int>(i, type_table)) << endl;
			}
			break;
		}
		case DOUBLE: {
			for (int i = 0; i < size; i++) {
				os << child_indent << "[" << i << "] "
						<< *(GetValue<double>(i, type_table)) << endl;
			}
			break;
		}
		case STRING: {
			for (int i = 0; i < size; i++) {
				os << child_indent << "[" << i << "] \""
						<< *(GetValue<string>(i, type_table)) << "\"" << endl;
			}
			break;
		}
		default:
			break;
		}
		os << "end array";
	}

	const_shared_ptr<ArrayTypeSpecifier> as_array = std::dynamic_pointer_cast<
			const ArrayTypeSpecifier>(element_type);
	if (as_array) {
		for (int i = 0; i < size; i++) {
			auto sub_array = static_pointer_cast<const Array>(
					GetValue<Array>(i, type_table));
			os << child_indent << "[" << i << "]: " << endl
					<< sub_array->ToString(type_table, child_indent);
		}
	}

	const_shared_ptr<RecordTypeSpecifier> as_record =
			std::dynamic_pointer_cast<const RecordTypeSpecifier>(
					element_type);
	if (as_record) {
		for (int i = 0; i < size; i++) {
			auto instance = static_pointer_cast<const Record>(
					GetValue<Record>(i, type_table));
			os << child_indent << "[" << i << "]: " << endl
					<< instance->ToString(type_table, child_indent + 1);
		}
	}

	return os.str();
}

const shared_ptr<const vector<shared_ptr<const void>>> Array::GetStorage(
		const_shared_ptr<TypeSpecifier> element_specifier,
		const int initial_size, const TypeTable& type_table) {
	auto storage = new vector<shared_ptr<const void>>(initial_size, element_specifier->DefaultValue(
					type_table));
	auto result = const_shared_ptr<const vector<shared_ptr<const void>>>(storage);
	return result;
}


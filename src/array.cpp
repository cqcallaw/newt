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

const string Array::ToString(const TypeTable* type_table,
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
						<< *((double *) GetValue<const double*>(i, type_table))
						<< endl;
			}
			break;
		}
		case STRING: {
			for (int i = 0; i < size; i++) {
				os << child_indent << "[" << i << "] \""
						<< *((string *) GetValue<const string*>(i, type_table))
						<< "\"" << endl;
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

const void* Array::GetStorage(const TypeSpecifier* element_specifier,
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


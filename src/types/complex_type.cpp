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

#include <complex_type.h>
#include <specifiers/type_specifier.h>
#include <type_table.h>
#include <sum_type_specifier.h>
#include <record_type_specifier.h>
#include <sum_type.h>

ComplexType::~ComplexType() {
}

const_shared_ptr<TypeSpecifier> ComplexType::ToActualType(
		const_shared_ptr<TypeSpecifier> original, const TypeTable& table) {
	auto as_record = dynamic_pointer_cast<const RecordTypeSpecifier>(original);
	if (as_record) {
		//check to see if our "record" type is actually a sum type
		auto sum_type = table.GetType<SumType>(as_record->GetTypeName());
		if (sum_type) {
			return make_shared<SumTypeSpecifier>(as_record);
		}
	}

	return original;
}

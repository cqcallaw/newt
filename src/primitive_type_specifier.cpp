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

#include <primitive_type_specifier.h>

const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::NONE =
		new PrimitiveTypeSpecifier(BasicType::NONE);
const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::BOOLEAN =
		new PrimitiveTypeSpecifier(BasicType::BOOLEAN);
const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::INT =
		new PrimitiveTypeSpecifier(BasicType::INT);
const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::DOUBLE =
		new PrimitiveTypeSpecifier(BasicType::DOUBLE);
const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::STRING =
		new PrimitiveTypeSpecifier(BasicType::STRING);
const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::ARRAY =
		new PrimitiveTypeSpecifier(BasicType::ARRAY);
const PrimitiveTypeSpecifier* PrimitiveTypeSpecifier::COMPOUND =
		new PrimitiveTypeSpecifier(BasicType::COMPOUND);

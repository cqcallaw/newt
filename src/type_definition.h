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

#ifndef TYPE_DEFINITION_H_
#define TYPE_DEFINITION_H_

#include <string>
#include <defaults.h>

class TypeTable;
class Indent;
class TypeSpecifier;

class TypeDefinition {
public:
	TypeDefinition();
	virtual ~TypeDefinition();

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const = 0;

	virtual const_shared_ptr<TypeSpecifier> GetMemberType(
			const std::string& member_name) const = 0;

};

#endif /* TYPE_DEFINITION_H_ */

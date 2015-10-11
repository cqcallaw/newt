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

#ifndef TYPE_SPECIFIER_H_
#define TYPE_SPECIFIER_H_

#include <string>
#include <yyltype.h>

class TypeTable;

using namespace std;
class TypeSpecifier {
public:
	virtual ~TypeSpecifier() {
	}

	virtual const string ToString() const = 0;
	virtual const bool IsAssignableTo(const TypeSpecifier* other) const = 0;
	virtual const void* DefaultValue(const TypeTable* type_table) const = 0;
};

ostream &operator<<(ostream &os, const TypeSpecifier &type_specifier);

#endif /* TYPE_SPECIFIER_H_ */

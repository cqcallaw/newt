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

#ifndef STRUCT_H_
#define STRUCT_H_

#include <string>
#include <symbol_context.h>

using namespace std;

class Struct {
public:
	Struct(const string type_name, SymbolContext* definition) :
			m_type_name(type_name), m_definition(definition) {
	}

	const string GetTypeName() const {
		return m_type_name;
	}

	SymbolContext* GetDefinition() const {
		return m_definition;
	}

private:
	const string m_type_name;
	SymbolContext* m_definition;
};

#endif /* STRUCT_H_ */

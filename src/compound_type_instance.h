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

#ifndef COMPOUND_TYPE_INSTANCE_H_
#define COMPOUND_TYPE_INSTANCE_H_

#include <string>
#include <linked_list.h>
#include <type.h>

class CompoundType;
class CompoundTypeSpecifier;
class SymbolContext;
class Symbol;
class Indent;

using namespace std;

class CompoundTypeInstance {
public:
	CompoundTypeInstance(const CompoundTypeSpecifier* type,
			SymbolContext* definition) :
			m_type(type), m_definition(definition) {
	}

	const CompoundTypeSpecifier* GetTypeSpecifier() const {
		return m_type;
	}

	SymbolContext* GetDefinition() const {
		return m_definition;
	}

	const static CompoundTypeInstance* GetDefaultInstance(
			const string type_name,
			const LinkedList<const SymbolContext*>* parent_context,
			const CompoundType* type);

	const static Symbol* GetSymbol(const TypeSpecifier* member_type,
			const string member_name, const void* void_value);

	const string ToString(const TypeTable* type_table,
			const Indent indent) const;

private:
	const CompoundTypeSpecifier* m_type;
	SymbolContext* m_definition;
};

#endif /* COMPOUND_TYPE_INSTANCE_H_ */

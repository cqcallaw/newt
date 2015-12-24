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

#include <defaults.h>
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
	CompoundTypeInstance(const_shared_ptr<CompoundTypeSpecifier> type,
			volatile_shared_ptr<SymbolContext> definition) :
			m_type(type), m_definition(definition) {
	}

	const_shared_ptr<CompoundTypeSpecifier> GetTypeSpecifier() const {
		return m_type;
	}

	volatile_shared_ptr<SymbolContext> GetDefinition() const {
		return m_definition;
	}

	static const_shared_ptr<CompoundTypeInstance> GetDefaultInstance(
			const string type_name, const_shared_ptr<CompoundType> type);

	const static const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<TypeSpecifier> member_type,
			const_shared_ptr<void> void_value);

	const string ToString(const TypeTable& type_table,
			const Indent indent) const;

private:
	const_shared_ptr<CompoundTypeSpecifier> m_type;
	volatile_shared_ptr<SymbolContext> m_definition;
};

#endif /* COMPOUND_TYPE_INSTANCE_H_ */

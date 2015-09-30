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

#ifndef SYMBOL_CONTEXT_H_
#define SYMBOL_CONTEXT_H_

#include <iostream>
#include "symbol.h"
#include <array_symbol.h>
#include <map>
#include <modifier.h>

class CompoundTypeInstance;

using namespace std;

enum SetResult {
	NO_SET_RESULT = 0,
	SET_SUCCESS = 1,
	UNDEFINED_SYMBOL = 2,
	INCOMPATIBLE_TYPE = 3,
	MUTATION_DISALLOWED = 4
};

const LinkedList<const Error*>* ToErrorList(const SetResult result,
		const YYLTYPE location, const string* name);

struct comparator {
	bool operator()(const string lhs, const string rhs) const {
		return lhs < rhs;
	}
};

class SymbolContext {
public:
	SymbolContext(const Modifier::Type modifiers,
			const LinkedList<SymbolContext*>* parent =
					LinkedList<SymbolContext*>::Terminator);
	SymbolContext(const Modifier::Type modifiers,
			const LinkedList<SymbolContext*>* parent_context,
			map<const string, const Symbol*, comparator>* values);
	virtual ~SymbolContext();

	const Modifier::Type GetModifiers() const {
		return m_modifiers;
	}

	const LinkedList<SymbolContext*>* GetParent() const {
		return m_parent;
	}

	map<const string, const Symbol*, comparator>* GetTable() const {
		return m_table;
	}

	const bool IsMutable() const {
		return m_modifiers & Modifier::READONLY;
	}

	const void print(ostream &os, const TypeTable* type_table,
			const Indent indent) const;

	const Symbol* GetSymbol(const string identifier) const;
	const Symbol* GetSymbol(const string* identifier) const;

	SetResult SetSymbol(const string identifier, const bool* value);
	SetResult SetSymbol(const string identifier, const int* value);
	SetResult SetSymbol(const string identifier, const double* value);
	SetResult SetSymbol(const string identifier, const string* value);
	SetResult SetSymbol(const string identifier,
			const CompoundTypeInstance* value);

	SetResult SetArraySymbol(const string identifier,
			const ArraySymbol* new_symbol);

private:
	const Modifier::Type m_modifiers;
	const LinkedList<SymbolContext*>* m_parent;
	map<const string, const Symbol*, comparator>* m_table;

	SetResult SetSymbol(const string identifier, const TypeSpecifier* type,
			const void* value);
};

#endif /* SYMBOL_CONTEXT_H_ */

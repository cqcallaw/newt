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
#include "array_symbol.h"
#include <map>

class Struct;

using namespace std;

enum SetResult {
	NO_SET_RESULT = 0,
	SET_SUCCESS = 1,
	UNDEFINED_SYMBOL = 2,
	INCOMPATIBLE_TYPE = 3
};

struct comparator {
	bool operator()(const string lhs, const string rhs) const {
		return lhs < rhs;
	}
};

class SymbolContext {
public:
	SymbolContext(const bool is_mutable,
			const LinkedList<const SymbolContext*>* parent = LinkedList<
					const SymbolContext*>::Terminator);
	SymbolContext(const LinkedList<const SymbolContext*>* parent_context,
			const map<const string, const Symbol*>* values);
	virtual ~SymbolContext();

	const LinkedList<const SymbolContext*>* GetParent() const {
		return m_parent;
	}

	const void print(ostream &os, const string* line_prefix) const;

	const Symbol* GetSymbol(const string identifier) const;
	const Symbol* GetSymbol(const string* identifier) const;

	const bool IsMutable() const {
		return m_mutable;
	}

	SetResult SetSymbol(const string identifier, const bool* value);
	SetResult SetSymbol(const string identifier, const int* value);
	SetResult SetSymbol(const string identifier, const double* value);
	SetResult SetSymbol(const string identifier, const string* value);
	SetResult SetSymbol(const string identifier, const Struct* value);

	SetResult SetArraySymbol(const string identifier,
			const ArraySymbol* new_symbol);

	SetResult SetSymbol(const string identifier, const int index,
			const bool* value);
	SetResult SetSymbol(const string identifier, const int index,
			const int* value);
	SetResult SetSymbol(const string identifier, const int index,
			const double* value);
	SetResult SetSymbol(const string identifier, const int index,
			const string* value);

//	SetResult SetMember(const string identifier, const string member_name,
//			const BasicType member_type, const void* value);

protected:
	map<const string, const Symbol*, comparator>* GetTable() const {
		return table;
	}

private:
	const bool m_mutable;
	const LinkedList<const SymbolContext*>* m_parent;
	map<const string, const Symbol*, comparator>* table;

	SetResult SetSymbol(const string identifier, const BasicType type,
			const void* value);

	SetResult SetArraySymbolIndex(const string identifier, BasicType type,
			int index, const void* value);
};

#endif /* SYMBOL_CONTEXT_H_ */

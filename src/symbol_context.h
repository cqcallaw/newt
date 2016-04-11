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

#include <array.h>
#include <iostream>
#include <map>
#include <modifier.h>
#include <symbol.h>

class Record;
class Function;

using namespace std;

enum SetResult {
	NO_SET_RESULT = 0,
	SET_SUCCESS = 1,
	UNDEFINED_SYMBOL = 2,
	INCOMPATIBLE_TYPE = 3,
	MUTATION_DISALLOWED = 4
};

const ErrorListRef ToErrorListRef(const SetResult result,
		const yy::location location, const_shared_ptr<string> name,
		const_shared_ptr<TypeSpecifier> symbol_type,
		const_shared_ptr<TypeSpecifier> value_type);

struct comparator {
	bool operator()(const string& lhs, const string& rhs) const {
		return lhs < rhs;
	}
};

typedef map<const string, plain_shared_ptr<Symbol>, comparator> symbol_map;

class SymbolContext {
public:
	SymbolContext(const Modifier::Type modifiers);
	SymbolContext(const SymbolContext&);
	virtual ~SymbolContext();

	volatile_shared_ptr<SymbolContext> Clone() const;

	const Modifier::Type GetModifiers() const {
		return m_modifiers;
	}

	virtual volatile_shared_ptr<SymbolContext> WithModifiers(
			const Modifier::Type modifiers) const {
		return make_shared<SymbolContext>(SymbolContext(modifiers, m_table));
	}

	const bool IsMutable() const {
		return m_modifiers & Modifier::READONLY;
	}

	const void print(ostream &os, const TypeTable& type_table,
			const Indent& indent) const;

	const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<string> identifier) const;
	const_shared_ptr<Symbol> GetSymbol(const string& identifier) const;

	SetResult SetSymbol(const string& identifier, const_shared_ptr<bool> value,
			const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier, const_shared_ptr<int> value,
			const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier,
			const_shared_ptr<double> value, const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier,
			const_shared_ptr<string> value, const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier, const_shared_ptr<Array> value,
			const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier,
			const_shared_ptr<ComplexTypeSpecifier> type,
			const_shared_ptr<Record> value, const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier,
			const_shared_ptr<Function> value, const TypeTable& type_table);
	SetResult SetSymbol(const string& identifier,
			const_shared_ptr<ComplexTypeSpecifier> type,
			const_shared_ptr<Sum> value, const TypeTable& type_table);

	static volatile_shared_ptr<SymbolContext> GetDefault();

protected:
	const shared_ptr<symbol_map> GetTable() const {
		return m_table;
	}

	SymbolContext(const Modifier::Type modifiers,
			const shared_ptr<symbol_map> values);

	virtual SetResult SetSymbol(const string& identifier,
			const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value,
			const TypeTable& type_table);
private:
	const Modifier::Type m_modifiers;
	const shared_ptr<symbol_map> m_table;
};

#endif /* SYMBOL_CONTEXT_H_ */

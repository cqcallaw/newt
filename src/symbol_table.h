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

#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <iostream>
#include "symbol.h"
#include "array_symbol.h"
#include <map>

using namespace std;

enum InsertResult {
	INSERT_SUCCESS = 0, SYMBOL_EXISTS = 1
};

enum SetResult {
	SET_SUCCESS = 0, UNDEFINED_SYMBOL = 1, INCOMPATIBLE_TYPE = 2
};

struct comparator {
	bool operator()(const string lhs, const string rhs) const {
		return lhs < rhs;
	}
};

class Symbol_table {
public:
	static Symbol_table* instance();
	void print(ostream &os);

	const Symbol* GetSymbol(const string identifier);
	const Symbol* GetSymbol(const string* identifier);

	InsertResult InsertSymbol(const Symbol* symbol);

	SetResult SetSymbol(const string identifier, const bool* value);
	SetResult SetSymbol(const string identifier, const int* value);
	SetResult SetSymbol(const string identifier, const double* value);
	SetResult SetSymbol(const string identifier, const string* value);

	SetResult SetSymbol(const string identifier, int index, const bool* value);
	SetResult SetSymbol(const string identifier, int index, const int* value);
	SetResult SetSymbol(const string identifier, int index,
			const double* value);
	SetResult SetSymbol(const string identifier, int index,
			const string* value);

	//compatibility functions for Tyson's test harness
	const void get(const string identifier, int &out);
	const void get(const string identifier, double &out);
	const void get(const string identifier, string &out);
	const bool get_type(const string identifier, Type &out);

	void set(const string identifier, const int value);
	void set(const string identifier, const double value);
	void set(const string identifier, const string value);

private:
	static Symbol_table* m_instance;

	std::map<const string, const Symbol*, comparator>* table;

	SetResult SetSymbol(const string identifier, Type type, const void* value);

	SetResult SetArraySymbol(const string identifier, Type type, int index,
			const void* value);

	Symbol_table();

	// disable default copy constructor and default assignment
	// done as a precaution, they should never be called
	Symbol_table(const Symbol_table &);
	const Symbol_table &operator=(const Symbol_table &);
};

#endif /* SYMBOL_TABLE_H_ */

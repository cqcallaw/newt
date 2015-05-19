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

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#include <iostream>

#include "type.h"

using namespace std;

class Expression;

class Symbol {
public:
	Symbol(const string name, const bool* value);
	Symbol(const string name, const int* value);
	Symbol(const string name, const double* value);
	Symbol(const string name, const string* value);

	Symbol(const string* name, const bool* value);
	Symbol(const string* name, const int* value);
	Symbol(const string* name, const double* value);
	Symbol(const string* name, const string* value);

	const static Symbol* GetSymbol(const Type type, const string* name,
			const Expression* initializer_expression);

	virtual ~Symbol() {
	}

	const Type GetType() const;
	const string GetName() const;
	const void* GetValue() const;

	const Symbol* WithValue(const bool* value) const;
	const Symbol* WithValue(const int* value) const;
	const Symbol* WithValue(const double* value) const;
	const Symbol* WithValue(const string* value) const;
	const Symbol* WithValue(const Type type, const void* value) const;

	virtual string ToString() const;

	const static std::string DefaultSymbolName;
	const static Symbol* DefaultSymbol;

protected:
	Symbol(Type type, const string name, const void* value);

private:
	Type type;
	string name;
	const void* value;
};

std::ostream &operator<<(std::ostream &os, const Symbol &symbol);

#endif /* SYMBOL_H_ */

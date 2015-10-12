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

#include <type.h>
#include <yyltype.h>
#include <indent.h>

using namespace std;

class Expression;
class ExecutionContext;
class Array;
class CompoundTypeInstance;

class Symbol {
public:
	Symbol(const string name, const bool* value);
	Symbol(const string name, const int* value);
	Symbol(const string name, const double* value);
	Symbol(const string name, const string* value);
	Symbol(const string name, const Array* value);
	Symbol(const string name, const CompoundTypeInstance* value);

	Symbol(const string* name, const bool* value);
	Symbol(const string* name, const int* value);
	Symbol(const string* name, const double* value);
	Symbol(const string* name, const string* value);
	Symbol(const string* name, const Array* value);
	Symbol(const string* name, const CompoundTypeInstance* value);

	virtual ~Symbol() {
	}

	const TypeSpecifier* GetType() const;
	const string GetName() const;
	const void* GetValue() const;

	const Symbol* WithValue(const bool* value) const;
	const Symbol* WithValue(const int* value) const;
	const Symbol* WithValue(const double* value) const;
	const Symbol* WithValue(const string* value) const;
	const Symbol* WithValue(const Array* value) const;
	const Symbol* WithValue(const CompoundTypeInstance* value) const;
	const Symbol* WithValue(const TypeSpecifier* type, const void* value) const;

	virtual const string ToString(const TypeTable* type_table,
			const Indent indent) const;

	const static string DefaultSymbolName;
	const static Symbol* DefaultSymbol;

protected:
	Symbol(const TypeSpecifier* type, const string name, const void* value);

private:
	const TypeSpecifier* m_type;
	const string m_name;
	const void* m_value;
};

#endif /* SYMBOL_H_ */

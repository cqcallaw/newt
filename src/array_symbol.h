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

#ifndef ARRAY_SYMBOL_H_
#define ARRAY_SYMBOL_H_

#include "symbol.h"
#include "yyltype.h"

class ExecutionContext;

class ArraySymbol: public Symbol {
public:
	ArraySymbol(const string name, const int value[], const int size,
			const bool initialized);
	ArraySymbol(const string name, const double value[], const int size,
			const bool initialized);
	ArraySymbol(const string name, const string* value[], const int size,
			const bool initialized);

	ArraySymbol(const string* name, const int value[], const int size,
			const bool initialized);
	ArraySymbol(const string* name, const double value[], const int size,
			const bool initialized);
	ArraySymbol(const string* name, const string* value[], const int size,
			const bool initialized);

	virtual string ToString() const override;

	const void* GetValue(const int index) const;

	const ArraySymbol* WithValue(const int index, int* value) const;
	const ArraySymbol* WithValue(const int index, double* value) const;
	const ArraySymbol* WithValue(const int index, string* value) const;

	const bool IsInitialized() const {
		return m_initialized;
	}

	const int GetSize() const {
		return m_size;
	}

private:
	const int m_size;
	const bool m_initialized;
};

std::ostream &operator<<(std::ostream &os, const ArraySymbol &symbol);

#endif /* ARRAY_SYMBOL_H_ */

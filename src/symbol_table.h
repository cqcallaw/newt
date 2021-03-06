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

#include <symbol_context.h>

extern size_t INVOCATION_DEPTH;

enum InsertResult {
	NO_INSERT_RESULT = 0, INSERT_SUCCESS = 1, SYMBOL_EXISTS = 2
};

class SymbolTable: public SymbolContext {
public:
	SymbolTable();
	SymbolTable(const Modifier::Type modifiers);
	SymbolTable(const Modifier::Type modifiers,
			const shared_ptr<symbol_map> values);
	SymbolTable(const SymbolContext& other);

	InsertResult InsertSymbol(const std::string& name,
			const_shared_ptr<Symbol> symbol);
};

#endif /* SYMBOL_TABLE_H_ */

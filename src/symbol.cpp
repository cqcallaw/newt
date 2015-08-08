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

#include <sstream>
#include <defaults.h>

#include "symbol.h"
#include "assert.h"
#include "indent.h"
#include "type.h"
#include "error.h"
#include "expression.h"
#include <execution_context.h>

const std::string Symbol::DefaultSymbolName = std::string("[!!_DEFAULT_!!]");
const Symbol* Symbol::DefaultSymbol = new Symbol(NONE, DefaultSymbolName, NULL);

Symbol::Symbol(const string name, const bool *value) :
		Symbol(BOOLEAN, name, (void *) value) {
}

Symbol::Symbol(const string name, const int *value) :
		Symbol(INT, name, (void *) value) {
}

Symbol::Symbol(const string name, const double *value) :
		Symbol(DOUBLE, name, (void *) value) {
}

Symbol::Symbol(const string name, const string *value) :
		Symbol(STRING, name, (void *) value) {
}

Symbol::Symbol(Type type, string name, const void* value) {
	this->type = type;
	this->name = name;
	this->value = value;
}

Symbol::Symbol(const string* name, const bool *value) :
		Symbol(*name, value) {
}

Symbol::Symbol(const string* name, const int *value) :
		Symbol(*name, value) {
}

Symbol::Symbol(const string* name, const double *value) :
		Symbol(*name, value) {
}

Symbol::Symbol(const string* name, const string *value) :
		Symbol(*name, value) {
}

const Type Symbol::GetType() const {
	return type;
}

const std::string Symbol::GetName() const {
	return name;
}

const void* Symbol::GetValue() const {
	return value;
}

const Symbol* Symbol::WithValue(const bool* value) const {
	return WithValue(BOOLEAN, (void*) value);
}
const Symbol* Symbol::WithValue(const int* value) const {
	return WithValue(INT, (void*) value);
}
const Symbol* Symbol::WithValue(const double* value) const {
	return WithValue(DOUBLE, (void*) value);
}
const Symbol* Symbol::WithValue(const string* value) const {
	return WithValue(STRING, (void*) value);
}

const Symbol* Symbol::WithValue(const Type type, const void* value) const {
	if (type > this->type) {
		return DefaultSymbol;
	}

	return new Symbol(type, name, value);
}

string Symbol::ToString() const {
	ostringstream os;

	os << type << " " << name;
	switch (type) {
	case BOOLEAN:
		os << " " << *((bool*) value) << "\n";
		break;
	case INT:
		os << " " << *((int*) value) << "\n";
		break;
	case DOUBLE:
		os << " " << *((double*) value) << "\n";
		break;
	case STRING:
		os << " \"" << *((string*) value) << "\"\n";
		break;
	default:
		assert(false);
	}

	return os.str();
}

ostream &operator<<(ostream &os, const Symbol &symbol) {
	os << symbol.ToString();
	return os;
}

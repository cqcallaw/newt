/*
 * symbol.h
 *
 *  Created on: Feb 10, 2015
 *      Author: caleb
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

/*
 * array_symbol.h
 *
 *  Created on: May 17, 2015
 *      Author: caleb
 */

#ifndef ARRAY_SYMBOL_H_
#define ARRAY_SYMBOL_H_

#include "symbol.h"

class ArraySymbol: public Symbol {
public:
	ArraySymbol(const string name, const int value[], int size);
	ArraySymbol(const string name, const double value[], int size);
	ArraySymbol(const string name, const string* value[], int size);

	ArraySymbol(const string* name, const int value[], int size);
	ArraySymbol(const string* name, const double value[], int size);
	ArraySymbol(const string* name, const string* value[], int size);

	const static ArraySymbol* GetSymbol(const Type type, const string* name,
			const Expression* size_expression);

	virtual string ToString() const override;

	const void* GetValue(const int index) const;

	const ArraySymbol* WithValue(const int index, int* value) const;
	const ArraySymbol* WithValue(const int index, double* value) const;
	const ArraySymbol* WithValue(const int index, string* value) const;

	const int GetSize() const;

	const static std::string DefaultArraySymbolName;
	const static ArraySymbol* DefaultArraySymbol;

private:
	int size;
};

std::ostream &operator<<(std::ostream &os, const ArraySymbol &symbol);

#endif /* ARRAY_SYMBOL_H_ */

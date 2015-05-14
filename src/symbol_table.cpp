#include <iostream>
#include <sstream>
#include "symbol_table.h"

#include "type.h"
#include "utils.h"

using namespace std;

Symbol_table * Symbol_table::m_instance = NULL;

Symbol_table::Symbol_table() {
	table = new map<const string, const Symbol*, comparator>();
}

Symbol_table* Symbol_table::instance() {
	if (Symbol_table::m_instance == NULL)
		Symbol_table::m_instance = new Symbol_table();
	return Symbol_table::m_instance;
}

const Symbol* Symbol_table::GetSymbol(const string identifier) {
	auto result = table->find(identifier);

	if (result != table->end()) {
		return result->second;
	}

	return Symbol::DefaultSymbol;
}

const Symbol* Symbol_table::GetSymbol(const string* identifier) {
	return GetSymbol(*identifier);
}
InsertResult Symbol_table::InsertSymbol(const Symbol* symbol) {
	std::map<const string, const Symbol*>::iterator search_result;

	search_result = table->find(symbol->GetName());

	if (search_result != table->end()) {
		return SYMBOL_EXISTS;
	} else {
		table->insert(
				std::pair<const string, const Symbol*>(symbol->GetName(),
						symbol));

		return INSERT_SUCCESS;
	}
}

SetResult Symbol_table::SetSymbol(const string identifier, const bool* value) {
	return SetSymbol(identifier, BOOLEAN, (void*) value);
}
SetResult Symbol_table::SetSymbol(const string identifier, const int* value) {
	return SetSymbol(identifier, INT, (void*) value);
}
SetResult Symbol_table::SetSymbol(const string identifier,
		const double* value) {
	return SetSymbol(identifier, DOUBLE, (void*) value);
}
SetResult Symbol_table::SetSymbol(const string identifier,
		const string* value) {
	return SetSymbol(identifier, STRING, (void*) value);
}

SetResult Symbol_table::SetSymbol(const string identifier, Type type,
		const void* value) {
	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	} else if ((symbol->GetType() < STRING && symbol->GetType() > type)
			|| (symbol->GetType() != type)) {
		return INCOMPATIBLE_TYPE;
	}

	//TODO: error checking
	//TODO: free memory from old symbols
	const Symbol* new_symbol = symbol->WithValue(type, value);
	table->erase(identifier);
	table->insert(
			std::pair<const string, const Symbol*>(identifier, new_symbol));

	return SET_SUCCESS;
}

SetResult Symbol_table::SetSymbol(const string identifier, int index,
		const bool* value) {
	return SetArraySymbol(identifier, BOOLEAN, index, (void*) value);
}
SetResult Symbol_table::SetSymbol(const string identifier, int index,
		const int* value) {
	return SetArraySymbol(identifier, INT, index, (void*) value);
}
SetResult Symbol_table::SetSymbol(const string identifier, int index,
		const double* value) {
	return SetArraySymbol(identifier, DOUBLE, index, (void*) value);
}
SetResult Symbol_table::SetSymbol(const string identifier, int index,
		const string* value) {
	return SetArraySymbol(identifier, STRING, index, (void*) value);
}

SetResult Symbol_table::SetArraySymbol(const string identifier, Type type,
		int index, const void* value) {
	const Symbol* symbol = GetSymbol(identifier);

	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return UNDEFINED_SYMBOL;
	}

	Symbol* new_symbol;
	const ArraySymbol* as_array_symbol = (ArraySymbol*) symbol;
	switch (symbol->GetType()) {
	case INT_ARRAY: {
		int* new_value = (int*) malloc(sizeof(int));
		if (type == BOOLEAN) {
			*new_value = *((bool*) value);
		} else if (type == INT) {
			*new_value = *((int*) value);
		} else {
			return INCOMPATIBLE_TYPE;
		}

		new_symbol = (Symbol*) as_array_symbol->WithValue(index, new_value);
		break;
	}
	case DOUBLE_ARRAY: {
		double* new_value = (double*) malloc(sizeof(double));
		if (type == BOOLEAN) {
			*new_value = *((bool*) value);
		} else if (type == INT) {
			*new_value = *((int*) value);
		} else if (type == DOUBLE) {
			*new_value = *((double*) value);
		} else {
			return INCOMPATIBLE_TYPE;
		}

		new_symbol = (Symbol*) as_array_symbol->WithValue(index, new_value);
		break;
	}
	case STRING_ARRAY: {
		string* new_value;
		if (type == BOOLEAN) {
			new_value = AsString((bool*) value);
		} else if (type == INT) {
			new_value = AsString((int*) value);
		} else if (type == DOUBLE) {
			new_value = AsString((double*) value);
		} else if (type == STRING) {
			new_value = (string*) value;
		} else {
			return INCOMPATIBLE_TYPE;
		}

		new_symbol = (Symbol*) as_array_symbol->WithValue(index, new_value);
		break;
	}
	default:
		return INCOMPATIBLE_TYPE;
	}

	//TODO: error checking
	//TODO: free memory from old symbols
	table->erase(identifier);
	table->insert(
			std::pair<const string, const Symbol*>(identifier, new_symbol));

	return SET_SUCCESS;
}

void Symbol_table::set(const string identifier, const int value) {
	SetSymbol(identifier, new int(value));
}
void Symbol_table::set(const string identifier, const double value) {
	SetSymbol(identifier, new double(value));
}
void Symbol_table::set(const string identifier, const string value) {
	SetSymbol(identifier, new string(value));
}

const void Symbol_table::get(const string identifier, int &out) {
	const Symbol* symbol = GetSymbol(identifier);
	if (symbol == Symbol::DefaultSymbol || symbol == NULL
			|| symbol->GetType() > INT) {
		out = 0;
	} else {
		out = *((int*) symbol->GetValue());
	}
}
const void Symbol_table::get(const string identifier, double &out) {
	const Symbol* symbol = GetSymbol(identifier);
	if (symbol == Symbol::DefaultSymbol || symbol == NULL
			|| symbol->GetType() > DOUBLE) {
		out = 0.0;
	} else {
		out = *((double*) symbol->GetValue());
	}
}
const void Symbol_table::get(const string identifier, string &out) {
	const Symbol* symbol = GetSymbol(identifier);
	if (symbol == Symbol::DefaultSymbol || symbol == NULL
			|| symbol->GetType() > STRING) {
		out = "";
	} else {
		out = *((string*) symbol->GetValue());
	}
}
const bool Symbol_table::get_type(const string identifier, Type &out) {
	const Symbol* symbol = GetSymbol(identifier);
	if (symbol == Symbol::DefaultSymbol || symbol == NULL) {
		return false;
	}
	out = symbol->GetType();
	return true;
}

void Symbol_table::print(ostream &os) {
	std::map<const string, const Symbol*>::iterator iter;
	for (iter = table->begin(); iter != table->end(); ++iter) {
		Symbol* symbol = (Symbol *) iter->second;
		os << *symbol;
	}
}

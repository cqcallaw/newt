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

#include <compound_type_instance.h>
#include <iostream>
#include <sstream>
#include <defaults.h>
#include <symbol_context.h>
#include "symbol_table.h"

#include "type.h"
#include "utils.h"

const LinkedList<const Error*>* ToErrorList(const SetResult result,
		const YYLTYPE location, const string* name) {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	switch (result) {
	case NO_SET_RESULT:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::DEFAULT_ERROR_CODE,
						location.first_line, location.first_column, *name));
		break;
	case UNDEFINED_SYMBOL:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						location.first_line, location.first_column, *name));
		break;
	case INCOMPATIBLE_TYPE:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::ASSIGNMENT_TYPE_ERROR,
						location.first_line, location.first_column, *name));
		break;
	case MUTATION_DISALLOWED:
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::READONLY, location.first_line,
						location.first_column, *name));
		break;
	case SET_SUCCESS:
	default:
		break;
	}

	return errors;
}

SymbolContext::SymbolContext(const Modifier::Type modifiers,
		const LinkedList<SymbolContext*>* parent) :
		SymbolContext(modifiers, parent,
				new map<const string, const Symbol*, comparator>()) {
}

SymbolContext::~SymbolContext() {
}

const Symbol* SymbolContext::GetSymbol(const string identifier) const {
	auto result = m_table->find(identifier);

	if (result != m_table->end()) {
		return result->second;
	} else if (m_parent != nullptr) {
		return m_parent->GetData()->GetSymbol(identifier);
	} else {
		return Symbol::DefaultSymbol;
	}
}

const Symbol* SymbolContext::GetSymbol(const string* identifier) const {
	const Symbol* result = GetSymbol(*identifier);
	return result;
}

SymbolContext::SymbolContext(const Modifier::Type modifiers,
		const LinkedList<SymbolContext*>* parent,
		map<const string, const Symbol*, comparator>* values) :
		m_modifiers(modifiers), m_parent(parent), m_table(values) {
}

const void SymbolContext::print(ostream &os, const TypeTable* type_table,
		const Indent indent) const {
	std::map<const string, const Symbol*>::iterator iter;
	for (iter = m_table->begin(); iter != m_table->end(); ++iter) {
		const Symbol* symbol = iter->second;
		os << symbol->ToString(type_table, indent);
		os << endl;
	}
}

SetResult SymbolContext::SetSymbol(const string identifier, const bool* value) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetBoolean(),
			(void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier, const int* value) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetInt(),
			(void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier,
		const double* value) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetDouble(),
			(void*) value);
}
SetResult SymbolContext::SetSymbol(const string identifier,
		const string* value) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetString(),
			(void*) value);
}

SetResult SymbolContext::SetSymbol(const string identifier,
		const CompoundTypeInstance* value) {
	return SetSymbol(identifier, value->GetTypeSpecifier(), (void*) value);
}

SetResult SymbolContext::SetSymbol(const string identifier,
		const TypeSpecifier* type, const void* value) {
	auto result = m_table->find(identifier);

	if (result != m_table->end()) {
		const Symbol* symbol = result->second;
		if ((symbol->GetType()->IsAssignableTo(type))) {
			if (m_modifiers & Modifier::READONLY) {
				return MUTATION_DISALLOWED;
			} else {
				const Symbol* new_symbol = symbol->WithValue(type, value);

				//TODO: error checking
				//TODO: free memory from old symbols
				m_table->erase(identifier);
				m_table->insert(
						std::pair<const string, const Symbol*>(identifier,
								new_symbol));

				return SET_SUCCESS;
			}
		} else {
			return INCOMPATIBLE_TYPE;
		}
	} else if (m_parent != nullptr) {
		return m_parent->GetData()->SetSymbol(identifier, type, value);
	} else {
		return UNDEFINED_SYMBOL;
	}
}

SetResult SymbolContext::SetArraySymbol(const string identifier,
		const ArraySymbol* new_symbol) {
	auto result = m_table->find(identifier);

	if (result != m_table->end()) {
		const Symbol* symbol = result->second;
		if ((symbol->GetType()->IsAssignableTo(new_symbol->GetType()))) {
			if (m_modifiers & Modifier::READONLY) {
				return MUTATION_DISALLOWED;
			} else {
				//TODO: error checking
				//TODO: free memory from old symbols
				m_table->erase(identifier);
				m_table->insert(
						std::pair<const string, const Symbol*>(identifier,
								new_symbol));

				return SET_SUCCESS;
			}
		} else {
			return INCOMPATIBLE_TYPE;
		}
	} else if (m_parent != nullptr) {
		return m_parent->GetData()->SetArraySymbol(identifier, new_symbol);
	} else {
		return UNDEFINED_SYMBOL;
	}
}

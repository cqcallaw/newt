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

#include <function.h>
#include <function_declaration.h>
#include <iostream>
#include <sstream>
#include <defaults.h>
#include <symbol_context.h>
#include "symbol_table.h"
#include <execution_context.h>
#include <record.h>
#include <sum.h>
#include <unit.h>
#include <maybe_type_specifier.h>

#include "type.h"
#include "utils.h"

const ErrorListRef ToErrorListRef(const SetResult result,
		const yy::location location, const_shared_ptr<string> name,
		const_shared_ptr<TypeSpecifier> symbol_type,
		const_shared_ptr<TypeSpecifier> value_type) {
	ErrorListRef errors = ErrorList::GetTerminator();

	switch (result) {
	case NO_SET_RESULT:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::DEFAULT_ERROR_CODE,
						location.begin.line, location.begin.column, *name),
				errors);
		break;
	case UNDEFINED_SYMBOL:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_VARIABLE,
						location.begin.line, location.begin.column, *name),
				errors);
		break;
	case INCOMPATIBLE_TYPE:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC,
						Error::ASSIGNMENT_TYPE_ERROR, location.begin.line,
						location.begin.column, symbol_type->ToString(),
						value_type->ToString()), errors);
		break;
	case MUTATION_DISALLOWED:
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::READONLY,
						location.begin.line, location.begin.column, *name),
				errors);
		break;
	case SET_SUCCESS:
	default:
		break;
	}

	return errors;
}

SymbolContext::SymbolContext(const Modifier::Type modifiers) :
		SymbolContext(modifiers, make_shared<symbol_map>()) {
}

SymbolContext::SymbolContext(const Modifier::Type modifiers,
		const shared_ptr<symbol_map> values) :
		m_modifiers(modifiers), m_table(values) {
}

SymbolContext::~SymbolContext() {
}

const_shared_ptr<Symbol> SymbolContext::GetSymbol(
		const string& identifier) const {
	if (identifier == *TypeTable::GetNilName()) {
		return Symbol::GetNilSymbol();
	}

	auto result = m_table->find(identifier);

	if (result != m_table->end()) {
		return result->second;
	} else {
		return Symbol::GetDefaultSymbol();
	}
}

const_shared_ptr<Symbol> SymbolContext::GetSymbol(
		const_shared_ptr<string> identifier) const {
	const_shared_ptr<Symbol> result = GetSymbol(*identifier);
	return result;
}

const void SymbolContext::print(ostream &os, const TypeTable& type_table,
		const Indent& indent) const {
	if (m_table->size() > 0) {
		symbol_map::iterator iter;
		for (iter = m_table->begin(); iter != m_table->end(); ++iter) {
			const string name = iter->first;
			auto symbol = iter->second;

			auto specifier = symbol->GetTypeSpecifier();
			os << indent << specifier->ToString() << " " << name << ":";
			os << symbol->ToString(type_table, indent);
			os << endl;
		}
	} else {
		os << indent << "<empty>" << endl;
	}
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<bool> value, const TypeTable& type_table) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetBoolean(),
			static_pointer_cast<const void>(value), type_table);
}
SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<std::uint8_t> value, const TypeTable& type_table) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetByte(),
			static_pointer_cast<const void>(value), type_table);
}
SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<int> value, const TypeTable& type_table) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetInt(),
			static_pointer_cast<const void>(value), type_table);
}
SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<double> value, const TypeTable& type_table) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetDouble(),
			static_pointer_cast<const void>(value), type_table);
}
SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<string> value, const TypeTable& type_table) {
	return SetSymbol(identifier, PrimitiveTypeSpecifier::GetString(),
			static_pointer_cast<const void>(value), type_table);
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Unit> value, const TypeTable& type_table) {
	return SetSymbol(identifier, type, static_pointer_cast<const void>(value),
			type_table);
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Record> value, const TypeTable& type_table) {
	return SetSymbol(identifier, type, static_pointer_cast<const void>(value),
			type_table);
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<Array> value, const TypeTable& type_table) {
	return SetSymbol(identifier, value->GetTypeSpecifier(),
			static_pointer_cast<const void>(value), type_table);
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<Function> value, const TypeTable& type_table) {
	return SetSymbol(identifier, value->GetTypeSpecifier(),
			static_pointer_cast<const void>(value), type_table);
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Sum> value, const TypeTable& type_table) {
	return SetSymbol(identifier, type, static_pointer_cast<const void>(value),
			type_table);
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<MaybeTypeSpecifier> type, const_shared_ptr<Sum> value,
		const TypeTable& type_table) {
	return SetSymbol(identifier, type, static_pointer_cast<const void>(value),
			type_table);
}

volatile_shared_ptr<SymbolContext> SymbolContext::GetDefault() {
	static volatile_shared_ptr<SymbolContext> instance = make_shared<
			SymbolContext>(Modifier::Type::NONE);
	return instance;
}

SetResult SymbolContext::SetSymbol(const string& identifier,
		const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value,
		const TypeTable& type_table) {
	auto result = m_table->find(identifier);

	if (result != m_table->end()) {
		auto existing_symbol = result->second;
		if (existing_symbol->GetTypeSpecifier()->AnalyzeAssignmentTo(type,
				type_table) == EQUIVALENT) {
			if (!(m_modifiers & Modifier::MUTABLE)) {
				return MUTATION_DISALLOWED;
			} else {
				auto new_symbol = existing_symbol->WithValue(type, value,
						type_table);

				//TODO: error checking
				m_table->erase(identifier);
				m_table->insert(
						std::pair<const string, const_shared_ptr<Symbol>>(
								identifier, new_symbol));

				return SET_SUCCESS;
			}
		} else {
			return INCOMPATIBLE_TYPE;
		}
	} else {
		return UNDEFINED_SYMBOL;
	}
}

volatile_shared_ptr<SymbolContext> SymbolContext::Clone() const {
	return volatile_shared_ptr<SymbolContext>(
			new SymbolContext(m_modifiers,
					make_shared<symbol_map>(m_table->begin(), m_table->end())));
}

SymbolContext::SymbolContext(const SymbolContext& other) :
		m_modifiers(other.m_modifiers), m_table(other.m_table) {
}

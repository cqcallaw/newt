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

#include "assert.h"
#include "indent.h"
#include "type.h"
#include "error.h"
#include "expression.h"
#include <execution_context.h>
#include <array_type_specifier.h>
#include <function_declaration.h>
#include <symbol.h>
#include <function.h>
#include <sum.h>
#include <primitive_type_specifier.h>
#include <maybe_type_specifier.h>
#include <unit.h>
#include <unit_type.h>
#include <record.h>
#include <memory>

Symbol::Symbol(const_shared_ptr<bool> value) :
		Symbol(PrimitiveTypeSpecifier::GetBoolean(),
				static_pointer_cast<const void>(value), false) {
}

Symbol::Symbol(const_shared_ptr<int> value) :
		Symbol(PrimitiveTypeSpecifier::GetInt(),
				static_pointer_cast<const void>(value), false) {
}

Symbol::Symbol(const_shared_ptr<const std::uint8_t> value) :
		Symbol(PrimitiveTypeSpecifier::GetByte(),
				static_pointer_cast<const void>(value), false) {
}

Symbol::Symbol(const_shared_ptr<double> value) :
		Symbol(PrimitiveTypeSpecifier::GetDouble(),
				static_pointer_cast<const void>(value), false) {
}

Symbol::Symbol(const_shared_ptr<string> value) :
		Symbol(PrimitiveTypeSpecifier::GetString(),
				static_pointer_cast<const void>(value), false) {
}

Symbol::Symbol(const_shared_ptr<Array> value) :
		Symbol(value->GetTypeSpecifier(),
				static_pointer_cast<const void>(value), true) {
}

Symbol::Symbol(const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Record> value) :
		Symbol(type, static_pointer_cast<const void>(value), true) {
}

Symbol::Symbol(const_shared_ptr<Function> value) :
		Symbol(value->GetTypeSpecifier(),
				static_pointer_cast<const void>(value), true) {
}

Symbol::Symbol(const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Sum> value) :
		Symbol(type, static_pointer_cast<const void>(value), true) {
}

Symbol::Symbol(const_shared_ptr<MaybeTypeSpecifier> type,
		const_shared_ptr<Sum> value) :
		Symbol(type, static_pointer_cast<const void>(value), true) {
}

Symbol::Symbol(const_shared_ptr<TypeSpecifier> type,
		const_shared_ptr<Unit> value) :
		Symbol(type, static_pointer_cast<const void>(value), false) {
}

Symbol::Symbol(const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value, const bool weakenable) :
		Symbol(type_specifier, value, weak_ptr<const void>(), weakenable) {
}

Symbol::Symbol(const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value, weak_ptr<const void> weak_value,
		const bool weakenable) :
		m_type_specifier(type_specifier), m_value(value), m_weak_ref(
				weak_value), m_weakenable(weakenable) {
	assert(type_specifier);
	assert(type_specifier != PrimitiveTypeSpecifier::GetNone());

	if (weakenable)
		assert(value);

	// value and weak value are mutually exclusive
	if (value) {
		assert(!weak_value.lock());
	}

	if (weak_value.lock()) {
		assert(!value);
	}
}

const_shared_ptr<Symbol> Symbol::WithValue(
		const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value, const TypeTable& type_table) const {
	if (type_specifier->AnalyzeAssignmentTo(this->m_type_specifier, type_table)
			!= EQUIVALENT) {
		return GetDefaultSymbol();
	}

	return const_shared_ptr<Symbol>(
			new Symbol(type_specifier, value, m_weakenable));
}

const string Symbol::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	return ToString(m_type_specifier, m_value, type_table, indent);
}

const_shared_ptr<Symbol> Symbol::GetDefaultSymbol() {
	const static const_shared_ptr<Symbol> default_symbol = nullptr;

	return default_symbol;
}

Symbol::~Symbol() {
}

const string Symbol::ToString(const_shared_ptr<TypeSpecifier> type_specifier,
		const_shared_ptr<void> value, const TypeTable& type_table,
		const Indent& indent) {
	ostringstream buffer;
	if (value) {
		if (type_specifier
				&& type_specifier != PrimitiveTypeSpecifier::GetNone()) {
			auto type_result = type_specifier->GetType(type_table, RESOLVE);
			if (ErrorList::IsTerminator(type_result->GetErrors())) {
				auto type = type_result->GetData<TypeDefinition>();
				auto type_map = ComplexType::DefaultTypeParameterMap;
				if (!TypeSpecifierList::IsTerminator(
						type_specifier->GetTypeArgumentList())) {
					auto type_specifier_mapping_result = ComplexType::GetTypeParameterMap(
							type->GetTypeParameterList(),
							type_specifier->GetTypeArgumentList(), type_table);
					auto errors = type_specifier_mapping_result.GetErrors();
					if (ErrorList::IsTerminator(errors)) {
						type_map = type_specifier_mapping_result.GetData();
					} else {
						buffer << "<error resolving type parameters>";
					}
				}
				buffer
						<< type->GetValueSeparator(indent, value.get(),
								type_map);
				buffer
						<< type->ValueToString(type_table, indent, value,
								type_map);
			} else {
				buffer << "UNDEFINED TYPE";
			}
		} else {
			buffer << "<invalid type specifier>";
		}
	} else {
		buffer << "<no value>";
	}
	return buffer.str();
}

const_shared_ptr<Symbol> Symbol::GetNilSymbol() {
	const static const_shared_ptr<Symbol> value = make_shared<Symbol>(
			Symbol(TypeTable::GetNilTypeSpecifier(),
					TypeTable::GetNilType()->GetValue()));
	return value;
}

const_shared_ptr<Symbol> Symbol::WeakenReference(
		const_shared_ptr<Symbol> original) {
	if (original->m_weakenable) {
		auto instance = new Symbol(original->m_type_specifier, nullptr,
				weak_ptr<const void>(original->m_value), false);
		return const_shared_ptr<Symbol>(instance);
	} else {
		return original;
	}
}

const_shared_ptr<void> Symbol::GetValue() const {
	if (m_value) {
		return m_value;
	} else {
		auto lock = m_weak_ref.lock();
		assert(lock); // fail loudly if we make a reference to a disposed value
		return lock;
	}
}

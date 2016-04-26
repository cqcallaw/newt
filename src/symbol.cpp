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

#include <record.h>
#include <memory>

Symbol::Symbol(const_shared_ptr<bool> value) :
		Symbol(PrimitiveTypeSpecifier::GetBoolean(),
				static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<int> value) :
		Symbol(PrimitiveTypeSpecifier::GetInt(),
				static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<double> value) :
		Symbol(PrimitiveTypeSpecifier::GetDouble(),
				static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<string> value) :
		Symbol(PrimitiveTypeSpecifier::GetString(),
				static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<Array> value) :
		Symbol(value->GetTypeSpecifier(),
				static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Record> value) :
		Symbol(type, static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<Function> value) :
		Symbol(value->GetType(), static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<Sum> value) :
		Symbol(type, static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<TypeSpecifier> type,
		const_shared_ptr<void> value) :
		m_type_specifier(type), m_value(value) {
	assert(type);
	assert(type != PrimitiveTypeSpecifier::GetNone());
}

const_shared_ptr<Symbol> Symbol::WithValue(const_shared_ptr<TypeSpecifier> type,
		const_shared_ptr<void> value, const TypeTable& type_table) const {
	if (!type->IsAssignableTo(this->m_type_specifier, type_table)) {
		return GetDefaultSymbol();
	}

	return const_shared_ptr<Symbol>(new Symbol(type, value));
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
			auto type = type_specifier->GetType(type_table, RESOLVE);

			if (type) {
				buffer << type->GetValueSeparator(indent, value.get());
				buffer << type->ValueToString(type_table, indent, value);
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

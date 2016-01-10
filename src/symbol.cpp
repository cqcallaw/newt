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
#include <primitive_type_specifier.h>
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

Symbol::Symbol(const_shared_ptr<CompoundTypeInstance> value) :
		Symbol(value->GetTypeSpecifier(),
				static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<Function> value) :
		Symbol(value->GetType(), static_pointer_cast<const void>(value)) {
}

Symbol::Symbol(const_shared_ptr<TypeSpecifier> type,
		const_shared_ptr<void> value) :
		m_type(type), m_value(value) {
	assert(type);
}

const_shared_ptr<Symbol> Symbol::WithValue(
		const_shared_ptr<TypeSpecifier> type,
		const_shared_ptr<void> value) const {
	if (!type->IsAssignableTo(this->m_type)) {
		return GetDefaultSymbol();
	}

	return const_shared_ptr<Symbol>(new Symbol(type, value));
}

const string Symbol::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(m_type);
	if (as_primitive) {
		buffer << " ";
		buffer << as_primitive->ToString(m_value);
	}

	const_shared_ptr<ArrayTypeSpecifier> as_array = std::dynamic_pointer_cast<
			const ArrayTypeSpecifier>(m_type);
	if (as_array) {
		buffer << endl;
		auto array = static_pointer_cast<const Array>(m_value);
		buffer << array->ToString(type_table, indent);
	}

	const_shared_ptr<CompoundTypeSpecifier> as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(m_type);
	if (as_compound) {
		buffer << endl;
		auto compound_type_instance = static_pointer_cast<
				const CompoundTypeInstance>(m_value);
		buffer << compound_type_instance->ToString(type_table, indent + 1);
	}

	const_shared_ptr<FunctionTypeSpecifier> as_function =
			std::dynamic_pointer_cast<const FunctionTypeSpecifier>(m_type);
	if (as_function) {
		buffer << endl;
		auto function = static_pointer_cast<const Function>(m_value);
		buffer << function->ToString(type_table, indent + 1);
	}

	return buffer.str();

}

const_shared_ptr<Symbol> Symbol::GetDefaultSymbol() {
	const static const_shared_ptr<Symbol> DefaultSymbol = const_shared_ptr<
			Symbol>(new Symbol(PrimitiveTypeSpecifier::GetNone(), nullptr));

	return DefaultSymbol;
}

Symbol::~Symbol() {
}

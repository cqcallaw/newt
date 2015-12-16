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

Symbol::Symbol(const string name, const bool *value) :
		Symbol(PrimitiveTypeSpecifier::GetBoolean(), name, (void *) value) {
}

Symbol::Symbol(const string name, const int *value) :
		Symbol(PrimitiveTypeSpecifier::GetInt(), name, (void *) value) {
}

Symbol::Symbol(const string name, const double *value) :
		Symbol(PrimitiveTypeSpecifier::GetDouble(), name, (void *) value) {
}

Symbol::Symbol(const string name, const string *value) :
		Symbol(PrimitiveTypeSpecifier::GetString(), name, (void *) value) {
}

Symbol::Symbol(const string name, const Array* value) :
		Symbol(value->GetTypeSpecifier(), name, (void *) value) {
}

Symbol::Symbol(const string name, const CompoundTypeInstance* value) :
		Symbol(value->GetTypeSpecifier(), name, (void *) value) {
}

Symbol::Symbol(const string name, const Function* value) :
		Symbol(value->GetType(), name, (void *) value) {
}

Symbol::Symbol(const TypeSpecifier* type, const string name, const void* value) :
		m_type(type), m_name(name), m_value(value) {
	assert(type != nullptr);
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

Symbol::Symbol(const string* name, const Array* value) :
		Symbol(value->GetTypeSpecifier(), *name, (void *) value) {
}

Symbol::Symbol(const string* name, const CompoundTypeInstance* value) :
		Symbol(*name, value) {
}

Symbol::Symbol(const string* name, const Function* value) :
		Symbol(*name, value) {
}

const TypeSpecifier* Symbol::GetType() const {
	return m_type;
}

const std::string Symbol::GetName() const {
	return m_name;
}

const void* Symbol::GetValue() const {
	return m_value;
}

const Symbol* Symbol::WithValue(const bool* value) const {
	return WithValue(PrimitiveTypeSpecifier::GetBoolean(), (void*) value);
}
const Symbol* Symbol::WithValue(const int* value) const {
	return WithValue(PrimitiveTypeSpecifier::GetInt(), (void*) value);
}
const Symbol* Symbol::WithValue(const double* value) const {
	return WithValue(PrimitiveTypeSpecifier::GetDouble(), (void*) value);
}
const Symbol* Symbol::WithValue(const string* value) const {
	return WithValue(PrimitiveTypeSpecifier::GetString(), (void*) value);
}

const Symbol* Symbol::WithValue(const Array* value) const {
	return WithValue(value->GetTypeSpecifier(), (void*) value);
}

const Symbol* Symbol::WithValue(const CompoundTypeInstance* value) const {
	return WithValue(value->GetTypeSpecifier(), (void*) value);
}

const Symbol* Symbol::WithValue(const TypeSpecifier* type,
		const void* value) const {
	if (!type->IsAssignableTo(this->m_type)) {
		return GetDefaultSymbol();
	}

	return new Symbol(type, m_name, value);
}

const string Symbol::ToString(const TypeTable* type_table,
		const Indent indent) const {
	ostringstream buffer;
	buffer << indent << m_type->ToString() << " " << m_name << ":";
	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(m_type);
	if (as_primitive != nullptr) {
		buffer << " ";
		buffer << as_primitive->ToString(m_value);
	}

	const ArrayTypeSpecifier* as_array =
			dynamic_cast<const ArrayTypeSpecifier*>(m_type);
	if (as_array != nullptr) {
		buffer << endl;
		const Array* array = static_cast<const Array*>(m_value);
		buffer << array->ToString(type_table, indent);
	}

	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(m_type);
	if (as_compound != nullptr) {
		buffer << endl;
		const CompoundTypeInstance* compound_type_instance =
				(const CompoundTypeInstance*) m_value;
		buffer << compound_type_instance->ToString(type_table, indent + 1);
	}

	return buffer.str();

}

const string Symbol::GetDefaultSymbolName() {
	const static std::string DefaultSymbolName = std::string("[!!_DEFAULT_!!]");
	return DefaultSymbolName;
}

const Symbol* Symbol::GetDefaultSymbol() {
	const static std::unique_ptr<Symbol> DefaultSymbol = std::unique_ptr
			< Symbol
			> (new Symbol(PrimitiveTypeSpecifier::GetNone(),
					GetDefaultSymbolName(), NULL));

	return DefaultSymbol.get();
}


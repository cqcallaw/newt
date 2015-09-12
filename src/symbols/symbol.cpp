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

#include "symbol.h"
#include "assert.h"
#include "indent.h"
#include "type.h"
#include "error.h"
#include "expression.h"
#include <execution_context.h>
#include <array_type_specifier.h>

const std::string Symbol::DefaultSymbolName = std::string("[!!_DEFAULT_!!]");
const Symbol* Symbol::DefaultSymbol = new Symbol(PrimitiveTypeSpecifier::NONE,
		DefaultSymbolName, NULL);

Symbol::Symbol(const string name, const bool *value) :
		Symbol(PrimitiveTypeSpecifier::BOOLEAN, name, (void *) value) {
}

Symbol::Symbol(const string name, const int *value) :
		Symbol(PrimitiveTypeSpecifier::INT, name, (void *) value) {
}

Symbol::Symbol(const string name, const double *value) :
		Symbol(PrimitiveTypeSpecifier::DOUBLE, name, (void *) value) {
}

Symbol::Symbol(const string name, const string *value) :
		Symbol(PrimitiveTypeSpecifier::STRING, name, (void *) value) {
}

Symbol::Symbol(const TypeSpecifier* type, const string name, const void* value) :
		m_type(type), m_name(name), m_value(value) {
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
	return WithValue(PrimitiveTypeSpecifier::BOOLEAN, (void*) value);
}
const Symbol* Symbol::WithValue(const int* value) const {
	return WithValue(PrimitiveTypeSpecifier::INT, (void*) value);
}
const Symbol* Symbol::WithValue(const double* value) const {
	return WithValue(PrimitiveTypeSpecifier::DOUBLE, (void*) value);
}
const Symbol* Symbol::WithValue(const string* value) const {
	return WithValue(PrimitiveTypeSpecifier::STRING, (void*) value);
}

const Symbol* Symbol::WithValue(const TypeSpecifier* type,
		const void* value) const {
	if (!type->IsAssignableTo(this->m_type)) {
		return DefaultSymbol;
	}

	return new Symbol(type, m_name, value);
}

const string Symbol::ToString(const TypeTable* type_table,
		const Indent indent) const {
	/*ostringstream os;

	 os << m_type << " " << m_name << ": ";
	 switch (m_type) {
	 case BOOLEAN:
	 os << *((bool*) m_value);
	 break;
	 case INT:
	 os << *((int*) m_value);
	 break;
	 case DOUBLE:
	 os << *((double*) m_value);
	 break;
	 case STRING:
	 os << "\"" << *((string*) m_value) << "\"";
	 break;
	 case COMPOUND: {
	 os << endl;
	 const CompoundTypeInstance* as_struct =
	 (const CompoundTypeInstance*) m_value;
	 const SymbolContext* context = as_struct->GetDefinition();
	 string indent = "\t";
	 context->print(os, &indent);
	 break;
	 }
	 default:
	 assert(false);
	 }

	 return os.str();
	 */

	ostringstream buffer;
	buffer << indent << m_type << m_name << ": ";
	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(m_type);
	if (as_primitive != nullptr) {
		buffer << as_primitive->ToString(m_value);
	}

	const ArrayTypeSpecifier* as_array =
			dynamic_cast<const ArrayTypeSpecifier*>(m_type);
	if (as_array != nullptr) {
		buffer << as_array->ToString();
	}

	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(m_type);
	if (as_compound != nullptr) {
		const string type_name = as_compound->GetTypeName();
		buffer << type_name << " " << m_name << ": ";
		const CompoundTypeInstance* compound_type_instance =
				(const CompoundTypeInstance*) m_value;
		buffer << compound_type_instance->ToString(type_table, indent + 1);
	}

	return buffer.str();

}

Symbol::Symbol(const string name, const CompoundTypeInstance* value) :
		Symbol(PrimitiveTypeSpecifier::COMPOUND, name, (void *) value) {
}

Symbol::Symbol(const string* name, const CompoundTypeInstance* value) :
		Symbol(*name, value) {
}

const Symbol* Symbol::WithValue(const CompoundTypeInstance* value) const {
	return WithValue(PrimitiveTypeSpecifier::COMPOUND, (void*) value);
}

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
#include <member_definition.h>

const CompoundTypeInstance* CompoundTypeInstance::GetDefaultInstance(
		const string type_name,
		const LinkedList<const SymbolContext*>* parent_context,
		const CompoundType* type) {
	map<const string, const Symbol*> symbol_mapping;

	const map<const string, const MemberDefinition*>* type_definition =
			type->GetDefinition();
	map<const string, const MemberDefinition*>::const_iterator iter;

	for (iter = type_definition->begin(); iter != type_definition->end();
			++iter) {
		const string member_name = iter->first;
		const MemberDefinition* member_type_information = iter->second;

		const Symbol* symbol = GetSymbol(member_type_information->GetType(),
				member_name, member_type_information->GetDefaultValue());
		symbol_mapping.insert(
				std::pair<const string, const Symbol*>(member_name, symbol));
	}

	const CompoundTypeInstance* instance = new CompoundTypeInstance(type_name,
			new SymbolContext(type->GetModifiers(), parent_context,
					&symbol_mapping));

	return instance;
}

const Symbol* CompoundTypeInstance::GetSymbol(const TypeSpecifier* member_type,
		const string member_name, const void* void_value) {
	if (member_type->IsAssignableTo(PrimitiveTypeSpecifier::BOOLEAN)) {
		return new Symbol(member_name, (bool*) void_value);
	} else if (member_type->IsAssignableTo(PrimitiveTypeSpecifier::INT)) {
		return new Symbol(member_name, (int*) void_value);
	} else if (member_type->IsAssignableTo(PrimitiveTypeSpecifier::DOUBLE)) {
		return new Symbol(member_name, (double*) void_value);
	} else if (member_type->IsAssignableTo(PrimitiveTypeSpecifier::STRING)) {
		return new Symbol(member_name, (string*) void_value);
	} else if (member_type->IsAssignableTo(PrimitiveTypeSpecifier::COMPOUND)) {
		return new Symbol(member_name, (CompoundTypeInstance*) void_value);
	} else {
		assert(false);
		return nullptr;
	}
}

const string CompoundTypeInstance::ToString(const Indent indent) const {
	ostringstream buffer;
	m_definition->print(buffer, indent);
	return buffer.str();
}

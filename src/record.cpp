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

#include <member_definition.h>
#include <function_type_specifier.h>
#include <record.h>
#include <sum.h>
#include <symbol_table.h>

const_shared_ptr<Record> Record::GetDefaultInstance(
		const_shared_ptr<std::string> type_name, const RecordType& type) {
	auto symbol_mapping = make_shared<symbol_map>();

	plain_shared_ptr<definition_map> type_definition = type.GetDefinition();
	definition_map::const_iterator iter;

	auto symbol_table = make_shared<SymbolTable>(type.GetModifiers());

	for (iter = type_definition->begin(); iter != type_definition->end();
			++iter) {
		const string member_name = iter->first;
		const_shared_ptr<MemberDefinition> member_type_information =
				iter->second;

		auto symbol = member_type_information->GetType()->GetSymbol(
				member_type_information->GetDefaultValue(), TypeTable());
		symbol_table->InsertSymbol(member_name, symbol);
	}

	const_shared_ptr<RecordTypeSpecifier> type_specifier = make_shared<
			RecordTypeSpecifier>(type_name);

	return make_shared<Record>(Record(type_specifier, symbol_table));
}

const string Record::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	m_definition->print(buffer, type_table, indent);
	string result = buffer.str();
	return result;
}

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

#include <function_type_specifier.h>
#include <record.h>
#include <sum.h>
#include <symbol_table.h>
#include <record_type.h>

const_shared_ptr<Record> Record::GetDefaultInstance(
		const_shared_ptr<std::string> type_name, const RecordType& type) {
	auto symbol_mapping = make_shared<symbol_map>();

	auto type_definition = type.GetDefinition();
	auto default_symbols = type_definition->GetDefaultSymbolContext();

	const_shared_ptr<RecordTypeSpecifier> type_specifier = make_shared<
			RecordTypeSpecifier>(type_name);

	return make_shared<Record>(type_specifier, default_symbols);
}

const string Record::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	m_definition->print(buffer, type_table, indent);
	string result = buffer.str();
	return result;
}

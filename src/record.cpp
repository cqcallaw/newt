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

const_shared_ptr<Record> Record::GetDefaultInstance(const RecordType& type,
		const_shared_ptr<type_parameter_map> type_mapping) {
	auto symbol_mapping = make_shared<symbol_map>();

	auto type_definition = type.GetDefinition();
	auto default_symbols = type_definition->GetDefaultSymbolContext(
			type.GetModifiers(), nullptr, type_mapping);

	return make_shared<Record>(default_symbols);
}

const string Record::ToString(const TypeTable& type_table,
		const_shared_ptr<type_parameter_map> type_mapping,
		const Indent& indent) const {
	ostringstream buffer;
	m_definition->print(buffer, type_table, type_mapping, indent);
	string result = buffer.str();
	return result;
}

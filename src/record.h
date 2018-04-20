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

#ifndef RECORD_H_
#define RECORD_H_

#include <defaults.h>
#include <string>
#include <linked_list.h>
#include <type.h>
#include <type_specifier.h>
#include <complex_type.h>

class RecordType;
class RecordTypeSpecifier;
class SymbolContext;
class Symbol;
class Indent;

using namespace std;

class Record {
public:
	Record(volatile_shared_ptr<SymbolContext> definition) :
			m_definition(definition) {
	}

	const_shared_ptr<Record> WithDefinition(
			volatile_shared_ptr<SymbolContext> new_definition) const {
		return make_shared<Record>(new_definition);
	}

	volatile_shared_ptr<SymbolContext> GetDefinition() const {
		return m_definition;
	}

	static const_shared_ptr<Record> GetDefaultInstance(const RecordType& type,
			const_shared_ptr<type_parameter_map> type_mapping);

	const string ToString(const TypeTable& type_table,
			const_shared_ptr<type_parameter_map> type_mapping,
			const Indent& indent) const;

private:
	volatile_shared_ptr<SymbolContext> m_definition;
};

#endif /* RECORD_H_ */

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

class RecordType;
class RecordTypeSpecifier;
class SymbolContext;
class Symbol;
class Indent;

using namespace std;

class Record {
public:
	Record(const_shared_ptr<RecordTypeSpecifier> type,
			volatile_shared_ptr<SymbolContext> definition) :
			m_type(type), m_definition(definition) {
	}

	const_shared_ptr<Record> WithDefinition(
			volatile_shared_ptr<SymbolContext> new_definition) const {
		return make_shared<Record>(m_type, new_definition);
	}

	const_shared_ptr<RecordTypeSpecifier> GetTypeSpecifier() const {
		return m_type;
	}

	volatile_shared_ptr<SymbolContext> GetDefinition() const {
		return m_definition;
	}

	static const_shared_ptr<Record> GetDefaultInstance(
			const_shared_ptr<std::string> type_name, const RecordType& type);

	const string ToString(const TypeTable& type_table,
			const Indent& indent) const;

private:
	const_shared_ptr<RecordTypeSpecifier> m_type;
	volatile_shared_ptr<SymbolContext> m_definition;
};

#endif /* RECORD_H_ */

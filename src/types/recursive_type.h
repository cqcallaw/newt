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

#ifndef TYPES_RECURSIVE_TYPE_H_
#define TYPES_RECURSIVE_TYPE_H_

#include <type_definition.h>
#include <assert.h>
#include <result.h>
#include <sstream>
#include <record.h>
#include <record_type_specifier.h>
#include <symbol_context.h>

class RecursiveType: public ComplexType {
public:
	RecursiveType(const_shared_ptr<string> type_name) :
			m_type_name(type_name) {
		m_default_value = make_shared<Record>(
				make_shared<RecordTypeSpecifier>(m_type_name),
				make_shared<SymbolContext>(Modifier::Type::READONLY));
		m_default_symbol = make_shared<Symbol>(m_default_value);
	}

	virtual ~RecursiveType() {
	}

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const {
		return name == *m_type_name;
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		ostringstream os;
		os << indent + 1;
		os << "<placeholder for " << *m_type_name << ">" << endl;
		return os.str();
	}

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const {
		ostringstream os;
		os << indent + 1;
		os << "<placeholder value for " << *m_type_name << ">" << endl;
		return os.str();
	}

	virtual const_shared_ptr<void> GetDefaultValue(
			const_shared_ptr<std::string> type_name) const {
		return m_default_value;
	}

	virtual const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<void> value,
			const_shared_ptr<ComplexTypeSpecifier> container = nullptr) const {
		return m_default_symbol;
	}

	const_shared_ptr<string> GetTypeName() const {
		return m_type_name;
	}

protected:
	virtual const_shared_ptr<Result> PreprocessSymbolCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const {
		return make_shared<Result>(m_default_symbol, ErrorList::GetTerminator());
	}

	virtual const ErrorListRef InstantiateCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<std::string> name,
			const_shared_ptr<Expression> initializer) const {
		return ErrorList::GetTerminator();
	}

private:
	const_shared_ptr<string> m_type_name;
	plain_shared_ptr<Record> m_default_value;
	plain_shared_ptr<Symbol> m_default_symbol;
};

#endif /* TYPES_RECURSIVE_TYPE_H_ */

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

#ifndef SUM_TYPE_H_
#define SUM_TYPE_H_

#include <declaration_statement.h>
#include <defaults.h>
#include <function.h>
#include <indent.h>
#include <result.h>
#include <type_definition.h>
#include <type_table.h>
#include <map>
#include <string>

enum WideningResult {
	INCOMPATIBLE = 0, AMBIGUOUS = 1, UNAMBIGUOUS = 2
};

class SumType: public TypeDefinition {
public:
	virtual ~SumType() {
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const;

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const;

	const_shared_ptr<TypeTable> GetTypeTable() const {
		return m_type_table;
	}

	const const_shared_ptr<DeclarationStatement> GetFirstDeclaration() const {
		return m_first_declaration;
	}

	static const_shared_ptr<Result> Build(
			const shared_ptr<ExecutionContext> context,
			const DeclarationListRef member_declarations);

	virtual bool IsSpecifiedBy(const std::string name,
			const TypeSpecifier& type_specifier) const;

	const WideningResult AnalyzeWidening(const TypeSpecifier& other) const;

	const_shared_ptr<std::string> MapSpecifierToVariant(
			const TypeSpecifier& type_specifier) const;

private:
	SumType(const_shared_ptr<TypeTable> type_table,
			const_shared_ptr<DeclarationStatement> first_declaration) :
			m_type_table(type_table), m_first_declaration(first_declaration) {
	}
	const_shared_ptr<TypeTable> m_type_table;
	const_shared_ptr<DeclarationStatement> m_first_declaration;
};

#endif /* SUM_TYPE_H_ */

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

#ifndef TYPES_PLACEHOLDER_TYPE_H_
#define TYPES_PLACEHOLDER_TYPE_H_

#include <type_definition.h>
#include <assert.h>
#include <result.h>
#include <sstream>
#include <complex_type.h>
#include <symbol_context.h>
#include <complex_type_specifier.h>

class PlaceholderType: public ComplexType {
public:
	PlaceholderType(const_shared_ptr<string> type_name,
			const_shared_ptr<Symbol> symbol) :
			m_type_name(type_name), m_symbol(symbol) {
	}

	virtual ~PlaceholderType() {
	}

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container) const {
		return make_shared<ComplexTypeSpecifier>(name, container,
				NamespaceQualifierList::GetTerminator());
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		ostringstream os;
		os << indent + 1;
		os << "<placeholder for " << *m_type_name << ">" << endl;
		return os.str();
	}

	virtual const std::string GetValueSeparator(const Indent& indent,
			const void* value) const {
		return "";
	}

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const {
		ostringstream os;
		os << indent + 1;
		os << "<placeholder value for " << *m_type_name << ">" << endl;
		return os.str();
	}

	virtual const_shared_ptr<void> GetDefaultValue(
			const TypeTable& type_table) const {
		return nullptr;
	}

	virtual const_shared_ptr<Symbol> GetSymbol(const TypeTable& type_table,
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void> value) const {
		return m_symbol;
	}

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const {
		assert(false);
		return nullptr;
	}

	virtual const WideningResult AnalyzeConversion(
			const ComplexTypeSpecifier& current,
			const TypeSpecifier& unaliased_other) const {
		return INCOMPATIBLE;
	}

	virtual const_shared_ptr<TypeSpecifier> GetMemberTypeSpecifier(
			const_shared_ptr<std::string> member_name) const {
		assert(false);
		return nullptr;
	}

	virtual const_shared_ptr<void> GetMemberDefaultValue(
			const_shared_ptr<std::string> member_name) const {
		assert(false);
		return nullptr;
	}

	virtual const_shared_ptr<TypeTable> GetDefinition() const {
		assert(false);
		return nullptr;
	}

	const_shared_ptr<string> GetTypeName() const {
		return m_type_name;
	}

protected:
	virtual const_shared_ptr<Result> PreprocessSymbolCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<Expression> initializer) const {
		return make_shared<Result>(Symbol::GetDefaultSymbol(),
				ErrorList::GetTerminator());
	}

	virtual const SetResult InstantiateCore(
			const std::shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<ComplexTypeSpecifier> type_specifier,
			const_shared_ptr<TypeSpecifier> value_type_specifier,
			const std::string& instance_name,
			const_shared_ptr<void> data) const {
		return SetResult::NO_SET_RESULT;
	}

private:
	const_shared_ptr<string> m_type_name;
	const_shared_ptr<Symbol> m_symbol;
};

#endif /* TYPES_PLACEHOLDER_TYPE_H_ */

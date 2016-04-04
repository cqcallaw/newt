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
#include <function_type.h>
#include <function.h>
#include <assert.h>

const_shared_ptr<void> FunctionType::GetDefaultValue(
		const_shared_ptr<std::string> type_name,
		const TypeTable& type_table) const {
	return m_type_specifier->DefaultValue(type_table);
}

const std::string FunctionType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	return m_type_specifier->ToString();
}

const std::string FunctionType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	auto as_function = static_pointer_cast<const Function>(value);
	return as_function->ToString(type_table, Indent(0));
}

const_shared_ptr<TypeSpecifier> FunctionType::GetTypeSpecifier(
		const_shared_ptr<std::string> name) const {
	return m_type_specifier;
}

bool FunctionType::IsSpecifiedBy(const std::string& name,
		const TypeSpecifier& type_specifier) const {
	return *m_type_specifier == type_specifier;
}

const std::string FunctionType::GetValueSeperator(const Indent& indent,
		const_shared_ptr<void> value) const {
	return "\n" + (indent + 1).ToString();
}

const_shared_ptr<Symbol> FunctionType::GetSymbol(const_shared_ptr<void> value,
		const_shared_ptr<ComplexTypeSpecifier> container) const {
	return make_shared<Symbol>(static_pointer_cast<const Function>(value));
}

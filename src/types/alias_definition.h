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

#ifndef TYPES_ALIAS_DEFINITION_H_
#define TYPES_ALIAS_DEFINITION_H_

#include <concrete_type.h>
#include <primitive_type_specifier.h>
#include <indent.h>

class AliasDefinition: public ConcreteType {
public:
	AliasDefinition(const_shared_ptr<TypeTable> origin_table,
			const_shared_ptr<TypeSpecifier> original,
			const_shared_ptr<void> default_value = nullptr) :
			m_origin_table(origin_table), m_original(original), m_default_value(
					default_value) {
	}

	virtual ~AliasDefinition() {
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		ostringstream os;
		Indent child_indent = indent + 1;
		os << child_indent;
		os << m_original->ToString();

		if (m_default_value) {
			string default_value = ValueToString(type_table, child_indent,
					m_default_value);
			os << " (";

			auto as_complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
					m_original);
			if (as_complex) {
				os << endl;
			}

			os << default_value;

			if (as_complex) {
				os << child_indent;
			}
			os << ")";
		}

		return os.str();
	}

	virtual const std::string ValueToString(const TypeTable& type_table,
			const Indent& indent, const_shared_ptr<void> value) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->ValueToString(type_table, indent, value);
		} else {
			return "<No origin found for alias '" + m_original->ToString() + "'";
		}
	}

	virtual const std::string GetValueSeperator(const Indent& indent,
			const_shared_ptr<void> value) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->GetValueSeperator(indent, value);
		} else {
			return "<No origin found for alias '" + m_original->ToString() + "'";
		}
	}

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const_shared_ptr<std::string> name,
			const_shared_ptr<ComplexTypeSpecifier> container) const {
		return m_original;
	}

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->IsSpecifiedBy(name, type_specifier);
		} else {
			return false;
		}
	}

	virtual const_shared_ptr<void> GetDefaultValue(
			const TypeTable& type_table) const;

	virtual const_shared_ptr<Symbol> GetSymbol(
			const_shared_ptr<TypeSpecifier> type_specifier,
			const_shared_ptr<void>) const;

	virtual const_shared_ptr<DeclarationStatement> GetDeclarationStatement(
			const yy::location position, const_shared_ptr<TypeSpecifier> type,
			const yy::location type_position,
			const_shared_ptr<std::string> name,
			const yy::location name_position,
			const_shared_ptr<Expression> initializer_expression) const {
		auto origin = GetOrigin();
		if (origin) {
			return origin->GetDeclarationStatement(position, type,
					type_position, name, name_position, initializer_expression);
		} else {
			assert(false);
			return nullptr;
		}
	}

	const_shared_ptr<TypeTable> getOriginTable() const {
		return m_origin_table;
	}

	const_shared_ptr<TypeSpecifier> GetOriginal() const {
		return m_original;
	}

	const const_shared_ptr<void> GetDefaultValue() const {
		return m_default_value;
	}

	const_shared_ptr<TypeDefinition> GetOrigin() const;

private:
	const_shared_ptr<TypeTable> m_origin_table;
	const_shared_ptr<TypeSpecifier> m_original;
	const_shared_ptr<void> m_default_value;
};

#endif /* TYPES_ALIAS_DEFINITION_H_ */

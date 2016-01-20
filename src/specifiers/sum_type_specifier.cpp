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

#include <sum_type_specifier.h>
#include <sstream>
#include <sum_declaration_statement.h>

SumTypeSpecifier::SumTypeSpecifier(const TypeSpecifierListRef types) :
		m_types(types) {
	//the specifier list should have at least two elements
	assert(!TypeSpecifierList::IsTerminator(types->GetNext()));
}

SumTypeSpecifier::~SumTypeSpecifier() {
}

const string SumTypeSpecifier::ToString() const {
	ostringstream buffer;
	buffer << "(";
	TypeSpecifierListRef subject = m_types;
	while (!TypeSpecifierList::IsTerminator(subject)) {
		buffer << subject->GetData()->ToString();
		subject = subject->GetNext();
		if (!TypeSpecifierList::IsTerminator(subject)) {
			//add separator
			buffer << "|";
		}
	}
	buffer << ")";

	return buffer.str();
}

const bool SumTypeSpecifier::IsAssignableTo(
		const_shared_ptr<TypeSpecifier> other) const {
	const_shared_ptr<SumTypeSpecifier> as_sum = std::dynamic_pointer_cast<
			const SumTypeSpecifier>(other);

	if (as_sum) {
		TypeSpecifierListRef subject = m_types;
		while (!TypeSpecifierList::IsTerminator(subject)) {
			if (as_sum->ContainsType(*subject->GetData(), ALLOW_WIDENING)) {
				subject = subject->GetNext();
			} else {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

const_shared_ptr<void> SumTypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	return GetDefaultMember()->DefaultValue(type_table);
}

const_shared_ptr<DeclarationStatement> SumTypeSpecifier::GetDeclarationStatement(
		const yy::location position, const_shared_ptr<TypeSpecifier> type,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) const {
	return make_shared<SumDeclarationStatement>(position,
			static_pointer_cast<const SumTypeSpecifier>(type), type_position,
			name, name_position, initializer_expression);
}

bool SumTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const SumTypeSpecifier& as_sum =
				dynamic_cast<const SumTypeSpecifier&>(other);

		//verify member types of this type specifier exist in the other type specifier
		TypeSpecifierListRef subject = m_types;
		while (!TypeSpecifierList::IsTerminator(subject)) {
			if (as_sum.ContainsType(*subject->GetData(), STRICT)) {
				subject = subject->GetNext();
			} else {
				return false;
			}
		}

		//verify member types of the other type specifier exist in this type specifier
		subject = as_sum.m_types;
		while (!TypeSpecifierList::IsTerminator(subject)) {
			if (ContainsType(*subject->GetData(), STRICT)) {
				subject = subject->GetNext();
			} else {
				return false;
			}
		}

		return true;
	} catch (std::bad_cast& e) {
		return false;
	}
}

const bool SumTypeSpecifier::ContainsType(const TypeSpecifier& other,
		ComparisonMode mode) const {
	TypeSpecifierListRef subject = m_types;
	while (!TypeSpecifierList::IsTerminator(subject)) {
		if (mode == ALLOW_WIDENING
				&& other.IsAssignableTo(subject->GetData())) {
			return true;
		} else if (mode == STRICT && other == *subject->GetData()) {
			return true;
		} else {
			subject = subject->GetNext();
		}
	}
	return false;
}

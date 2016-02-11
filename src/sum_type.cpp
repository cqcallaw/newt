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

#include <error.h>
#include <expression.h>
#include <linked_list.h>
#include <location.hh>
#include <position.hh>
#include <primitive_declaration_statement.h>
#include <primitive_type.h>
#include <primitive_type_specifier.h>
#include <record_declaration_statement.h>
#include <sum.h>
#include <sum_type.h>
#include <sum_type_specifier.h>
#include <symbol.h>
#include <memory>
#include <sstream>
#include <typeinfo>

const std::string SumType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	m_type_table->print(os, child_indent);
	return os.str();
}

const std::string SumType::ValueToString(const TypeTable& type_table,
		const Indent& indent, const_shared_ptr<void> value) const {
	ostringstream buffer;
	auto as_sum = static_pointer_cast<const Sum>(value);
	buffer << Symbol::ToString(as_sum->GetType(), value, type_table, indent);
	return buffer.str();
}

const_shared_ptr<Result> SumType::Build(const TypeTable& type_table,
		const DeclarationListRef member_declarations) {
	ErrorListRef errors = ErrorList::GetTerminator();

	DeclarationListRef subject = member_declarations;
	while (!DeclarationList::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> declaration = subject->GetData();

		const_shared_ptr<Expression> initializer_expression =
				declaration->GetInitializerExpression();
		if (initializer_expression && !initializer_expression->IsConstant()) {
			//if we have a non-constant initializer expression, generate an error
			yy::location position = initializer_expression->GetPosition();
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.begin.line, position.begin.column),
					errors);
		}

		subject = subject->GetNext();
	}

	const shared_ptr<TypeTable> types = make_shared<TypeTable>();
	if (ErrorList::IsTerminator(errors)) {
		DeclarationListRef subject = member_declarations;
		while (!DeclarationList::IsTerminator(subject)) {
			auto declaration = subject->GetData();
			auto variant_name = declaration->GetName();

			auto as_primitive = dynamic_pointer_cast<
					const PrimitiveDeclarationStatement>(declaration);
			if (as_primitive) {
				auto basic_type =
						static_pointer_cast<const PrimitiveTypeSpecifier>(
								as_primitive->GetType())->GetBasicType();
				types->AddType(*as_primitive->GetName(),
						make_shared<PrimitiveType>(basic_type));
			}

			auto as_record = dynamic_pointer_cast<
					const RecordDeclarationStatement>(declaration);
			if (as_record) {
				auto record_type_name = as_record->GetName();
				types->AddType(*record_type_name,
						type_table.GetType<TypeDefinition>(*record_type_name));
			}

			subject = subject->GetNext();
		}
	}

	auto type = const_shared_ptr<SumType>(
			new SumType(types, member_declarations->GetData()));

	return make_shared<Result>(type, errors);
}

bool SumType::IsSpecifiedBy(const std::string name,
		const TypeSpecifier& type_specifier) const {
	try {
		const SumTypeSpecifier& as_sum =
				dynamic_cast<const SumTypeSpecifier&>(type_specifier);
		return name == as_sum.GetTypeName();
	} catch (std::bad_cast& e) {
		return false;
	}
}

const WideningResult SumType::AnalyzeWidening(
		const TypeSpecifier& other) const {
	auto count = m_type_table->CountEntriesOfType(other);

	if (count > 1) {
		return AMBIGUOUS;
	} else if (count == 1) {
		return UNAMBIGUOUS;
	} else {
		return INCOMPATIBLE;
	}
}

const_shared_ptr<std::string> SumType::MapSpecifierToVariant(
		const TypeSpecifier& type_specifier) const {
	return make_shared<std::string>(
			m_type_table->MapSpecifierToName(type_specifier));
}

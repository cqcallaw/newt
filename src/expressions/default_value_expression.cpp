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

#include <default_value_expression.h>
#include <record_type.h>
#include <sum_type.h>
#include <type_table.h>
#include <execution_context.h>
#include <nested_type_specifier.h>
#include <member_definition.h>
#include <record_type_specifier.h>
#include <sum_type_specifier.h>

DefaultValueExpression::DefaultValueExpression(const yy::location position,
		const_shared_ptr<TypeSpecifier> type, const yy::location type_position) :
		Expression(position), m_type(type), m_type_position(type_position) {
}

DefaultValueExpression::DefaultValueExpression(
		const DefaultValueExpression* other) :
		Expression(other->GetPosition()), m_type(other->m_type), m_type_position(
				other->m_type_position) {
}

DefaultValueExpression::~DefaultValueExpression() {
}

const_shared_ptr<TypeSpecifier> DefaultValueExpression::GetType(
		const shared_ptr<ExecutionContext> execution_context) const {
	auto as_nested_type = dynamic_pointer_cast<const NestedTypeSpecifier>(
			m_type);
	if (as_nested_type) {
		auto parent_name = *as_nested_type->GetParent()->GetTypeName();
		auto member_name = *as_nested_type->GetMemberName();

		auto record_type =
				execution_context->GetTypeTable()->GetType<RecordType>(
						parent_name);
		if (record_type) {
			return record_type->GetMember(member_name)->GetType();
		}
	}
	return m_type;
}

const_shared_ptr<Result> DefaultValueExpression::Evaluate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	plain_shared_ptr<void> return_value;

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(m_type);
	if (as_primitive) {
		if (as_primitive != PrimitiveTypeSpecifier::GetNone()) {
			return_value = as_primitive->DefaultValue(
					*execution_context->GetTypeTable());
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, "@"), errors);
		}
	}

	const_shared_ptr<RecordTypeSpecifier> as_record = std::dynamic_pointer_cast<
			const RecordTypeSpecifier>(m_type);
	if (as_record) {
		const string type_name = *as_record->GetTypeName();
		const_shared_ptr<RecordType> type =
				execution_context->GetTypeTable()->GetType<RecordType>(
						type_name);

		if (type) {
			return_value = m_type->DefaultValue(
					*execution_context->GetTypeTable());
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name), errors);
		}
	}

	const_shared_ptr<NestedTypeSpecifier> as_nested = std::dynamic_pointer_cast<
			const NestedTypeSpecifier>(m_type);
	if (as_nested) {
		const string type_name = *as_nested->GetParent()->GetTypeName();
		return_value = m_type->DefaultValue(*execution_context->GetTypeTable());

		if (!return_value) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name), errors);
		}
	}

	return make_shared<Result>(return_value, errors);
}

const ErrorListRef DefaultValueExpression::Validate(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(m_type);
	if (as_primitive) {
		if (as_primitive == PrimitiveTypeSpecifier::GetNone()) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, "@"), errors);
		}
	}

	const_shared_ptr<ComplexTypeSpecifier> as_complex =
			std::dynamic_pointer_cast<const ComplexTypeSpecifier>(m_type);
	if (as_complex) {
		if (!execution_context->GetTypeTable()->ContainsType(*as_complex)) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column,
							*as_complex->GetTypeName()), errors);
		}
	}

	const_shared_ptr<NestedTypeSpecifier> as_nested = std::dynamic_pointer_cast<
			const NestedTypeSpecifier>(m_type);
	if (as_nested) {
		auto parent = as_nested->GetParent();

		auto parent_as_record = std::dynamic_pointer_cast<
				const RecordTypeSpecifier>(parent);
		if (parent_as_record) {
			auto definition = execution_context->GetTypeTable()->GetType<
					TypeDefinition>(parent->GetTypeName());
			if (definition) {
				//record type specifier may represent either either record type or sum type
				auto record_definition = dynamic_pointer_cast<const RecordType>(
						definition);
				if (record_definition) {
					auto member_definition = record_definition->GetMember(
							*as_nested->GetMemberName());
					if (member_definition
							== MemberDefinition::GetDefaultMemberDefinition()) {
						errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::UNDECLARED_TYPE,
										m_type_position.begin.line,
										m_type_position.begin.column,
										as_nested->ToString()), errors);
					}
				}

				auto sum_definition = dynamic_pointer_cast<const SumType>(
						definition);
				if (sum_definition) {
					auto table = sum_definition->GetTypeTable();
					auto member_definition = table->GetType<TypeDefinition>(
							*as_nested->GetMemberName());
					if (!member_definition) {
						errors = ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::UNDECLARED_TYPE,
										m_type_position.begin.line,
										m_type_position.begin.column,
										as_nested->ToString()), errors);
					}
				}
			} else {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::UNDECLARED_TYPE,
								m_type_position.begin.line,
								m_type_position.begin.column,
								*parent->GetTypeName()), errors);
			}
		}
	}

	return errors;
}

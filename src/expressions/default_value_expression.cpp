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
#include <compound_type.h>
#include <type_table.h>
#include <execution_context.h>
#include <compound_type_specifier.h>

DefaultValueExpression::DefaultValueExpression(const yy::location position,
		const_shared_ptr<TypeSpecifier> type, const yy::location type_position) :
		Expression(position), m_type(type), m_type_position(type_position) {
}

DefaultValueExpression::~DefaultValueExpression() {
}

const_shared_ptr<TypeSpecifier> DefaultValueExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_type;
}

const Result* DefaultValueExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();
	plain_shared_ptr<void> return_value;

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(m_type);
	if (as_primitive != nullptr) {
		if (as_primitive != PrimitiveTypeSpecifier::GetNone()) {
			return_value = as_primitive->DefaultValue(
					*execution_context->GetTypeTable());
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, "#"));
		}
	}

	const_shared_ptr<CompoundTypeSpecifier> as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(m_type);
	if (as_compound) {
		const string type_name = as_compound->GetTypeName();
		const CompoundType* type = execution_context->GetTypeTable()->GetType(
				type_name);

		if (type != CompoundType::GetDefaultCompoundType()) {
			return_value = m_type->DefaultValue(
					*execution_context->GetTypeTable());
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name));
		}
	}

	return new Result(return_value, errors);
}

const LinkedList<const Error*>* DefaultValueExpression::Validate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
			std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(m_type);
	if (as_primitive != nullptr) {
		if (as_primitive == PrimitiveTypeSpecifier::GetNone()) {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, "#"));
		}
	}

	const_shared_ptr<CompoundTypeSpecifier> as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(m_type);
	if (as_compound) {
		const string type_name = as_compound->GetTypeName();
		const CompoundType* type = execution_context->GetTypeTable()->GetType(
				type_name);

		if (type == CompoundType::GetDefaultCompoundType()) {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name));
		}
	}

	return errors;
}

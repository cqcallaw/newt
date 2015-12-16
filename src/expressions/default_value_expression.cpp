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

DefaultValueExpression::DefaultValueExpression(const YYLTYPE position,
		const TypeSpecifier* type, const YYLTYPE type_position) :
		Expression(position), m_type(type), m_type_position(type_position) {
}

DefaultValueExpression::~DefaultValueExpression() {
}

const TypeSpecifier* DefaultValueExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_type;
}

const Result* DefaultValueExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();
	const void* return_value = nullptr;

	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(m_type);
	if (as_primitive != nullptr) {
		if (as_primitive != PrimitiveTypeSpecifier::GetNone()) {
			return_value = as_primitive->DefaultValue(
					execution_context->GetTypeTable());
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_type_position.first_line,
							m_type_position.first_column, "#"));
		}
	}

	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(m_type);
	if (as_compound) {
		const string type_name = as_compound->GetTypeName();
		const CompoundType* type = execution_context->GetTypeTable()->GetType(
				type_name);

		if (type != CompoundType::GetDefaultCompoundType()) {
			return_value = m_type->DefaultValue(
					execution_context->GetTypeTable());
		} else {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.first_line,
							m_type_position.first_column, type_name));
		}
	}

	return new Result(return_value, errors);
}

const LinkedList<const Error*>* DefaultValueExpression::Validate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const PrimitiveTypeSpecifier* as_primitive =
			dynamic_cast<const PrimitiveTypeSpecifier*>(m_type);
	if (as_primitive != nullptr) {
		if (as_primitive == PrimitiveTypeSpecifier::GetNone()) {
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::INVALID_RIGHT_OPERAND_TYPE,
							m_type_position.first_line,
							m_type_position.first_column, "#"));
		}
	}

	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(m_type);
	if (as_compound) {
		const string type_name = as_compound->GetTypeName();
		const CompoundType* type = execution_context->GetTypeTable()->GetType(
				type_name);

		if (type == CompoundType::GetDefaultCompoundType()) {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.first_line,
							m_type_position.first_column, type_name));
		}
	}

	return errors;
}

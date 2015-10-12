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

#include <inferred_declaration_statement.h>
#include <assert.h>
#include <type_specifier.h>
#include <expression.h>
#include <primitive_declaration_statement.h>
#include <array_declaration_statement.h>
#include <struct_instantiation_statement.h>

InferredDeclarationStatement::InferredDeclarationStatement(
		const std::string* name, const YYLTYPE name_position,
		const Expression* initializer_expression) :
		m_name(name), m_name_position(name_position), m_initializer_expression(
				initializer_expression) {
}

InferredDeclarationStatement::~InferredDeclarationStatement() {
}

const Expression* InferredDeclarationStatement::GetInitializerExpression() const {
	return m_initializer_expression;
}

const TypeSpecifier* InferredDeclarationStatement::GetType() const {
	assert(false);
	return PrimitiveTypeSpecifier::GetNone();
}

const LinkedList<const Error*>* InferredDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const TypeSpecifier* expression_type = m_initializer_expression->GetType(
			execution_context);

	if (dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type)) {
		auto temp_statement = new PrimitiveDeclarationStatement(expression_type,
				m_initializer_expression->GetPosition(), m_name,
				m_name_position, m_initializer_expression);
		errors = temp_statement->preprocess(execution_context);
		delete (temp_statement);
	}

	const ArrayTypeSpecifier* as_array =
			dynamic_cast<const ArrayTypeSpecifier*>(expression_type);
	if (as_array) {
		auto temp_statement = new ArrayDeclarationStatement(as_array,
				m_initializer_expression->GetPosition(), m_name,
				m_name_position, m_initializer_expression);
		errors = temp_statement->preprocess(execution_context);
		delete (temp_statement);
	}

	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(expression_type);
	if (as_compound) {
		auto temp_statement = new StructInstantiationStatement(as_compound,
				m_initializer_expression->GetPosition(), m_name,
				m_name_position, m_initializer_expression);
		errors = temp_statement->preprocess(execution_context);
		delete (temp_statement);
	}

	return errors;
}

const LinkedList<const Error*>* InferredDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const TypeSpecifier* expression_type = m_initializer_expression->GetType(
			execution_context);

	if (dynamic_cast<const PrimitiveTypeSpecifier*>(expression_type)) {
		auto temp_statement = new PrimitiveDeclarationStatement(expression_type,
				m_initializer_expression->GetPosition(), m_name,
				m_name_position, m_initializer_expression);
		errors = temp_statement->execute(execution_context);
		delete (temp_statement);
	}

	const ArrayTypeSpecifier* as_array =
			dynamic_cast<const ArrayTypeSpecifier*>(expression_type);
	if (as_array) {
		auto temp_statement = new ArrayDeclarationStatement(as_array,
				m_initializer_expression->GetPosition(), m_name,
				m_name_position, m_initializer_expression);
		errors = temp_statement->execute(execution_context);
		delete (temp_statement);
	}

	const CompoundTypeSpecifier* as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(expression_type);
	if (as_compound) {
		auto temp_statement = new StructInstantiationStatement(as_compound,
				m_initializer_expression->GetPosition(), m_name,
				m_name_position, m_initializer_expression);
		errors = temp_statement->execute(execution_context);
		delete (temp_statement);
	}

	return errors;
}

const std::string* InferredDeclarationStatement::GetName() const {
	return m_name;
}

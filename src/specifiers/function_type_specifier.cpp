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
#include <typeinfo>
#include <declaration_statement.h>
#include <sstream>
#include <declaration_list.h>
#include <assert.h>
#include <function_result_type_specifier.h>
#include <type_list.h>
#include <function_declaration_statement.h>
#include <function_expression.h>

FunctionTypeSpecifier::FunctionTypeSpecifier(
		const DeclarationList* parameter_list, const TypeSpecifier* return_type) :
		m_parameter_list(parameter_list), m_return_type(return_type) {
}

FunctionTypeSpecifier::~FunctionTypeSpecifier() {
}

const string FunctionTypeSpecifier::ToString() const {
	ostringstream buffer;
	buffer << "(";
	const LinkedList<const DeclarationStatement*>* subject = m_parameter_list;
	while (!subject->IsTerminator()) {
		const DeclarationStatement* statement = subject->GetData();
		buffer << statement->GetType()->ToString() << " "
				<< *statement->GetName();
		subject = subject->GetNext();

		if (!subject->IsTerminator()) {
			//add separator
			buffer << ", ";
		}
	}
	buffer << ") -> " << m_return_type->ToString() << "";
	return buffer.str();
}

const bool FunctionTypeSpecifier::IsAssignableTo(
		const TypeSpecifier* other) const {
	return this == other;
}

const void* FunctionTypeSpecifier::DefaultValue(
		const TypeTable* type_table) const {
	return m_return_type->DefaultValue(type_table);
}

bool FunctionTypeSpecifier::operator ==(const TypeSpecifier& other) const {
	try {
		const FunctionTypeSpecifier& as_function =
				dynamic_cast<const FunctionTypeSpecifier&>(other);
		if (*m_return_type == *as_function.GetReturnType()) {
			const LinkedList<const DeclarationStatement*>* subject =
					m_parameter_list;
			const LinkedList<const DeclarationStatement*>* other_subject =
					as_function.GetParameterList();
			while (!subject->IsTerminator()) {
				const DeclarationStatement* statement = subject->GetData();
				const DeclarationStatement* other_statement =
						other_subject->GetData();
				if (*statement->GetType() == *other_statement->GetType()) {
					subject = subject->GetNext();
					other_subject = other_subject->GetNext();
				} else {
					return false;
				}
			}

			return true;
		} else {
			return false;
		}
	} catch (std::bad_cast& e) {
		return false;
	}
}

const Statement* FunctionTypeSpecifier::GetInferredDeclarationStatement(
		const YYLTYPE position, const std::string* name,
		const YYLTYPE name_position,
		const Expression* initializer_expression) const {
	return new FunctionDeclarationStatement(position, name, name_position,
			static_cast<const FunctionExpression*>(initializer_expression));
}

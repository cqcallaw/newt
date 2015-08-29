/*
 * member_declaration.cpp
 *
 *  Created on: Aug 28, 2015
 *      Author: caleb
 */

#include <member_declaration.h>

MemberDeclaration::MemberDeclaration(const Type type,
		const YYLTYPE type_position, const std::string* name,
		const YYLTYPE name_position, const Expression* initializer_expression,
		const YYLTYPE initializer_expression_position) :
		m_type(type), m_type_position(type_position), m_name(name), m_name_position(
				name_position), m_initializer_expression(
				initializer_expression), m_initializer_expression_position(
				initializer_expression_position) {
}

MemberDeclaration::~MemberDeclaration() {
}


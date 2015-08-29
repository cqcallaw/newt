/*
 * member_declaration.h
 *
 *  Created on: Aug 28, 2015
 *      Author: caleb
 */

#ifndef MEMBER_DECLARATION_H_
#define MEMBER_DECLARATION_H_

#include <defaults.h>
#include <type.h>

class MemberDeclaration {
public:
	MemberDeclaration(const Type type, const YYLTYPE type_position,
			const std::string* name, const YYLTYPE name_position,
			const Expression* initializer_expression = nullptr,
			const YYLTYPE initializer_expression_position = DefaultLocation);
	virtual ~MemberDeclaration();

	const Expression* GetInitializerExpression() const {
		return m_initializer_expression;
	}

	const YYLTYPE GetInitializerExpressionPosition() const {
		return m_initializer_expression_position;
	}

	const std::string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

	const Type GetType() const {
		return m_type;
	}

	const YYLTYPE GetTypePosition() const {
		return m_type_position;
	}

private:
	const Type m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_initializer_expression;
	const YYLTYPE m_initializer_expression_position;
};

#endif /* MEMBER_DECLARATION_H_ */

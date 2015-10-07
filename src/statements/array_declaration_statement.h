/*
 * array_declaration_statement.h
 *
 *  Created on: Jul 28, 2015
 *      Author: caleb
 */

#ifndef STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_
#define STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_

#include "statement.h"
#include <string>
#include <type.h>
#include <defaults.h>
#include <declaration_statement.h>

class Result;
class ArrayTypeSpecifier;

class ArrayDeclarationStatement: public DeclarationStatement {
public:
	ArrayDeclarationStatement(const ArrayTypeSpecifier* type,
			const YYLTYPE type_position, const std::string* name,
			const YYLTYPE name_position, const Expression* size_expression =
					nullptr, const YYLTYPE size_expression_position =
					DefaultLocation);

	virtual ~ArrayDeclarationStatement();

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;

	virtual const Expression* GetInitializerExpression() const {
		return nullptr;
	}

	virtual const TypeSpecifier* GetType() const;

	virtual const std::string* GetName() const;

	const bool IsFixedSize() const;

private:
	const ArrayTypeSpecifier* m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_size_expression;
	const YYLTYPE m_size_expression_position;
};

#endif /* STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_ */

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
class DimensionList;

class ArrayDeclarationStatement: public DeclarationStatement {
public:
	ArrayDeclarationStatement(const ArrayTypeSpecifier* type,
			const YYLTYPE type_position, const std::string* name,
			const YYLTYPE name_position,
			const Expression* initializer_expression = nullptr);

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

private:
	const ArrayTypeSpecifier* m_type;
	const YYLTYPE m_type_position;
	const std::string* m_name;
	const YYLTYPE m_name_position;
	const Expression* m_initializer_expression;
};

#endif /* STATEMENTS_ARRAY_DECLARATION_STATEMENT_H_ */

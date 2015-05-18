/*
 * array_variable.h
 *
 *  Created on: May 18, 2015
 *      Author: caleb
 */

#ifndef ARRAY_VARIABLE_H_
#define ARRAY_VARIABLE_H_

#include <string>
#include "variable.h"

class Expression;

class ArrayVariable: public Variable {
public:
	ArrayVariable(const std::string* name, const Expression* index_expression);
	virtual ~ArrayVariable();

	const Expression* GetIndexExpression() const {
		return m_index_expression;
	}

	virtual const Type GetType() const;

	virtual const std::string* ToString() const;

private:
	const Expression* m_index_expression;
};

std::ostream &operator<<(std::ostream &os, const ArrayVariable &array_variable);

#endif /* ARRAY_VARIABLE_H_ */

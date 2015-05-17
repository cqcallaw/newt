/*
 * expression.h
 *
 *  Created on: Feb 16, 2015
 *      Author: caleb
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <symbol_table.h>
#include <symbol.h>
#include <array_symbol.h>
#include <utils.h>
#include <assert.h>
#include <type.h>

class Expression {
public:
	Expression(const Type type);
	virtual ~Expression();

	const Type GetType() const {
		return m_type;
	}

	virtual const void* Evaluate() const;

	const string* ToString() const;

private:
	const Type m_type;
};

#endif /* EXPRESSION_H_ */

/*
 * variable.h
 *
 *  Created on: Feb 17, 2015
 *      Author: caleb
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <string>
#include "symbol.h"

class Expression;

class Variable {
public:
	Variable(const string* name);
	virtual ~Variable();

	const string* GetName() const {
		return m_name;
	}

	virtual const string* ToString() const;

	const static Variable* DefaultVariable;

	virtual const Type GetType() const;

private:
	const string* m_name;
};

std::ostream &operator<<(std::ostream &os, const Variable &variable);

#endif /* VARIABLE_H_ */

/*
 * parameter.h
 *
 *  Created on: Mar 9, 2015
 *      Author: caleb
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <string>
#include "expression.h"

using namespace std;

class Parameter {
public:
	Parameter(const string* name, const Expression* expression);
	virtual ~Parameter();

	const Expression* GetExpression() const {
		return expression;
	}

	const string* GetName() const {
		return name;
	}

	const static Parameter* DefaultParamater;

private:
	const string* name;
	const Expression* expression;
};

#endif /* PARAMETER_H_ */

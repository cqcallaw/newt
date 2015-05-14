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

class ArrayVariable: public Variable {
public:
	ArrayVariable(const string* name, const Expression* index_expression);
	virtual ~ArrayVariable();

	const Expression* GetIndexExpression() const {
		return m_index_expression;
	}

	virtual const Type GetType() const;

	virtual const string* ToString() const;

private:
	const Expression* m_index_expression;
};

std::ostream &operator<<(std::ostream &os, const ArrayVariable &array_variable);

class MemberVariable: public Variable {
public:
	MemberVariable(const string* name, const string* member_variable_name);
	virtual ~MemberVariable();

	virtual const string* ToString() const;

	virtual const Type GetType() const;

	const string* GetMemberVariableName() const {
		return m_member_variable_name;
	}

private:
	const string* m_member_variable_name;
};

std::ostream &operator<<(std::ostream &os, const MemberVariable &variable);

class ArrayMemberVariable: public ArrayVariable {
public:
	ArrayMemberVariable(const string* name, const Expression* index_expression,
			const string* member_variable_name);
	virtual ~ArrayMemberVariable();

	virtual const string* ToString() const;

	virtual const Type GetType() const;

	const string* GetMemberVariableName() const {
		return m_member_variable_name;
	}

private:
	const string* m_member_variable_name;
};

std::ostream &operator<<(std::ostream &os, const MemberVariable &variable);

#endif /* VARIABLE_H_ */

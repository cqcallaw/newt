/*
 * member_variable.h
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
 */

#ifndef MEMBER_VARIABLE_H_
#define MEMBER_VARIABLE_H_

#include <variable.h>

class MemberVariable: public Variable {
public:
	MemberVariable(const std::string* container_name,
			const YYLTYPE container_name_location,
			const Variable* member_variable);
	virtual ~MemberVariable();

	virtual const TypeSpecifier* GetType(const ExecutionContext* context) const;

	virtual const std::string* ToString(const ExecutionContext* context) const;

	virtual const bool IsBasicReference() const {
		return false;
	}

	const Variable* GetMemberVariable() const {
		return m_member_variable;
	}

private:
	const Variable* m_member_variable;
};

#endif /* MEMBER_VARIABLE_H_ */

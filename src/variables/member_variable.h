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
	MemberVariable(const std::string* name, YYLTYPE location,
			const std::string* member_name, YYLTYPE member_name_location);
	virtual ~MemberVariable();

	virtual const PrimitiveType GetType(const ExecutionContext* context) const;

	virtual const std::string* ToString(const ExecutionContext* context) const;

private:
	const std::string* m_member_name;
	const YYLTYPE m_member_name_location;
};

#endif /* MEMBER_VARIABLE_H_ */

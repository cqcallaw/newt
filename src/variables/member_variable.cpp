/*
 * member_variable.cpp
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
 */

#include <member_variable.h>
#include <execution_context.h>
#include <symbol_table.h>
#include <sstream>

MemberVariable::MemberVariable(const std::string* name, YYLTYPE location,
		const std::string* member_name, YYLTYPE member_name_location) :
		Variable(name, location), m_member_name(member_name), m_member_name_location(
				member_name_location) {

}

MemberVariable::~MemberVariable() {
}

const BasicType MemberVariable::GetType(
		const ExecutionContext* context) const {
	const Symbol* symbol = context->GetSymbolTable()->GetSymbol(*GetName());


	return NONE;
}

const std::string* MemberVariable::ToString(
		const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "." << *m_member_name << ">";
	return new string(buffer.str());
}

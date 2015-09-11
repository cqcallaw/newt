/*
 * member_variable.cpp
 *
 *  Created on: Aug 29, 2015
 *      Author: caleb
 */

#include <compound_type_instance.h>
#include <member_variable.h>
#include <execution_context.h>
#include <symbol_table.h>
#include <sstream>

MemberVariable::MemberVariable(const std::string* container_name,
		const YYLTYPE container_name_location, const Variable* member_variable) :
		Variable(container_name, container_name_location), m_member_variable(
				member_variable) {

}

MemberVariable::~MemberVariable() {
}

const TypeSpecifier* MemberVariable::GetType(
		const ExecutionContext* context) const {
	const CompoundTypeInstance* container =
			(const CompoundTypeInstance*) context->GetSymbolContext()->GetSymbol(
					GetName())->GetValue();
	const ExecutionContext* temp_context = context->WithSymbolContext(
			container->GetDefinition());
	const TypeSpecifier* result = m_member_variable->GetType(temp_context);
	delete (temp_context);
	return result;
}

const std::string* MemberVariable::ToString(
		const ExecutionContext* context) const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "." << *m_member_variable << ">";
	return new string(buffer.str());
}

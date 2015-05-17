/*
 * variable.cpp0
 *
 *  Created on: Feb 17, 2015
 *      Author: caleb
 */

#include <sstream>
#include <defaults.h>

#include "variable.h"
#include "assert.h"
#include "expression.h"

Variable::Variable(const string* name) :
		m_name(name) {
	assert(name != NULL && name != nullptr);
}

const Type Variable::GetType() const {
	return Symbol_table::instance()->GetSymbol(m_name)->GetType();
}

const string* Variable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *m_name << ">";
	return new string(buffer.str());
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
	os << *(variable.ToString());
	return os;
}

Variable::~Variable() {
}

ArrayVariable::ArrayVariable(const string* name,
		const Expression* index_expression) :
		Variable(name), m_index_expression(index_expression) {
	assert(index_expression != NULL && index_expression != nullptr);assert(
			index_expression->GetType() == INT);
}

const Type ArrayVariable::GetType() const {
	switch (Variable::GetType()) {
	case INT_ARRAY:
		return INT;
	case DOUBLE_ARRAY:
		return DOUBLE;
	case STRING_ARRAY:
		return STRING;
	default:
		assert(false);
		return NONE;
	}
}

const string* ArrayVariable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "["
			<< *((int*) m_index_expression->Evaluate()) << "]>";
	return new string(buffer.str());
}

std::ostream &operator<<(std::ostream &os,
		const ArrayVariable &array_variable) {
	os << *(array_variable.ToString());
	return os;
}

ArrayVariable::~ArrayVariable() {
}

MemberVariable::MemberVariable(const string* name,
		const string* member_variable_name) :
		Variable(name), m_member_variable_name(member_variable_name) {
	assert(member_variable_name != NULL && member_variable_name != nullptr);
}

MemberVariable::~MemberVariable() {
}

const string* MemberVariable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "." << *m_member_variable_name << ">";
	return new string(buffer.str());
}

const Type MemberVariable::GetType() const {
	/*const Symbol* symbol = Symbol_table::instance()->GetSymbol(GetName());
	if (symbol->GetType() != GAME_OBJECT) {
		assert(false);
		return NONE;
	}

	Game_object* game_object = (Game_object*) symbol->GetValue();
	Gpl_type type;
	Status status = game_object->get_member_variable_type(
			*m_member_variable_name, type);

	if (status == MEMBER_NOT_OF_GIVEN_TYPE || status == MEMBER_NOT_DECLARED) {
		assert(false);
		return NONE;
	}

	return type;*/
	return NONE;
}

std::ostream& operator <<(std::ostream& os,
		const MemberVariable& member_variable) {
	os << *(member_variable.ToString());
	return os;
}

ArrayMemberVariable::ArrayMemberVariable(const string* name,
		const Expression* index_expression, const string* member_variable_name) :
		ArrayVariable(name, index_expression), m_member_variable_name(
				member_variable_name) {
	assert(member_variable_name != NULL && member_variable_name != nullptr);
}

ArrayMemberVariable::~ArrayMemberVariable() {
}

const string* ArrayMemberVariable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *ArrayVariable::GetName() << "["
			<< *((int*) GetIndexExpression()->Evaluate()) << "]."
			<< *GetMemberVariableName() << ">";
	return new string(buffer.str());
}

const Type ArrayMemberVariable::GetType() const {
	/*const Symbol* symbol = Symbol_table::instance()->GetSymbol(
			ArrayVariable::GetName());
	if (symbol->GetType() != GAME_OBJECT_ARRAY) {
		assert(false);
		return NONE;
	}

	const ArraySymbol* array_symbol = (ArraySymbol*) symbol;

	Game_object* game_object = (Game_object*) array_symbol->GetValue(
			*((int*) GetIndexExpression()->Evaluate()));

	Gpl_type type;
	Status status = game_object->get_member_variable_type(
			*(GetMemberVariableName()), type);

	if (status == MEMBER_NOT_OF_GIVEN_TYPE || status == MEMBER_NOT_DECLARED) {
		assert(false);
		return NONE;
	}

	return type;*/
	//TODO: re-implement this
	return NONE;
}

std::ostream& operator <<(std::ostream& os,
		const ArrayMemberVariable& array_member_variable) {
	os << *(array_member_variable.ToString());
	return os;
}

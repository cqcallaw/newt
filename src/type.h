#ifndef TYPE_H
#define TYPE_H

#include <iostream>

enum Type {
	NONE = 0,
	BOOLEAN = 1,
	INT = 2,
	DOUBLE = 4,
	STRING = 8,
	DOUBLE_ARRAY = 16,
	INT_ARRAY = 32,
	STRING_ARRAY = 64,
};

std::string type_to_string(Type type);

std::ostream &operator<<(std::ostream &os, const Type &type);

enum Status {
	OK, MEMBER_NOT_OF_GIVEN_TYPE, MEMBER_NOT_DECLARED
};

std::string status_to_string(Status status);

enum OperatorType {
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	MOD,
	UNARY_MINUS,
	NOT,
	AND,
	OR,
	EQUAL,
	NOT_EQUAL,
	LESS_THAN,
	LESS_THAN_EQUAL,
	GREATER_THAN,
	GREATER_THAN_EQUAL,
	NEAR,
	TOUCHES,
	SIN,
	COS,
	TAN,
	ASIN,
	ACOS,
	ATAN,
	SQRT,
	FLOOR,
	ABS,
	RANDOM
};

std::string operator_to_string(OperatorType op);

#endif // #ifndef TYPE_H

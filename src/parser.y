/*
    Copyright (C) 2015 The newt Authors.

    This file is part of newt.

    newt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    newt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with newt.  If not, see <http://www.gnu.org/licenses/>.
*/

%define api.pure full
%locations
%define parse.error verbose

%define parse.trace

%code requires {
#include <string>
#include <symbol.h>
#include <symbol_table.h>
#include <expression.h>
#include <variable.h>
#include <parameter.h>
#include <parameterlist.h>
#include <statement.h>
#include <assignment_statement.h>
#include <statement_list.h>
#include <statement_block.h>
#include <execution_context.h>
#include <stack>

#include <type.h>
typedef void* yyscan_t;

}

%lex-param   { yyscan_t scanner }
%parse-param { StatementBlock** main_statement_block }
%parse-param { yyscan_t scanner }

%code {
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <algorithm>

#include <error.h>
#include <utils.h>
#include <array_variable.h>
#include <constant_expression.h>
#include <comparison_expression.h>
#include <arithmetic_expression.h>
#include <logic_expression.h>
#include <unary_expression.h>
#include <binary_expression.h>
//#include <string_concatenation_expression.h>
#include <variable_expression.h>
#include <print_statement.h>
#include <assignment_statement.h>
#include <declaration_statement.h>
#include <array_declaration_statement.h>
#include <exit_statement.h>
#include <if_statement.h>
#include <for_statement.h>
#include <statement_block.h>
#include <defaults.h>

#include <lexer.h>

void yyerror(YYLTYPE* locp, StatementBlock** main_statement_block, yyscan_t scanner, const char* str) {
	Error::parse_error(locp->first_line, string(str));
}

}

%union {
 bool           union_boolean;
 int            union_int;
 double         union_double;
 std::string*   union_string;
 Type           union_type;
 const Expression*          union_expression;
 const Variable*            union_variable;
 OperatorType               union_operator_type;
 const Parameter*           union_parameter_type;
 const ParameterList*       union_parameter_list_type;
 const Symbol*              union_symbol_type;
 const Statement*           union_statement_type;
 const StatementList*       union_statement_list_type;
 const StatementBlock*      union_statement_block_type;
 const AssignmentStatement* union_assignment_statement_type;
}

%token T_BOOLEAN               "bool"
%token T_INT                   "int"
%token T_DOUBLE                "double"
%token T_STRING                "string"

%token <union_boolean> T_TRUE  "boolean true"
%token <union_boolean> T_FALSE "boolean false"

%token T_IF                    "if"
%token T_FOR                   "for"
%token T_ELSE                  "else"
%token <union_int> T_PRINT     "print" // value is line number
%token <union_int> T_EXIT      "exit" // value is line number

%token T_LPAREN              "("
%token T_RPAREN              ")"
%token T_LBRACE              "{"
%token T_RBRACE              "}"
%token T_LBRACKET            "["
%token T_RBRACKET            "]"
%token T_SEMIC               ";"
%token T_COMMA               ","
%token T_PERIOD              "."

%token T_ASSIGN              "="
%token T_PLUS_ASSIGN         "+="
%token T_MINUS_ASSIGN        "-="

%token T_ASTERISK            "*"
%token T_DIVIDE              "/"
%token T_MOD                 "%"
%token T_PLUS                "+"
%token T_MINUS               "-"
%token T_SIN                 "sin"
%token T_COS                 "cos"
%token T_TAN                 "tan"
%token T_ASIN                "asin"
%token T_ACOS                "acos"
%token T_ATAN                "atan"
%token T_SQRT                "sqrt"
%token T_FLOOR               "floor"
%token T_ABS                 "abs"
%token T_RANDOM              "random"

%token T_LESS                "<"
%token T_GREATER             ">"
%token T_LESS_EQUAL          "<="
%token T_GREATER_EQUAL       ">="
%token T_EQUAL               "=="
%token T_NOT_EQUAL           "!="

%token T_AND                 "&&"
%token T_OR                  "||"
%token T_NOT                 "!"

%token T_NEWLINE             "newline"

%token <union_string> T_ID               "identifier"
%token <union_int> T_INT_CONSTANT        "int constant"
%token <union_double > T_DOUBLE_CONSTANT "double constant"
%token <union_string> T_STRING_CONSTANT  "string constant"

%token T_ERROR               "error" // error token

%left T_OR
%left T_AND
%left T_EQUAL T_NOT_EQUAL
%left T_LESS T_LESS_EQUAL T_GREATER T_GREATER_EQUAL
%left T_PLUS T_MINUS
%left T_ASTERISK T_DIVIDE T_MOD

%right T_ASSIGN
%right T_PLUS_ASSIGN
%right T_MINUS_ASSIGN

%nonassoc UNARY_OPS

%precedence IF_NO_ELSE
%precedence T_ELSE

%type <union_type> simple_type
%type <union_expression> expression
%type <union_expression> primary_expression
%type <union_statement_type> variable_declaration
%type <union_variable> variable_reference
%type <union_operator_type> math_operator
%type <union_statement_type> statement
%type <union_statement_list_type> statement_list
%type <union_statement_list_type> padded_statement_list
%type <union_statement_block_type> if_block
%type <union_statement_block_type> statement_block
%type <union_statement_type> if_statement
%type <union_assignment_statement_type> assign_statement
%type <union_statement_type> print_statement
%type <union_statement_type> for_statement
%type <union_statement_type> exit_statement

%% // begin rules

//---------------------------------------------------------------------
program:
	padded_statement_list
	{
		if ($1 != LinkedList<const Statement*>::Terminator) {
			//statement list comes in reverse order
			//wrap in StatementList because Reverse is a LinkedList<T> function
			*main_statement_block = new StatementBlock($1->Reverse(true));
		} else {
			*main_statement_block = new StatementBlock(StatementList::Terminator);
		}
	}
	;

padded_statement_list:
	statement_list optional_newlines
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
optional_newlines:
	T_NEWLINE optional_newlines
	| empty
	;

//---------------------------------------------------------------------
variable_declaration:
	simple_type T_ID
	{
		$$ = new DeclarationStatement($1, @1, $2, @2);
	}
	| simple_type T_ID T_ASSIGN expression
	{
		$$ = new DeclarationStatement($1, @1, $2, @2, $4, @4);
	}
	| simple_type T_ID T_LBRACKET expression T_RBRACKET
	{
		$$ = new ArrayDeclarationStatement($1, @1, $2, @2, $4, @4);
	}
	;

//---------------------------------------------------------------------
simple_type:
	T_BOOLEAN
	{
		$$ = BOOLEAN;
	}
	|
	T_INT
	{
		$$ = INT;
	}
	| T_DOUBLE
	{
		$$ = DOUBLE;
	}
	| T_STRING
	{
		$$ = STRING;
	}
	;

//---------------------------------------------------------------------
if_block:
	statement
	{
		$$ = new StatementBlock(new StatementList($1, StatementList::Terminator));
	}
	| statement_block
	{
		$$ = $1;
	}
	;
	
//---------------------------------------------------------------------
statement_block:
	T_LBRACE padded_statement_list T_RBRACE
	{
		$$ = new StatementBlock($2->Reverse(true)); //statement list comes in reverse order
	}
	;

//---------------------------------------------------------------------
statement_list:
	statement_list optional_newlines statement
	{
		$$ = new StatementList($3, $1);
	}
	| empty
	{
		$$ = StatementList::Terminator;
	}
	;

//---------------------------------------------------------------------
statement:
	variable_declaration
	{
		$$ = $1;
	}
	| if_statement
	{
		$$ = $1;
	}
	| for_statement
	{
		$$ = $1;
	}
	| assign_statement
	{
		$$ = $1;
	}
	| print_statement
	{
		$$ = $1;
	}
	| exit_statement
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
if_statement:
	T_IF T_LPAREN expression T_RPAREN if_block %prec IF_NO_ELSE
	{
		$$ = new IfStatement($3, $5);
	}
	| T_IF T_LPAREN expression T_RPAREN if_block T_ELSE if_block
	{
		$$ = new IfStatement($3, $5, $7);
	}
	;

//---------------------------------------------------------------------
for_statement:
	T_FOR T_LPAREN assign_statement T_SEMIC expression T_SEMIC assign_statement T_RPAREN statement_block
	{
		$$ = new ForStatement($3, $5, $7, $9);
	}
	;

//---------------------------------------------------------------------
print_statement:
	T_PRINT T_LPAREN expression T_RPAREN
	{
		$$ = new PrintStatement($1, $3);
	}
	;

//---------------------------------------------------------------------
exit_statement:
	T_EXIT T_LPAREN expression T_RPAREN
	{
		$$ = new ExitStatement($1, $3);
	}
	;

//---------------------------------------------------------------------
assign_statement:
	variable_reference T_ASSIGN expression
	{
		$$ = new AssignmentStatement($1, AssignmentStatement::AssignmentType::ASSIGN, $3);
	}
	| variable_reference T_PLUS_ASSIGN expression
	{
		$$ = new AssignmentStatement($1, AssignmentStatement::AssignmentType::PLUS_ASSIGN, $3);
	}
	| variable_reference T_MINUS_ASSIGN expression
	{
		$$ = new AssignmentStatement($1, AssignmentStatement::AssignmentType::MINUS_ASSIGN, $3);
	}
	;

//---------------------------------------------------------------------
variable_reference:
	T_ID
	{
		$$ = new Variable($1, @1);
	}
	| T_ID T_LBRACKET expression T_RBRACKET
	{
		$$ = new ArrayVariable($1, @1, $3, @3);
	}
	| T_ID T_PERIOD T_ID
	{
	}
	| T_ID T_LBRACKET expression T_RBRACKET T_PERIOD T_ID
	{
	}
	;

//---------------------------------------------------------------------
expression:
	primary_expression
	{
		$$ = $1;
	}
	| expression T_OR expression
	{
		$$ = new LogicExpression(@$, OR, $1, $3);
	}
	| expression T_AND expression
	{
		$$ = new LogicExpression(@$, AND, $1, $3);
	}
	| expression T_LESS_EQUAL expression
	{
		$$ = new ComparisonExpression(@$, LESS_THAN_EQUAL, $1, $3);
	}
	| expression T_GREATER_EQUAL  expression
	{
		$$ = new ComparisonExpression(@$, GREATER_THAN_EQUAL, $1, $3);
	}
	| expression T_LESS expression 
	{
		$$ = new ComparisonExpression(@$, LESS_THAN, $1, $3);
	}
	| expression T_GREATER  expression
	{
		$$ = new ComparisonExpression(@$, GREATER_THAN, $1, $3);
	}
	| expression T_EQUAL expression
	{
		$$ = new ComparisonExpression(@$, EQUAL, $1, $3);
	}
	| expression T_NOT_EQUAL expression
	{
		$$ = new ComparisonExpression(@$, NOT_EQUAL, $1, $3);
	}
	| expression T_PLUS expression 
	{
		//string concatenation isn't strictly an arithmetic operation, so this is something of a hack
		$$ = new ArithmeticExpression(@$, PLUS, $1, $3);
	}
	| expression T_MINUS expression
	{
		$$ = new ArithmeticExpression(@$, MINUS, $1, $3);
	}
	| expression T_ASTERISK expression
	{
		$$ = new ArithmeticExpression(@$, MULTIPLY, $1, $3);
	}
	| expression T_DIVIDE expression
	{
		$$ = new ArithmeticExpression(@$, DIVIDE, $1, $3);
	}
	| expression T_MOD expression
	{
		$$ = new ArithmeticExpression(@$, MOD, $1, $3);
	}
	| T_MINUS expression %prec UNARY_OPS
	{
		$$ = new UnaryExpression(@$, UNARY_MINUS, $2);
	}
	| T_NOT expression %prec UNARY_OPS
	{
		$$ = new UnaryExpression(@$, NOT, $2);
	}
	| math_operator T_LPAREN expression T_RPAREN
	{
		$$ = new UnaryExpression(@$, $1, $3);
	}
	;

//---------------------------------------------------------------------
primary_expression:
	T_LPAREN expression T_RPAREN
	{
		$$ = $2;
	}
	| variable_reference
	{
		$$ = new VariableExpression(@1, $1);
	}
	| T_INT_CONSTANT
	{
		$$ = new ConstantExpression(@1, $1);
	}
	| T_TRUE
	{
		$$ = new ConstantExpression(@1, true);
	}
	| T_FALSE
	{
		$$ = new ConstantExpression(@1, false);
	}
	| T_DOUBLE_CONSTANT
	{
		$$ = new ConstantExpression(@1, $1);
	}
	| T_STRING_CONSTANT
	{
		$$ = new ConstantExpression(@1, $1);
	}
	;

//---------------------------------------------------------------------
math_operator:
	T_SIN
	{
		$$ = SIN;
	}
	| T_COS
	{
		$$ = COS;
	}
	| T_TAN
	{
		$$ = TAN;
	}
	| T_ASIN
	{
		$$ = ASIN;
	}
	| T_ACOS
	{
		$$ = ACOS;
	}
	| T_ATAN
	{
		$$ = ATAN;
	}
	| T_SQRT
	{
		$$ = SQRT;
	}
	| T_ABS
	{
		$$ = ABS;
	}
	| T_FLOOR
	{
		$$ = FLOOR;
	}
	| T_RANDOM
	{
		$$ = RANDOM;
	}
	;

//---------------------------------------------------------------------
empty:
	;

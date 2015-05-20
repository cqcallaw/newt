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

%option noyywrap

%{

#include <error.h>
#include <parser.h>
#include <iostream>
using namespace std;

Error error_handler; // error reporting object

// The following variable keeps track of what input line flex is reading.
// It will be used for reporting the line numbers of errors.
int line_count = 1;

void get_int_value()
{
  // assumes that the string in yytext is an integer
  // convert the string yytext into an integer
  // put the int into global union used to send values to the parser (record.y)
  // the union and its union_int field are declared in record.y
  yylval.union_int = atoi(yytext);
}

void get_double_value()
{
  // assumes that the string in yytext is a double
  // convert the string yytext into a double
  // put the double into global union used to send values to the parser (record.y)
  // the union and its union_int field are declared in record.y
  yylval.union_double = atof(yytext);
}

void get_string_value()
{
  // Would like to put a string in the union, but the way the union works
  // it has to be a pointer to a string.
  yylval.union_string = new string(yytext);
}

void get_quoted_string_value()
{
  // allocate on heap. could allocate it on the stack, but we might run out of stack space for large strings
  string* as_string = new string(yytext); 
  yylval.union_string = new string(as_string->substr(1, as_string->length() - 2));
  delete (as_string);
}

void error()
{
  // have the error handling object report the problem
  // note: it uses line_count as a global variable
  error_handler.error(Error::ILLEGAL_TOKEN, yytext);

  // create a string to hold the current text
  // put that string in the yylval union so it can be used by parser (.y file)
  yylval.union_string = new string(yytext);
}

bool newline = false;
void check_newline()
{
	//because a newline is a statement and expression terminator, 
	//we may get an error when encountering a newline.
	//thus, we must increment the line counter _after_ the 
	//newline token has been processed by the parser.
	if (newline)
	{
		newline = false;
		line_count++; // count the lines for error reporting
	}
}
void set_newline()
{
	newline = true;
}

%}

%%

[\n]            check_newline(); set_newline(); return T_NEWLINE; 
[ \t\r]         check_newline(); // ignore spaces, tabs and the carriage return

\/\/.*$         check_newline(); // ignore comments

"bool"          check_newline(); return T_BOOLEAN;
"int"           check_newline(); return T_INT;
"double"        check_newline(); return T_DOUBLE;
"string"        check_newline(); return T_STRING;

"if"            check_newline(); return T_IF;
"for"           check_newline(); return T_FOR;
"else"          check_newline(); return T_ELSE;

"exit"          check_newline(); yylval.union_int = line_count; return T_EXIT;
"print"         check_newline(); yylval.union_int = line_count; return T_PRINT;
"true"          check_newline(); return T_TRUE;
"false"         check_newline(); return T_FALSE;

"sin"           check_newline(); return T_SIN;
"cos"           check_newline(); return T_COS;
"tan"           check_newline(); return T_TAN;
"asin"          check_newline(); return T_ASIN;
"acos"          check_newline(); return T_ACOS;
"atan"          check_newline(); return T_ATAN;
"sqrt"          check_newline(); return T_SQRT;

"abs"           check_newline(); return T_ABS;
"floor"         check_newline(); return T_FLOOR;
"random"        check_newline(); return T_RANDOM;

"="             check_newline(); return T_ASSIGN;
"{"             check_newline(); return T_LBRACE;
"}"             check_newline(); return T_RBRACE;
";"             check_newline(); return T_SEMIC;
","             check_newline(); return T_COMMA;

"("             check_newline(); return T_LPAREN;
")"             check_newline(); return T_RPAREN;
"["             check_newline(); return T_LBRACKET;
"]"             check_newline(); return T_RBRACKET;
"."             check_newline(); return T_PERIOD;
"+="            check_newline(); return T_PLUS_ASSIGN;
"-="            check_newline(); return T_MINUS_ASSIGN;

"*"             check_newline(); return T_ASTERISK;
"/"             check_newline(); return T_DIVIDE;
"+"             check_newline(); return T_PLUS;
"-"             check_newline(); return T_MINUS;
"%"             check_newline(); return T_MOD;
"<"             check_newline(); return T_LESS;
">"             check_newline(); return T_GREATER;
"<="            check_newline(); return T_LESS_EQUAL;
">="            check_newline(); return T_GREATER_EQUAL;
"=="            check_newline(); return T_EQUAL;
"!="            check_newline(); return T_NOT_EQUAL;
"!"             check_newline(); return T_NOT;
"&&"            check_newline(); return T_AND;
"||"            check_newline(); return T_OR;

[0-9]+                              check_newline(); get_int_value(); return T_INT_CONSTANT;
([0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+)  check_newline(); get_double_value(); return T_DOUBLE_CONSTANT;
\"(\\.|[^\"])*\"                    check_newline(); get_quoted_string_value(); return T_STRING_CONSTANT; //regex taken from http://stackoverflow.com/a/2039820/577298 and http://stackoverflow.com/a/9260547/577298

[_A-Za-z][_A-Za-z0-9]*              check_newline(); get_string_value(); return T_ID;

.                                   error(); return T_ERROR;

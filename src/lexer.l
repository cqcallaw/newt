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

%}

%%

[\n]            line_count++; // count the lines for error reporting
[ \t\r]         ; // ignore spaces, tabs and the microsoft line feed (\r)

\/\/.*$	; // ignore comments

"bool" return T_BOOLEAN;
"int" return T_INT;
"double" return T_DOUBLE;
"string" return T_STRING;

"if" return T_IF;
"for" return T_FOR;
"else" return T_ELSE;

"exit" yylval.union_int = line_count; return T_EXIT;
"print" yylval.union_int = line_count; return T_PRINT;
"true" return T_TRUE;
"false" return T_FALSE;

"sin" return T_SIN;
"cos" return T_COS;
"tan" return T_TAN;
"asin" return T_ASIN;
"acos" return T_ACOS;
"atan" return T_ATAN;
"sqrt" return T_SQRT;

"abs" return T_ABS;
"floor" return T_FLOOR;
"random" return T_RANDOM;

"="             return T_ASSIGN;
"{"             return T_LBRACE;
"}"             return T_RBRACE;
";"             return T_SEMIC;
","             return T_COMMA;

"("             return T_LPAREN;
")"             return T_RPAREN;
"["             return T_LBRACKET;
"]"             return T_RBRACKET;
"."             return T_PERIOD;
"+="            return T_PLUS_ASSIGN;
"-="            return T_MINUS_ASSIGN;

"*"             return T_ASTERISK;
"/"             return T_DIVIDE;
"+"             return T_PLUS;
"-"             return T_MINUS;
"%"             return T_MOD;
"<"             return T_LESS;
">"             return T_GREATER;
"<="            return T_LESS_EQUAL;
">="            return T_GREATER_EQUAL;
"=="            return T_EQUAL;
"!="            return T_NOT_EQUAL;
"!"             return T_NOT;
"&&"            return T_AND;
"||"            return T_OR;

[0-9]+          get_int_value(); return T_INT_CONSTANT;
([0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+)  get_double_value(); return T_DOUBLE_CONSTANT;
\"(\\.|[^\"])*\" get_quoted_string_value(); return T_STRING_CONSTANT; //regex taken from http://stackoverflow.com/a/2039820/577298 and http://stackoverflow.com/a/9260547/577298

[_A-Za-z][_A-Za-z0-9]*       get_string_value(); return T_ID;

.               error(); return T_ERROR;

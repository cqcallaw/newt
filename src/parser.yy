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
%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.2"
%defines
%define parser_class_name {newt_parser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%locations
%define api.location.type {yy::location}
%define parse.error verbose
%define parse.trace

%code requires {
#include <string>
#include <symbol.h>
#include <symbol_table.h>
#include <expression.h>
#include <variable.h>
#include <parameter.h>
#include <statement.h>
#include <assignment_statement.h>
#include <declaration_statement.h>
#include <record_declaration_statement.h>
#include <statement_block.h>
#include <execution_context.h>
#include <stack>
#include <modifier.h>
#include <member_instantiation.h>
#include <dimension.h>

#include <function_expression.h>
#include <function_variant.h>

#include <type.h>
#include <specifiers/type_specifier.h>
#include <specifiers/function_type_specifier.h>
#include <specifiers/function_declaration.h>
#include <specifiers/nested_type_specifier.h>
#include <specifiers/maybe_type_specifier.h>

#include <namespace_qualifier.h>

#include <match.h>

class Driver;

}

%param { Driver& driver }

%code {
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <algorithm>

#include <error.h>
#include <utils.h>
#include <defaults.h>

#include <basic_variable.h>
#include <array_variable.h>
#include <member_variable.h>

#include <constant_expression.h>
#include <comparison_expression.h>
#include <arithmetic_expression.h>
#include <logic_expression.h>
#include <unary_expression.h>
#include <binary_expression.h>
#include <variable_expression.h>
#include <with_expression.h>
#include <default_value_expression.h>
#include <invoke_expression.h>
#include <using_expression.h>

#include <print_statement.h>
#include <assignment_statement.h>
#include <primitive_declaration_statement.h>
#include <array_declaration_statement.h>
#include <complex_instantiation_statement.h>
#include <function_declaration_statement.h>
#include <inferred_declaration_statement.h>
#include <sum_declaration_statement.h>
#include <type_alias_declaration_statement.h>
#include <nested_declaration_statement.h>
#include <unit_declaration_statement.h>
#include <maybe_declaration_statement.h>
#include <exit_statement.h>
#include <if_statement.h>
#include <for_statement.h>
#include <foreach_statement.h>
#include <invoke_statement.h>
#include <return_statement.h>
#include <match_statement.h>

#include <statement_block.h>

#include <specifiers/array_type_specifier.h>
#include <specifiers/primitive_type_specifier.h>
#include <specifiers/record_type_specifier.h>
#include <specifiers/sum_type_specifier.h>

#include <assignment_type.h>

//for some reason Bison isn't dynamically resizing the stack when needed,
//so we set the initial and max sizes to the same value here.
#define YYINITDEPTH 10000
#define YYMAXDEPTH 10000

#include <driver.h>

void yy::newt_parser::error(const location_type& location, const std::string& message) {
	driver.parser_error(location, message);
}

}

%define api.token.prefix {T_}
%token
	END         0         "end of file"
	BOOLEAN               "bool"
	BYTE                  "byte"
	INT                   "int"
	DOUBLE                "double"
	STRING                "string"

	IF                    "if"
	FOR                   "for"
	ELSE                  "else"

	LPAREN              "("
	RPAREN              ")"
	LBRACE              "{"
	RBRACE              "}"
	LBRACKET            "["
	RBRACKET            "]"
	COLON               ":"
	DOUBLE_COLON        "::"
	SEMICOLON           ";"
	COMMA               ","
	PERIOD              "."
	AT                  "@"
	QMARK               "?"

	EQUALS              "="
	PLUS_ASSIGN         "+="
	MINUS_ASSIGN        "-="
	
	ASTERISK            "*"
	DIVIDE              "/"
	PERCENT             "%"
	PLUS                "+"
	MINUS               "-"
	AMPERSAND           "&"

	LESS                "<"
	GREATER             ">"
	LESS_EQUAL          "<="
	GREATER_EQUAL       ">="
	EQUAL               "=="
	NOT_EQUAL           "!="

	PIPE                 "|"

	AND                 "&&"
	OR                  "||"
	NOT                 "!"

	ARROW_RIGHT         "->"

	UNDERSCORE          "_"

	MUTABLE             "mutable modifier"
	WITH                "with"
	MATCH               "match"
	AS                  "as"
	IN                  "in"
	USING                 "using"

	RETURN              "return"
	PRINT     "print"
	EXIT      "exit"

	ERROR               "error"
;

%token <bool> TRUE "boolean true"
%token <bool> FALSE "boolean false"
%token <plain_shared_ptr<std::string>> IDENTIFIER "identifier"
%token <int> INT_CONSTANT "int constant"
%token <std::uint8_t> BYTE_CONSTANT "byte constant"
%token <double> DOUBLE_CONSTANT "double constant"
%token <plain_shared_ptr<std::string>> STRING_CONSTANT "string constant"

%left LBRACKET
%left PERIOD
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left ASTERISK DIVIDE PERCENT WITH

%right EQUALS
%right PLUS_ASSIGN
%right MINUS_ASSIGN

%nonassoc UNARY_OPS

%precedence IF_NO_ELSE
%precedence ELSE

%type <plain_shared_ptr<TypeSpecifier>> type_specifier
%type <plain_shared_ptr<PrimitiveTypeSpecifier>> primitive_type_specifier
%type <plain_shared_ptr<FunctionDeclaration>> function_declaration
%type <plain_shared_ptr<FunctionTypeSpecifier>> function_type_specifier
%type <plain_shared_ptr<ComplexTypeSpecifier>> complex_type_specifier
%type <plain_shared_ptr<NestedTypeSpecifier>> nested_type_specifier
%type <plain_shared_ptr<MaybeTypeSpecifier>> maybe_type_specifier

%type <plain_shared_ptr<Expression>> expression
%type <plain_shared_ptr<Expression>> variable_expression
%type <plain_shared_ptr<FunctionExpression>> function_expression
%type <plain_shared_ptr<FunctionVariant>> function_variant
%type <FunctionVariantListRef> function_variant_list
%type <plain_shared_ptr<Expression>> invoke_expression
%type <plain_shared_ptr<Expression>> using_expression
%type <plain_shared_ptr<Expression>> optional_initializer

%type <plain_shared_ptr<Variable>> variable_reference

%type <plain_shared_ptr<Statement>> statement
%type <StatementListRef> statement_list
%type <plain_shared_ptr<DeclarationStatement>> variable_declaration
%type <plain_shared_ptr<StatementBlock>> if_block
%type <plain_shared_ptr<StatementBlock>> statement_block
%type <plain_shared_ptr<Statement>> if_statement
%type <plain_shared_ptr<AssignmentStatement>> assign_statement
%type <plain_shared_ptr<Statement>> print_statement
%type <plain_shared_ptr<Statement>> for_statement
%type <plain_shared_ptr<Statement>> exit_statement
%type <plain_shared_ptr<RecordDeclarationStatement>> struct_declaration_statement
%type <plain_shared_ptr<DeclarationStatement>> sum_declaration_statement
%type <plain_shared_ptr<RecordDeclarationStatement>> struct_body
%type <plain_shared_ptr<DeclarationStatement>> sum_body
%type <plain_shared_ptr<Statement>> return_statement
%type <plain_shared_ptr<Statement>> match_statement
%type <DeclarationListRef> parameter_list
%type <DeclarationListRef> optional_parameter_list
%type <TypeSpecifierListRef> anonymous_parameter_list
%type <TypeSpecifierListRef> optional_anonymous_parameter_list
%type <ArgumentListRef> argument_list
%type <ArgumentListRef> optional_argument_list

%type <plain_shared_ptr<Modifier>> modifier
%type <ModifierListRef> modifier_list

%type <DeclarationListRef> declaration_list
%type <plain_shared_ptr<MemberInstantiation>> member_instantiation
%type <MemberInstantiationListRef> member_instantiation_list
%type <MemberInstantiationListRef> optional_member_instantiation_list
%type <MemberInstantiationListRef> member_instantiation_block

%type <plain_shared_ptr<Dimension>> dimension
%type <DimensionListRef> dimensions

%type <plain_shared_ptr<NamespaceQualifier>> namespace_qualifier
%type <NamespaceQualifierListRef> namespace_qualifier_list

%type <plain_shared_ptr<DeclarationStatement>> variant
%type <DeclarationListRef> variant_list

%type <plain_shared_ptr<Match>> match_condition
%type <MatchListRef> match_condition_list

%printer { yyoutput << $$; } <*>;

%% // begin rules

//---------------------------------------------------------------------
program:
	statement_list
	{
		//statement list comes in reverse order
		//wrap in StatementListRef because Reverse is a LinkedList<T> function
		plain_shared_ptr<StatementBlock> main_statement_block = make_shared<const StatementBlock>(StatementList::Reverse($1), @1);
		driver.SetStatementBlock(main_statement_block);
	}
	;

//---------------------------------------------------------------------
variable_declaration:
	IDENTIFIER COLON primitive_type_specifier optional_initializer
	{
		$$ = make_shared<PrimitiveDeclarationStatement>(@$, $3, @3, $1, @1, $4);
	}
	| IDENTIFIER COLON type_specifier dimensions optional_initializer
	{
		plain_shared_ptr<TypeSpecifier> type_specifier = $3;
		//add dimensions to type specifier
		DimensionListRef dimension = $4;
		while (!DimensionList::IsTerminator(dimension)) {
			type_specifier = make_shared<ArrayTypeSpecifier>(type_specifier);
			dimension = dimension->GetNext();
		}

		const_shared_ptr<ArrayTypeSpecifier> array_type_specifier = std::dynamic_pointer_cast<const ArrayTypeSpecifier>(type_specifier);
		$$ = make_shared<ArrayDeclarationStatement>(@$, array_type_specifier, @3, $1, @1, $5);
	}
	| IDENTIFIER COLON complex_type_specifier optional_initializer
	{
		$$ = make_shared<ComplexInstantiationStatement>(@$, $3, @3, $1, @1, $4);
	}
	| IDENTIFIER COLON function_type_specifier optional_initializer
	{
		$$ = make_shared<FunctionDeclarationStatement>(@$, $3, $1, @1, $4);
	}
	| IDENTIFIER COLON nested_type_specifier optional_initializer
	{
		$$ = make_shared<NestedDeclarationStatement>(@$, $3, @3, $1, @1, $4);
	}
	| IDENTIFIER COLON maybe_type_specifier optional_initializer
	{
		$$ = make_shared<MaybeDeclarationStatement>(@$, $3, @3, $1, @1, $4);
	}
	| IDENTIFIER COLON EQUALS expression
	{
		$$ = make_shared<InferredDeclarationStatement>(@$, $1, @1, $4);
	}
	;

//---------------------------------------------------------------------
optional_initializer:
	EQUALS expression
	{
		$$ = $2;
	}
	| empty {
		$$ = nullptr;
	}
	;

//---------------------------------------------------------------------
primitive_type_specifier:
	BOOLEAN
	{
		$$ = PrimitiveTypeSpecifier::GetBoolean();
	}
	| BYTE
	{
		$$ = PrimitiveTypeSpecifier::GetByte();
	}
	| INT
	{
		$$ = PrimitiveTypeSpecifier::GetInt();
	}
	| DOUBLE
	{
		$$ = PrimitiveTypeSpecifier::GetDouble();
	}
	| STRING
	{
		$$ = PrimitiveTypeSpecifier::GetString();
	}
	;

//---------------------------------------------------------------------
function_type_specifier:
	LPAREN optional_anonymous_parameter_list RPAREN ARROW_RIGHT type_specifier
	{
		const TypeSpecifierListRef type_list = TypeSpecifierList::Reverse($2);
		$$ = make_shared<FunctionTypeSpecifier>(type_list, $5, @$);
	}
	;

//---------------------------------------------------------------------
complex_type_specifier:
	IDENTIFIER
	{
		$$ = make_shared<ComplexTypeSpecifier>($1, @$);
	}
	| namespace_qualifier_list IDENTIFIER
	{
		const NamespaceQualifierListRef namespace_qualifier_list = NamespaceQualifierList::Reverse($1);
		$$ = make_shared<ComplexTypeSpecifier>($2, nullptr, namespace_qualifier_list, @$);
	}
	;

//---------------------------------------------------------------------
nested_type_specifier:
	complex_type_specifier PERIOD IDENTIFIER
	{
		$$ = make_shared<NestedTypeSpecifier>($1, $3, @$);
	}
	| nested_type_specifier PERIOD IDENTIFIER
	{
		$$ = make_shared<NestedTypeSpecifier>($1, $3, @$);
	}
	;

//---------------------------------------------------------------------
maybe_type_specifier:
	primitive_type_specifier QMARK
	{
		$$ = make_shared<MaybeTypeSpecifier>($1, @$);
	}
	| complex_type_specifier QMARK
	{
		$$ = make_shared<MaybeTypeSpecifier>($1, @$);
	}
	| nested_type_specifier QMARK
	{
		$$ = make_shared<MaybeTypeSpecifier>($1, @$);
	}
	;

//---------------------------------------------------------------------
type_specifier:
	primitive_type_specifier
	{
		$$ = $1;
	}
	| complex_type_specifier
	{
		$$ = $1;
	}
	| function_type_specifier
	{
		$$ = $1;
	}
	| nested_type_specifier
	{
		$$ = $1;
	}
	| maybe_type_specifier
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
statement_block:
	LBRACE statement_list RBRACE
	{
		$$ = make_shared<StatementBlock>(StatementList::Reverse($2), @2); //statement list comes in reverse order
	}
	;

//---------------------------------------------------------------------
statement_list:
	statement_list statement
	{
		$$ = StatementList::From($2, $1);
	}
	| empty
	{
		$$ = StatementList::GetTerminator();
	}
	;

//---------------------------------------------------------------------
statement:
	variable_declaration
	{
		$$ = $1;
	}
	| struct_declaration_statement
	{
		$$ = $1;
	}
	| sum_declaration_statement
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
	| return_statement
	{
		$$ = $1;
	}
	| match_statement
	{
		$$ = $1;
	}
	| variable_reference LPAREN optional_argument_list RPAREN
	{
		const ArgumentListRef argument_list = ArgumentList::Reverse($3);
		$$ = make_shared<InvokeStatement>($1, argument_list, @3);
	}
	;

//---------------------------------------------------------------------
if_block:
	statement
	{
		$$ = make_shared<StatementBlock>(StatementList::From($1, StatementList::GetTerminator()), @1);
	}
	| statement_block
	{
		$$ = $1;
	}
	;
	
//---------------------------------------------------------------------
if_statement:
	IF LPAREN expression RPAREN if_block %prec IF_NO_ELSE
	{
		$$ = make_shared<IfStatement>($3, $5);
	}
	| IF LPAREN expression RPAREN if_block ELSE if_block
	{
		$$ = make_shared<IfStatement>($3, $5, $7);
	}
	;

//---------------------------------------------------------------------
for_statement:
	FOR LPAREN assign_statement SEMICOLON expression SEMICOLON assign_statement RPAREN statement_block
	{
		$$ = make_shared<ForStatement>($3, $5, $7, $9);
	}
	|
	FOR LPAREN variable_declaration SEMICOLON expression SEMICOLON assign_statement RPAREN statement_block
	{
		$$ = make_shared<ForStatement>($3, $5, $7, $9);
	}
	| FOR IDENTIFIER IN expression statement_block
	{
		$$ = make_shared<ForeachStatement>($2, $4, $5);
	}
	;

//---------------------------------------------------------------------
print_statement:
	PRINT LPAREN expression RPAREN
	{
		$$ = make_shared<PrintStatement>(@1.begin.line, $3);
	}
	;

//---------------------------------------------------------------------
exit_statement:
	EXIT LPAREN expression RPAREN
	{
		$$ = make_shared<ExitStatement>($3);
	}
	| PRINT LPAREN RPAREN
	{
		$$ = make_shared<ExitStatement>();
	}
	;

//---------------------------------------------------------------------
assign_statement:
	variable_reference EQUALS expression
	{
		$$ = make_shared<const AssignmentStatement>($1, AssignmentType::ASSIGN, $3);
	}
	| variable_reference PLUS_ASSIGN expression
	{
		$$ = make_shared<const AssignmentStatement>($1, AssignmentType::PLUS_ASSIGN, $3);
	}
	| variable_reference MINUS_ASSIGN expression
	{
		$$ = make_shared<const AssignmentStatement>($1, AssignmentType::MINUS_ASSIGN, $3);
	}
	;

//---------------------------------------------------------------------
return_statement:
	RETURN expression
	{
		$$ = make_shared<ReturnStatement>($2);
	}
	;

//---------------------------------------------------------------------
match_statement:
	MATCH LPAREN expression RPAREN match_condition_list
	{
		auto reverse = MatchList::Reverse($5);
		$$ = make_shared<MatchStatement>(@$, $3, reverse, @5);
	}
	;

//---------------------------------------------------------------------
match_condition_list:
	match_condition_list PIPE match_condition
	{
		$$ = MatchList::From($3, $1);
	}
	| match_condition
	{
		$$ = MatchList::From($1, MatchList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
match_condition:
	IDENTIFIER statement_block
	{
		$$ = make_shared<Match>($1, @1, $1, @1, $2);
	}
	| IDENTIFIER AS IDENTIFIER statement_block
	{
		$$ = make_shared<Match>($1, @1, $3, @3, $4);
	}
	| UNDERSCORE statement_block
	{
		$$ = make_shared<Match>(Match::DEFAULT_MATCH_NAME, @1, Match::DEFAULT_MATCH_NAME, GetDefaultLocation(), $2);
	}
	;
	

//---------------------------------------------------------------------
variable_reference:
	IDENTIFIER
	{
		$$ = make_shared<BasicVariable>($1, @1);
	}
	| namespace_qualifier_list IDENTIFIER
	{
		const NamespaceQualifierListRef namespace_qualifier_list = NamespaceQualifierList::Reverse($1);
		$$ = make_shared<BasicVariable>($2, @2, namespace_qualifier_list);
	}
	| variable_reference LBRACKET expression RBRACKET
	{
		$$ = make_shared<ArrayVariable>($1, $3);
	}
	| variable_reference PERIOD variable_reference
	{
		$$ = make_shared<MemberVariable>($1, $3);
	}
	;

//---------------------------------------------------------------------
expression:
	LPAREN expression RPAREN
	{
		$$ = $2;
	}
	| variable_expression
	{
		$$ = $1;
	}
	| TRUE
	{
		$$ = make_shared<const ConstantExpression>(@1, true);
	}
	| FALSE
	{
		$$ = make_shared<const ConstantExpression>(@1, false);
	}
	| BYTE_CONSTANT
	{
		$$ = make_shared<const ConstantExpression>(@1, $1);
	}
	| INT_CONSTANT
	{
		$$ = make_shared<const ConstantExpression>(@1, $1);
	}
	| DOUBLE_CONSTANT
	{
		$$ = make_shared<const ConstantExpression>(@1, $1);
	}
	| STRING_CONSTANT
	{
		$$ = make_shared<const ConstantExpression>(@1, $1);
	}
	| expression OR expression
	{
		$$ = make_shared<const LogicExpression>(@$, OR, $1, $3);
	}
	| expression AND expression
	{
		$$ = make_shared<const LogicExpression>(@$, AND, $1, $3);
	}
	| expression LESS_EQUAL expression
	{
		$$ = make_shared<const ComparisonExpression>(@$, LESS_THAN_EQUAL, $1, $3);
	}
	| expression GREATER_EQUAL  expression
	{
		$$ = make_shared<const ComparisonExpression>(@$, GREATER_THAN_EQUAL, $1, $3);
	}
	| expression LESS expression 
	{
		$$ = make_shared<const ComparisonExpression>(@$, LESS_THAN, $1, $3);
	}
	| expression GREATER  expression
	{
		$$ = make_shared<const ComparisonExpression>(@$, GREATER_THAN, $1, $3);
	}
	| expression EQUAL expression
	{
		$$ = make_shared<const ComparisonExpression>(@$, EQUAL, $1, $3);
	}
	| expression NOT_EQUAL expression
	{
		$$ = make_shared<const ComparisonExpression>(@$, NOT_EQUAL, $1, $3);
	}
	| expression PLUS expression 
	{
		//string concatenation isn't strictly an arithmetic operation, so this is something of a hack
		$$ = make_shared<const ArithmeticExpression>(@$, PLUS, $1, $3);
	}
	| expression MINUS expression
	{
		$$ = make_shared<const ArithmeticExpression>(@$, MINUS, $1, $3);
	}
	| expression ASTERISK expression
	{
		$$ = make_shared<const ArithmeticExpression>(@$, MULTIPLY, $1, $3);
	}
	| expression DIVIDE expression
	{
		$$ = make_shared<const ArithmeticExpression>(@$, DIVIDE, $1, $3);
	}
	| expression PERCENT expression
	{
		$$ = make_shared<const ArithmeticExpression>(@$, MOD, $1, $3);
	}
	| MINUS expression %prec UNARY_OPS
	{
		$$ = make_shared<const UnaryExpression>(@$, UNARY_MINUS, $2);
	}
	| NOT expression %prec UNARY_OPS
	{
		$$ = make_shared<const UnaryExpression>(@$, NOT, $2);
	}
	| AT type_specifier
	{
		$$ = make_shared<const DefaultValueExpression>(@$, $2, @2);
	}
	| expression WITH member_instantiation_block
	{
		$$ = make_shared<const WithExpression>(@$, $1, $3, @3);
	}
	| function_expression
	{
		$$ = $1;
	}
	| invoke_expression
	{
		$$ = $1;
	}
	| using_expression
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
variable_expression:
	variable_reference
	{
		$$ = make_shared<VariableExpression>(@1, $1);
	}
	;

//---------------------------------------------------------------------
invoke_expression:
	variable_expression LPAREN optional_argument_list RPAREN
	{
		const ArgumentListRef argument_list = ArgumentList::Reverse($3);
		$$ = make_shared<InvokeExpression>(@$, $1, argument_list, @3);
	}
	| invoke_expression LPAREN optional_argument_list RPAREN
	{
		const ArgumentListRef argument_list = ArgumentList::Reverse($3);
		$$ = make_shared<InvokeExpression>(@$, $1, argument_list, @3);
	}
	| function_expression LPAREN optional_argument_list RPAREN
	{
		const ArgumentListRef argument_list = ArgumentList::Reverse($3);
		$$ = make_shared<InvokeExpression>(@$, $1, argument_list, @3);
	}
	;

//---------------------------------------------------------------------
function_declaration:
	LPAREN optional_parameter_list RPAREN ARROW_RIGHT type_specifier
	{
		const DeclarationListRef parameter_list = DeclarationList::Reverse($2);
		$$ = make_shared<FunctionDeclaration>(parameter_list, $5, @$);
	}
	;

//---------------------------------------------------------------------
function_expression:
	function_variant_list
	{
		// put function variants in order
		const FunctionVariantListRef function_variant_list = FunctionVariantList::Reverse($1);
		$$ = make_shared<FunctionExpression>(@$, function_variant_list);
	}
	;

//---------------------------------------------------------------------
function_variant:
	function_declaration statement_block
	{
		auto variant = make_shared<FunctionVariant>(@$, $1, $2);
		$$ = variant; 
		//$$ = FunctionVariantList::From(variant, FunctionVariantList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
function_variant_list:
	function_variant_list AMPERSAND function_variant
	{
		//auto variant = make_shared<FunctionVariant>(@$, $3, $4); 
		$$ = FunctionVariantList::From($3, $1);
	}
	| function_variant
	{
		//auto variant = make_shared<FunctionVariant>(@$, $1, $2); 
		$$ = FunctionVariantList::From($1, FunctionVariantList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
using_expression:
	USING expression AS IDENTIFIER ARROW_RIGHT type_specifier statement_block
	{
		$$ = make_shared<UsingExpression>(@$, $2, $4, $6, $7);
	}
	;

//---------------------------------------------------------------------
optional_parameter_list:
	parameter_list
	{
		$$ = $1;
	}
	| empty
	{
		$$ = DeclarationList::GetTerminator();
	}
	;

//---------------------------------------------------------------------
parameter_list:
	parameter_list COMMA variable_declaration
	{
		$$ = DeclarationList::From($3, $1);
	}
	| variable_declaration
	{
		$$ = DeclarationList::From($1, DeclarationList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
optional_anonymous_parameter_list:
	anonymous_parameter_list
	{
		$$ = $1;
	}
	| empty
	{
		$$ = TypeSpecifierList::GetTerminator();
	}
	;

//---------------------------------------------------------------------
anonymous_parameter_list:
	anonymous_parameter_list COMMA type_specifier
	{
		$$ = TypeSpecifierList::From($3, $1);
	}
	| type_specifier
	{
		$$ = TypeSpecifierList::From($1, TypeSpecifierList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
optional_argument_list:
	argument_list
	{
		$$ = $1;
	}
	| empty
	{
		$$ = ArgumentList::GetTerminator();
	}
	;

//---------------------------------------------------------------------
argument_list:
	argument_list COMMA expression
	{
		$$ = ArgumentList::From($3, $1);
	}
	| expression
	{
		$$ = ArgumentList::From($1, ArgumentList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
modifier_list:
	modifier_list modifier
	{
		$$ = ModifierList::From($2, $1);
	}
	| modifier
	{
		$$ = ModifierList::From($1, ModifierList::GetTerminator());
	}

//---------------------------------------------------------------------
modifier:
	MUTABLE
	{
		$$ = make_shared<Modifier>(Modifier(Modifier::MUTABLE, @1));
	}

//---------------------------------------------------------------------
struct_declaration_statement:
	modifier_list struct_body
	{
		ModifierListRef modifier_list = ModifierList::From(ModifierList::Reverse($1));
		$$ = $2->WithModifiers(modifier_list, @1);
	}
	| struct_body
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
struct_body:
	IDENTIFIER LBRACE declaration_list RBRACE
	{
		const DeclarationListRef member_declaration_list = DeclarationList::Reverse($3);
		const_shared_ptr<RecordTypeSpecifier> type = make_shared<RecordTypeSpecifier>($1, @$);
		$$ = make_shared<RecordDeclarationStatement>(@$, type, $1, @1, member_declaration_list, @3, ModifierList::GetTerminator(), GetDefaultLocation());
	}

//---------------------------------------------------------------------
declaration_list:
	declaration_list COMMA variable_declaration
	{
		$$ = DeclarationList::From($3, $1);
	}
	| declaration_list COMMA sum_body
	{
		$$ = DeclarationList::From($3, $1);
	}
	| declaration_list COMMA struct_body
	{
		$$ = DeclarationList::From($3, $1);
	}
	| struct_body
	{
		$$ = DeclarationList::From($1, DeclarationList::GetTerminator());
	}
	| sum_body
	{
		$$ = DeclarationList::From($1, DeclarationList::GetTerminator());
	}
	| variable_declaration
	{
		$$ = DeclarationList::From($1, DeclarationList::GetTerminator());
	}
	| empty
	{
		$$ = DeclarationList::GetTerminator();
	}
	;

//---------------------------------------------------------------------
member_instantiation_block:
	LBRACE optional_member_instantiation_list RBRACE
	{
		$$ = $2;
	}
	;

//---------------------------------------------------------------------
optional_member_instantiation_list:
	member_instantiation_list
	{
		$$ = $1;
	}
	| empty
	{
		$$ = MemberInstantiationList::GetTerminator();
	}
	;

//---------------------------------------------------------------------
member_instantiation_list:
	member_instantiation_list COMMA member_instantiation
	{
		$$ = MemberInstantiationList::From($3, $1);
	}
	| member_instantiation
	{
		$$ = MemberInstantiationList::From($1, MemberInstantiationList::GetTerminator());
	}

//---------------------------------------------------------------------
member_instantiation:
	IDENTIFIER EQUALS expression
	{
		$$ = make_shared<MemberInstantiation>(MemberInstantiation($1, @1, $3));
	}
	;

//---------------------------------------------------------------------
dimensions:
	dimensions dimension
	{
		$$ = DimensionList::From($2, $1);
	}
	| dimension
	{
		$$ = DimensionList::From($1, DimensionList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
dimension:
	LBRACKET RBRACKET
	{
		$$ = make_shared<Dimension>(Dimension(@$));
	}
	;

//---------------------------------------------------------------------
namespace_qualifier_list:
	namespace_qualifier_list namespace_qualifier
	{
		$$ = NamespaceQualifierList::From($2, $1);
	}
	| namespace_qualifier
	{
		$$ = NamespaceQualifierList::From($1, NamespaceQualifierList::GetTerminator());
	}
	;

//---------------------------------------------------------------------
namespace_qualifier:
	IDENTIFIER DOUBLE_COLON
	{
		$$ = make_shared<NamespaceQualifier>($1);
	}
	;

//---------------------------------------------------------------------
sum_declaration_statement:
	sum_body
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
sum_body:
	IDENTIFIER LBRACE variant_list RBRACE
	{
		const DeclarationListRef variant_list = DeclarationList::Reverse($3);
		const_shared_ptr<SumTypeSpecifier> type = make_shared<SumTypeSpecifier>($1);
		$$ = make_shared<SumDeclarationStatement>(@$, type, $1, @1, variant_list, @3);
	}
	;

//---------------------------------------------------------------------
variant_list:
	variant_list PIPE variant
	{
		$$ = DeclarationList::From($3, $1);
	}
	| variant PIPE variant
	{
		$$ = DeclarationList::From($3,
			DeclarationList::From($1, DeclarationList::GetTerminator()));
	}
	;

//---------------------------------------------------------------------
variant:
	struct_declaration_statement
	{
		$$ = $1;
	}
	| sum_body
	{
		$$ = $1;
	}
	| IDENTIFIER COLON primitive_type_specifier
	{
		$$ = make_shared<TypeAliasDeclarationStatement>(@$, $3, @3, $1, @1);
	}
	| IDENTIFIER COLON complex_type_specifier
	{
		$$ = make_shared<TypeAliasDeclarationStatement>(@$, $3, @3, $1, @1);
	}
	| IDENTIFIER COLON maybe_type_specifier
	{
		$$ = make_shared<TypeAliasDeclarationStatement>(@$, $3, @3, $1, @1);
	}
	| IDENTIFIER
	{
		const_shared_ptr<ComplexTypeSpecifier> type = make_shared<ComplexTypeSpecifier>($1);
		$$ = make_shared<UnitDeclarationStatement>(@$, type, @1, $1, @1);
	}
	;

empty:
	;

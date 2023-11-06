%{
#include <iostream>
#include "classes.h"
void yyerror(char const* s);
extern int yylex(void);

using namespace std;
%}

%union {
    int int_literal;
    char char_literal;
    bool boolean_literal;
    string *string_literal;
    string *identifier;
	SimpleType* simpleType;
	TypeName* typeName;
	ArrayType* arrayType;
	Argument* argument;
	ArgumentList* argumentList;
	ObjectInitializer* objectInitializer;
	ObjectCreation* objectCreation;
	MemberInitializer* memberInitializer;
	MemberInitializerList* memberInitializerList;
	Expression* expression;
	ExpressionList* expressionList;
	ArrayInitializer* arrayInitializer;
	ArrayCreation* arrayCreation;
	ElementAccess* elementAccess;
	InvocationExpression* invocationExpression;
	VarDeclarator* varDeclarator;
	VarDeclaratorList* varDeclaratorList;
	Statement* statement;
	StatementList* statementList;
	IfStatement* ifStatement;
	WhileStatement* whileStatement;
	DoStatement* doStatement;
	ForeachStatement* foreachStatement;
	ForStatement* forStatement;
	ReturnStatement* returnStatement;
	ParamList* paramList;
	Modifier* modifier;
	ModifielrList* modifielrList;
	ClassMember* classMember;
	ClassMemberList* classMemberList;
	Method* method;
	Field* field;
	Constructor* constructor;
	ClassDeclaration* classDeclaration;
	NamespaceMember* namespaceMember
	NamespaceMemberList* namespaceMemberList;
	NamespaceDeclaration* namespaceDeclaration;
	Programm* programm;
	
	
	
}

%token ABSTRACT 
%token VIRTUAL 
%token OVERRIDE

%token BASE
%token THIS
%token CLASS
%token NAMESPACE
%token STATIC
%token RETURN

%token PUBLIC
%token INTERNAL
%token PROTECTED
%token PRIVATE

%token DO
%token IF
%token FOR
%token FOREACH
%token IN
%token WHILE

%token VOID
%token INT
%token CHAR
%token STRING
%token BOOL

%token <int_literal>INT_LITERAL
%token <char_literal>CHAR_LITERAL
%token <string_literal>STRING_LITERAL
%token <boolean_literal>BOOLEAN_LITERAL

%token <identifier>ID

%right '='
%left  OR
%left  AND
%left  EQUALITY INEQUALITY
%left  '<' '>' LESS_EQUAL GREATER_EQUAL IS AS
%left  '+' '-'
%left  '*' '/' '%'
%right '!' UNMINUS
%left  '.' '[' ']'
%right NEW
%nonassoc '(' ')'
%nonassoc THEN
%nonassoc ELSE

%start program

%type <simpleType>type
%type <typeName>type_name
%type <arrayType>array_type
%type <argument>argm
%type <argumentList>argm_list argm_list_em
%type <memberInitializer>member_initializer
%type <memberInitializerList>member_initializer_list member_initializer_list_em
%type <objectInitializer>obj_initializer
%type <objectCreation>obj_creation_expr
%type <expression>expr member_access
%type <expressionList>expr_list expr_list_em
%type <arrayInitializer>array_initializer
%type <arrayCreation>array_creation_expr
%type <elementAccess>element_access
%type <invocationExpression>invocation_expression

%%

program: namespace_member_declaration_list_em { $$ = $1; }
       ;


namespace_declaration: NAMESPACE type_name '{' namespace_member_declaration_list_em '}' //////////////?????????
                     ;


namespace_member_declaration_list_em: /* empty */ { $$ = NULL; }
                                    | namespace_member_declaration_list { $$ = $1; }
                                    ;


namespace_member_declaration_list: namespace_member_declaration { $$ = $1; }
                                 | namespace_member_declaration_list namespace_member_declaration { $$ = NamespaceMemberList::Append($1,$2); }
                                 ;


namespace_member_declaration: namespace_declaration { $$ = $1; }
                            | class_declaration { $$ = $1; }
                            ;


class_declaration: modifier_list_em CLASS ID '{' class_member_declaration_list_em '}' { $$ = ClassDeclaration($1,ClassMember::t_CLASS, $3, $5); }/////////////???????????
                 | modifier_list_em CLASS ID ':' type_name '{' class_member_declaration_list_em '}'////////////////??????????
                 ;


class_member_declaration_list_em: /* empty */ { $$ = NULL; }
                                | class_member_declaration_list { $$ = $1; }
                                ;


class_member_declaration_list: class_member_declaration { $$ = $1; }
                             | class_member_declaration_list class_member_declaration { $$ = ClassMemberList::Append($1,$2); }
                             ;


class_member_declaration: field_declaration { $$ = $1; }
                        | method_declaration { $$ = $1; }
                        | constructor_declaration { $$ = $1; }
                        | class_declaration { $$ = $1; }
                        ;


constructor_declaration: modifier_list_em ID '(' param_list_em ')' ';' { $$ = Constructor($1,$2,$4,ClassMember::t_NULL); }
                       | modifier_list_em ID '(' param_list_em ')' '{' stmt_list_em '}' { $$ = Constructor($1,$2,$4,ClassMember::t_NULL, $7); }
                       | modifier_list_em ID '(' param_list_em ')' ':' BASE '(' argm_list_em ')' ';' { $$ = Constructor($1,$2,$4,ClassMember::t_BASE,$9); }
                       | modifier_list_em ID '(' param_list_em ')' ':' BASE '(' argm_list_em ')' '{' stmt_list_em '}' { $$ = Constructor($1,$2,$4,ClassMember::t_BASE,$9, $12); }
                       | modifier_list_em ID '(' param_list_em ')' ':' THIS '(' argm_list_em ')' ';' { $$ = Constructor($1,$2,$4,ClassMember::t_THIS,$9); }
                       | modifier_list_em ID '(' param_list_em ')' ':' THIS '(' argm_list_em ')' '{' stmt_list_em '}' { $$ = Constructor($1,$2,$4,ClassMember::t_THIS,$9, $12); }
                       ;


field_declaration: modifier_list_em type ID ';' { $$ = Field($1,ClassMember::t_SIMPLE_TYPE,$3); }
                 | modifier_list_em type ID '=' expr ';' { $$ = Field($1,ClassMember::t_SIMPLE_TYPE,$3,$5); }
                 | modifier_list_em type_name ID ';' { $$ = Field($1,ClassMember::t_TYPENAME,$3); }
                 | modifier_list_em type_name ID '=' expr ';' { $$ = Field($1,ClassMember::t_TYPENAME,$3,$5); }
                 | modifier_list_em array_type ID ';' { $$ = Field($1,ClassMember::t_ARRAY,$3); }
                 | modifier_list_em array_type ID '=' expr ';' { $$ = Field($1,ClassMember::t_ARRAY,$3,$5); }
                 ;


method_declaration: modifier_list_em type ID '(' param_list_em ')' '{' stmt_list_em '}' { $$ = Method($1,ClassMember::t_SIMPLE_TYPE,$3,$5,$8); }
                  | modifier_list_em type ID '(' param_list_em ')' ';' { $$ = Method($1,ClassMember::t_SIMPLE_TYPE,$3,$5); }
                  | modifier_list_em type_name ID '(' param_list_em ')' '{' stmt_list_em '}' { $$ = Method($1,ClassMember::t_TYPENAME,$3,$5,$8); }
                  | modifier_list_em type_name ID '(' param_list_em ')' ';' { $$ = Method($1,ClassMember::t_TYPENAME,$3,$5); }
                  | modifier_list_em VOID ID '(' param_list_em ')' '{' stmt_list_em '}' { $$ = Method($1,ClassMember::t_VOID,$3,$5,$8); }
                  | modifier_list_em VOID ID '(' param_list_em ')' ';' { $$ = Method($1,ClassMember::t_VOID,$3,$5); }
                  | modifier_list_em array_type ID '(' param_list_em ')' '{' stmt_list_em '}' { $$ = Method($1,ClassMember::t_ARRAY,$3,$5,$8); }
                  | modifier_list_em array_type ID '(' param_list_em ')' ';' { $$ = Method($1,ClassMember::t_ARRAY,$3,$5); }
                  ;


modifier_list_em: /* empty*/ { $$ = NULL; }
                | modifier_list { $$ = $1; }
                ;


modifier_list: modifier { $$ = $1; }
             | modifier_list modifier { $$ = ModifielrList::Append($1,$2); }
             ;


modifier: PRIVATE { $$ = new Modifier(Modifier::t_PRIVATE); }
        | PROTECTED { $$ = new Modifier(Modifier::t_PROTECTED); }
        | PUBLIC { $$ = new Modifier(Modifier::t_PUBLIC); }
        | INTERNAL { $$ = new Modifier(Modifier::t_INTERNAL); }
        | ABSTRACT { $$ = new Modifier(Modifier::t_ABSTRACT); }
        | STATIC { $$ = new Modifier(Modifier::t_STATIC); }
        | OVERRIDE { $$ = new Modifier(Modifier::t_OVERRIDE); }
        | VIRTUAL { $$ = new Modifier(Modifier::t_VIRTUAL); }
        ;


param_list_em: /* empty */ { $$ = NULL; }
             | param_list { $$ = $1; }
             ;


param_list: var_declarator { $$ = new ParamList($1); }
          | param_list ',' var_declarator { $$ = ParamList::Append($1,$3); }
          ;


stmt: ';' { $$ = Statement(Statement::t_EMPTY); }
    | expr ';' { $$ = Statement(Statement::t_EXPRESSION); }
    | var_declarator_list ';' { $$ = Statement(Statement::t_DECLARATOR); }
    | if_stmt { $$ = Statement(Statement::t_IF); }
    | while_stmt { $$ = Statement(Statement::t_WHILE); }
    | do_stmt { $$ = Statement(Statement::t_DO); }
    | for_stmt { $$ = Statement(Statement::t_FOR); }
    | foreach_stmt { $$ = Statement(Statement::t_FOREACH); }
    | return_stmt { $$ = Statement(Statement::t_RETURN); }
    | '{' stmt_list_em '}' { $$ = Statement(Statement::t_BLOCK); }
    ;


stmt_list_em: /* empty*/ { $$ = NULL; }
            | stmt_list { $$ = $1; }
            ;


stmt_list: stmt { $$ = $1; }
         | stmt_list stmt { $$ =  StatementList::Append($1,$2); }
         ;


return_stmt: RETURN ';'
           | RETURN expr ';'
           ; 


for_stmt: FOR '(' for_expr ';' for_expr ';' for_expr ')' stmt
        | FOR '(' var_declarator_list ';' for_expr ';' for_expr ')' stmt
    	;


for_expr: /*empty*/ { $$ = NULL; }
	    | expr { $$ = $1; }
	    ;


foreach_stmt: FOREACH '(' var_declarator IN expr ')' stmt
	        ;


do_stmt: DO stmt WHILE '(' expr ')' ';'
       ;


while_stmt: WHILE '(' expr ')' stmt
          ;


if_stmt: IF '(' expr ')' stmt %prec THEN
       | IF '(' expr ')' stmt ELSE stmt
       ;


var_declarator_list: var_declarator { $$ = $1; }
                   | var_declarator '=' expr { $$ = VarDeclaratorList(); }
                   | var_declarator_list ',' ID { $$ = VarDeclaratorList($1,$3); }
                   | var_declarator_list ',' ID '=' expr { $$ = VarDeclaratorList($1,$3,$5); }
                   ;


var_declarator: type ID { $$ = new VarDeclarator($1, $2); }
              | type_name ID { $$ = new VarDeclarator($1, $2); }
              | array_type ID { $$ = new VarDeclarator($1, $2); }
              ;


expr: INT_LITERAL { $$ = new Expression(Expression::t_INT_LITER); }
    | CHAR_LITERAL { $$ = new Expression(Expression::t_CHAR_LITER); }
    | STRING_LITERAL { $$ = new Expression(Expression::t_STRING_LITER); }
    | BOOLEAN_LITERAL { $$ = new Expression(Expression::t_BOOL_LITER); }
    | '(' expr ')' { $$ = new Expression(Expression::t_PARENTHESIZED, $2); }
    | member_access { $$ = $1; }
    | invocation_expression { $$ = $1; }
    | obj_creation_expr { $$ = $1; }
    | array_creation_expr { $$ = $1; }
    | element_access { $$ = $1; }
    | '-' expr %prec UNMINUS { $$ = new Expression(Expression::t_UNMINUS, $2); }  
    | '!' expr { $$ = new Expression(Expression::t_NOT, $2); }
    | '(' type ')' expr { $$ = new Expression(Expression::t_SIMPLE_TYPE_CAST, $2, $4); }
    | '(' array_type ')' expr { $$ = new Expression(Expression::t_ARRAY_CAST, $2, $4); }
    | '(' expr ')' expr { $$ = new Expression(Expression::t_TYPENAME_CAST, $2, $4); }
    | expr '*' expr { $$ = new Expression(Expression::t_MUL, $1, $3); }
    | expr '/' expr { $$ = new Expression(Expression::t_DIV, $1, $3); }
    | expr '%' expr	{ $$ = new Expression(Expression::t_MOD, $1, $3); }
    | expr '+' expr { $$ = new Expression(Expression::t_SUM, $1, $3); }
    | expr '-' expr { $$ = new Expression(Expression::t_SUB, $1, $3); }
    | expr '<' expr { $$ = new Expression(Expression::t_LESS, $1, $3); }
    | expr '>' expr	{ $$ = new Expression(Expression::t_GREATER, $1, $3); }
    | expr LESS_EQUAL expr { $$ = new Expression(Expression::t_LESS_EQUAL, $1, $3); }
    | expr GREATER_EQUAL expr { $$ = new Expression(Expression::t_GREATER_EQUAL, $1, $3); }
    | expr IS type { $$ = new Expression(Expression::t_IS, $3, $1); }
    | expr IS type_name { $$ = new Expression(Expression::t_IS, $3, $1); }
    | expr IS array_type { $$ = new Expression(Expression::t_IS, $3, $1); }
    | expr AS type { $$ = new Expression(Expression::t_AS, $3, $1); }
    | expr AS type_name { $$ = new Expression(Expression::t_AS, $3, $1); }
    | expr AS array_type { $$ = new Expression(Expression::t_AS, $3, $1); }
    | expr EQUALITY expr { $$ = new Expression(Expression::t_EQUALITY, $1 ,$3); }
    | expr INEQUALITY expr { $$ = new Expression(Expression::t_INEQUALITY, $1 ,$3); }
    | expr AND expr { $$ = new Expression(Expression::t_AND, $1, $3); }
    | expr OR expr { $$ = new Expression(Expression::t_OR, $1, $3); }
    | expr '=' expr { $$ = new Expression(Expression::t_IS, $1, $3); }
    ;


member_access: type_name { $$ = MemberAccess::FromTypeName($1); }
             | type '.' type_name { $$ = MemberAccess::FromTypeName($3, new Expression(Expression::t_SIMPLE_TYPE, $1)); }
             | THIS { $$ = new Expression(Expression::t_THIS); }
             | THIS '.' type_name { $$ = MemberAccess::FromTypeName($3, new Expression(Expression::t_THIS)); }
             | BASE '.' type_name { $$ = MemberAccess::FromTypeName($3, new Expression(Expression::t_BASE)); }
             | invocation_expression '.' type_name { $$ = MemberAccess::FromTypeName($3, $1); }
             | '(' expr ')' '.' type_name { $$ = MemberAccess::FromTypeName($5, new Expression(Expression::t_PARENTHESIZED, $2)); }
             | obj_creation_expr '.' type_name { $$ = MemberAccess::FromTypeName($3, $1); }
             | array_creation_expr '.' type_name { $$ = MemberAccess::FromTypeName($3, $1); }
             | element_access '.' type_name { $$ = MemberAccess::FromTypeName($3, $1); }
             ;


invocation_expression: member_access '(' argm_list_em ')' { $$ = new InvocationExpression($1, $3); }
                     ;


element_access: type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($1), $3); }
              | type '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, new Expression(Expression::t_SIMPLE_TYPE, $1)), $5); }
              | THIS '[' argm_list ']' { $$ = new ElementAccess(new Expression(Expression::t_THIS), $3); }
              | THIS '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, new Expression(Expression::t_THIS)), $5); }
              | BASE '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, new Expression(Expression::t_BASE)), $5); }
              | invocation_expression '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, $1), $5); }
              | '(' expr ')' '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($5, new Expression(Expression::t_PARENTHESIZED, $2)), $7); }
              | obj_creation_expr '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, $1), $5); }
              | array_creation_expr '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, $1), $5); }
              | element_access '.' type_name '[' argm_list ']' { $$ = new ElementAccess(MemberAccess::FromTypeName($3, $1), $5); }
              | obj_creation_expr '[' argm_list ']' { $$ = new ElementAccess($1, $3); }
              | invocation_expression '[' argm_list ']' { $$ = new ElementAccess($1, $3); }
              | element_access '[' argm_list ']' { $$ = new ElementAccess($1, $3); }
              | '(' expr ')' '[' argm_list ']' { $$ = new ElementAccess(new Expression(Expression::t_PARENTHESIZED, $2), $5); }
              ;


array_creation_expr: NEW array_type { $$ = new ArrayCreation($2); }
                   | NEW array_type array_initializer { $$ = new ArrayCreation($2,$3); }
                   | NEW type '[' expr ']' { $$ = new ArrayCreation($2,$4); }
                   | NEW type '[' expr ']' array_initializer { $$ = new ArrayCreation($2,$4,$6); }
                   | NEW type_name '[' expr ']' { $$ = new ArrayCreation($2, $4); }
                   | NEW type_name '[' expr ']' array_initializer { $$ = new ArrayCreation($2,$4,$6); }
                   ;


array_initializer: '{' expr_list_em '}' { $$ = new ArrayInitializer($2); }
                 | '{' expr_list ',' '}' { $$ = new ArrayInitializer($2); }
                 ;


expr_list: expr { $$ = new ExpressionList($1); }
         | expr_list ',' expr { ExpressionList::Append($1, $3); }
         ;


expr_list_em: /* empty*/ { $$ = NULL; }
            | expr_list { $$ = $1; }
            ;


obj_creation_expr: NEW type '(' argm_list_em ')' { $$ = new ObjectCreation($2,$4); }
                 | NEW type '(' argm_list_em ')' obj_initializer { $$ = new ObjectCreation($2,$4,$6); }
                 | NEW type obj_initializer { $$ = new ObjectCreation($2,NULL,$3); }
                 | NEW type_name '(' argm_list_em ')' { $$ = new ObjectCreation($2,$4); }
                 | NEW type_name '(' argm_list_em ')' obj_initializer { $$ = new ObjectCreation($2,$4,$6); }
                 | NEW type_name obj_initializer { $$ = new ObjectCreation($2,NULL,$3); }
                 ;
                 

obj_initializer: '{' member_initializer_list_em '}' { $$ = new ObjectInitializer($2); }
               | '{' member_initializer_list ',' '}' { $$ = new ObjectInitializer($2); }
               ;


member_initializer_list_em: /* empty */ { $$ = NULL; }
                          | member_initializer_list { $$ = $1; }
                          ;


member_initializer_list: member_initializer { $$ = new MemberInitializerList($1); }
                       | member_initializer_list ',' member_initializer { MemberInitializerList::Append($1, $3); }
                       ;


member_initializer: ID '=' expr { $$ = new MemberInitializer($1,$3); }
                  | '[' argm_list ']' '=' expr { $$ = new MemberInitializer($2,$5); }
                  | ID '=' obj_initializer { $$ = new MemberInitializer($1,$3); }
                  | '[' argm_list ']' '=' obj_initializer { $$ = new MemberInitializer($2,$5); }
                  ;


argm_list_em: /* empty */ { $$ = NULL; }
            | argm_list { $$ = $1;}
            ;


argm_list: argm { $$ = new ArgumentList($1);}
         | argm_list ',' argm { ArgumentList::Append($1,$3);}
         ;


argm: expr { $$ = new Argument($1); }
    | ID ':' expr { $$ = new Argument($3,$1); }
    ;


array_type: type '[' ']' { $$ = new ArrayType($1); }
          | type_name '[' ']' { $$ = new ArrayType($1); }
          ;


type: INT { $$ = new SimpleType(SimpleType::t_INT);}
    | CHAR { $$ = new SimpleType(SimpleType::t_CHAR);}
    | STRING { $$ = new SimpleType(SimpleType::t_STRING);}
    | BOOL { $$ = new SimpleType(SimpleType::t_BOOL);}
    ;


type_name: ID { $$ = new TypeName($1); }
         | type_name '.' ID { TypeName::Append($1,$3); }
         ;



%%

void yyerror(char const* s)
{
    cout << s << endl;
}
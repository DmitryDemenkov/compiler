%{
#include <iostream>
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
	SimpleType* SimpleType;
	TypeName* TypeName;
	ArrayType* ArrayType;
	Argument* Argument;
	ArgumentList* ArgumentList;
	ObjectInitializer* ObjectInitializer;
	MemberInitializer* MemberInitializer;
	MemberInitializerList* MemberInitializerList;
	Expression* Expression;
	ObjectCreation* ObjectCreation;
	ExpressionList* ExpressionList;
	ArrayInitializer* ArrayInitializer;
	ArrayCreation* ArrayCreation;
	MemberAccess* MemberAccess;
	ElementAccess* ElementAccess;
	InvocationExpression* InvocationExpression;
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

%token INT_LITERAL
%token CHAR_LITERAL
%token STRING_LITERAL
%token BOOLEAN_LITERAL

%token ID

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

%%

program: namespace_member_declaration_list_em
       ;


namespace_declaration: NAMESPACE type_name '{' namespace_member_declaration_list_em '}'
                     ;


namespace_member_declaration_list_em: /* empty */
                                    | namespace_member_declaration_list
                                    ;


namespace_member_declaration_list: namespace_member_declaration
                                 | namespace_member_declaration_list namespace_member_declaration
                                 ;


namespace_member_declaration: namespace_declaration
                            | class_declaration
                            ;


class_declaration: modifier_list_em CLASS ID '{' class_member_declaration_list_em '}'
                 | modifier_list_em CLASS ID ':' type_name '{' class_member_declaration_list_em '}'
                 ;


class_member_declaration_list_em: /* empty */ { $$ = null; }
                                | class_member_declaration_list
                                ;


class_member_declaration_list: class_member_declaration
                             | class_member_declaration_list class_member_declaration
                             ;


class_member_declaration: field_declaration
                        | method_declaration
                        | constructor_declaration
                        | class_declaration
                        ;


constructor_declaration: modifier_list_em ID '(' param_list_em ')' ';'
                       | modifier_list_em ID '(' param_list_em ')' '{' stmt_list_em '}'
                       | modifier_list_em ID '(' param_list_em ')' ':' BASE '(' argm_list_em ')' ';'
                       | modifier_list_em ID '(' param_list_em ')' ':' BASE '(' argm_list_em ')' '{' stmt_list_em '}'
                       | modifier_list_em ID '(' param_list_em ')' ':' THIS '(' argm_list_em ')' ';'
                       | modifier_list_em ID '(' param_list_em ')' ':' THIS '(' argm_list_em ')' '{' stmt_list_em '}'
                       ;


field_declaration: modifier_list_em type ID ';'
                 | modifier_list_em type ID '=' expr ';'
                 | modifier_list_em type_name ID ';'
                 | modifier_list_em type_name ID '=' expr ';'
                 | modifier_list_em array_type ID ';'
                 | modifier_list_em array_type ID '=' expr ';'
                 ;


method_declaration: modifier_list_em type ID '(' param_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em type ID '(' param_list_em ')' ';'
                  | modifier_list_em type_name ID '(' param_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em type_name ID '(' param_list_em ')' ';'
                  | modifier_list_em VOID ID '(' param_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em VOID ID '(' param_list_em ')' ';'
                  | modifier_list_em array_type ID '(' param_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em array_type ID '(' param_list_em ')' ';'
                  ;


modifier_list_em: /* empty*/ { $$ = null; }
                | modifier_list
                ;


modifier_list: modifier
             | modifier_list modifier
             ;


modifier: PRIVATE
        | PROTECTED
        | PUBLIC
        | INTERNAL
        | ABSTRACT
        | STATIC
        | OVERRIDE
        | VIRTUAL
        ;


param_list_em: /* empty */ { $$ = null; }
             | param_list
             ;


param_list: var_declarator
          | param_list ',' var_declarator
          ;


stmt: ';'
    | expr ';'
    | var_declarator_list ';'
    | if_stmt
    | while_stmt
    | do_stmt
    | for_stmt
    | foreach_stmt
    | return_stmt
    | '{' stmt_list_em '}'
    ;


stmt_list_em: /* empty*/ { $$ = null; }
            | stmt_list
            ;


stmt_list: stmt
         | stmt_list stmt
         ;


return_stmt: RETURN ';'
           | RETURN expr ';'
           ; 


for_stmt: FOR '(' for_expr ';' for_expr ';' for_expr ')' stmt
        | FOR '(' var_declarator_list ';' for_expr ';' for_expr ')' stmt
    	;


for_expr: /*empty*/ { $$ = null; }
	    | expr
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


var_declarator_list: var_declarator
                   | var_declarator '=' expr
                   | var_declarator_list ',' ID
                   | var_declarator_list ',' ID '=' expr
                   ;


var_declarator: type ID 
              | type_name ID 
              | array_type ID 
              ;


expr: INT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | BOOLEAN_LITERAL
    | '(' expr ')' { $$ = $2; }
    | member_access
    | invocation_expression
    | obj_creation_expr
    | array_creation_expr
    | element_access;
    | '-' expr %prec UNMINUS
    | '!' expr
    | '(' type ')' expr
    | '(' array_type ')' expr
    | '(' expr ')' expr
    | expr '*' expr 
    | expr '/' expr 
    | expr '%' expr	
    | expr '+' expr 
    | expr '-' expr 
    | expr '<' expr 
    | expr '>' expr	
    | expr LESS_EQUAL expr 
    | expr GREATER_EQUAL expr 
    | expr IS type
    | expr IS type_name
    | expr IS array_type
    | expr AS type
    | expr AS type_name
    | expr AS array_type
    | expr EQUALITY expr
    | expr INEQUALITY expr
    | expr AND expr
    | expr OR expr
    | expr '=' expr 
    ;


member_access: type_name
             | type '.' type_name
             | THIS
             | THIS '.' type_name
             | BASE '.' type_name
             | invocation_expression '.' type_name
             | '(' expr ')' '.' type_name
             | obj_creation_expr '.' type_name
             | array_creation_expr '.' type_name
             | element_access '.' type_name
             ;


invocation_expression: member_access '(' argm_list_em ')'
                     ;


element_access: type_name '[' argm_list ']'
              | type '.' type_name '[' argm_list ']'
              | THIS '[' argm_list ']'
              | THIS '.' type_name '[' argm_list ']'
              | BASE '.' type_name '[' argm_list ']'
              | invocation_expression '.' type_name '[' argm_list ']'
              | '(' expr ')' '.' type_name '[' argm_list ']'
              | obj_creation_expr '.' type_name '[' argm_list ']'
              | array_creation_expr '.' type_name '[' argm_list ']'
              | element_access '.' type_name '[' argm_list ']'
              | obj_creation_expr '[' argm_list ']'
              | invocation_expression '[' argm_list ']'
              | element_access '[' argm_list ']'
              | '(' expr ')' '[' argm_list ']'
              ;


array_creation_expr: NEW array_type
                   | NEW array_type array_initializer
                   | NEW type '[' expr ']'
                   | NEW type '[' expr ']' array_initializer
                   | NEW type_name '[' expr ']'
                   | NEW type_name '[' expr ']' array_initializer
                   ;


array_initializer: '{' expr_list_em '}' 
                 | '{' expr_list ',' '}' { $$ = ArrayInitializer::ArrayInitializer(expression); }
                 ;


expr_list: expr { $$ = ExpressionList::ExpressionList(expression); }
         | expr_list ',' expr { $$ = ExpressionList::Append(list, expression); }
         ;


expr_list_em: /* empty*/ { $$ = null; }
            | expr_list { $$ = ExpressionList::ExpressionList(expression); }
            ;


obj_creation_expr: NEW type '(' argm_list_em ')'
                 | NEW type '(' argm_list_em ')' obj_initializer
                 | NEW type obj_initializer
                 | NEW type_name '(' argm_list_em ')'
                 | NEW type_name '(' argm_list_em ')' obj_initializer
                 | NEW type_name obj_initializer
                 ;
                 

obj_initializer: '{' member_initializer_list_em '}' { $$ = ObjectInitializer::ObjectInitializer(initializers); }
               | '{' member_initializer_list ',' '}' { $$ = ObjectInitializer::ObjectInitializer(initializers); }
               ;


member_initializer_list_em: /* empty */ { $$ = null; }
                          | member_initializer_list { $$ = MemberInitializerList::MemberInitializerList(memberInitializer); }
                          ;


member_initializer_list: member_initializer { $$ = MemberInitializerList::MemberInitializerList(memberInitializer); }
                       | member_initializer_list ',' member_initializer { $$ = MemberInitializerList::Append(list, memberInitializer); }
                       ;


member_initializer: ID '=' expr { $$ = MemberInitializer::MemberInitializer(identifier,expression); }
                  | '[' argm_list ']' '=' expr { $$ = MemberInitializer::MemberInitializer(argumentList,expression); }
                  | ID '=' obj_initializer { $$ = MemberInitializer::MemberInitializer(identifier,objectInitializer); }
                  | '[' argm_list ']' '=' obj_initializer { $$ = MemberInitializer::MemberInitializer(argumentList,objectInitializer); }
                  ;


argm_list_em: /* empty */ { $$ = null; }
            | argm_list { $$ = $1; }
            ;


argm_list: argm { $$ = $1; } { $$ = ArgumentList::ArgumentList(list, argument);}
         | argm_list ',' argm { $$ = ArgumentList::Append(list, argument);}
         ;


argm: expr { $$ = Argument::Argument(expression,identifier); }
    | ID ':' expr { $$ = Argument::Argument(expression,identifier); }
    ;


array_type: type '[' ']'
          | type_name '[' ']' { $$ = ArrayType::ArrayType(t_TYPE_NAME); }
          ;


type: INT { $$ = SimpleType::SimpleType(t_INT);}
    | CHAR { $$ = SimpleType::SimpleType(t_CHAR);}
    | STRING { $$ = SimpleType::SimpleType(t_STRING);}
    | BOOL { $$ = SimpleType::SimpleType(t_BOOL);}
    ;


type_name: ID { $$ = $1; }
         | type_name '.' ID { $$ = TypeName::TypeName(identifier); }
         ;



%%

void yyerror(char const* s)
{
    cout << s << endl;
}
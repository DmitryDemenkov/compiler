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
	ObjectCreation* ObjectCreation;
	MemberInitializer* MemberInitializer;
	MemberInitializerList* MemberInitializerList;
	Expression* Expression;
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


expr: INT_LITERAL { $$ = new Expression(t_INT_LITER); }
    | CHAR_LITERAL { $$ = new Expression(t_CHAR_LITER); }
    | STRING_LITERAL { $$ = new Expression(t_STRING_LITER); }
    | BOOLEAN_LITERAL { $$ = new Expression(t_BOOL_LITER); }
    | '(' expr ')' { $$ = Expression($2); }
    | member_access { $$ = new Expression(t_MEMBER_ACCESS); }
    | invocation_expression { $$ = Expression(t_INVOCATION); }
    | obj_creation_expr { $$ = new Expression(t_OBJ_CREATION); }
    | array_creation_expr { $$ = new Expression(t_ARR_CREATION); }
    | element_access { $$ = new Expression(t_ELEMENT_ACCESS); }
    | '-' expr %prec UNMINUS { $$ = Expression(t_UNMINUS, $2); }  
    | '!' expr { $$ = Expression(t_NOT, $2); }
    | '(' type ')' expr { $$ = Expression($2, $4); }
    | '(' array_type ')' expr { $$ = Expression($2, $4); }
    | '(' expr ')' expr { $$ = Expression($2, $4); }
    | expr '*' expr { $$ = Expression($1,t_MUL, $3); }
    | expr '/' expr { $$ = Expression($1,t_DIV, $3); }
    | expr '%' expr	{ $$ = Expression($1,t_MOD, $3); }
    | expr '+' expr { $$ = Expression($1,t_SUM, $3); }
    | expr '-' expr { $$ = Expression($1,t_SUB, $3); }
    | expr '<' expr { $$ = Expression($1,t_LESS, $3); }
    | expr '>' expr	{ $$ = Expression($1,t_GREATER, $3); }
    | expr LESS_EQUAL expr { $$ = Expression($1,t_LESS_EQUAL, $3); }
    | expr GREATER_EQUAL expr { $$ = Expression($1,t_GREATER_EQUAL, $3); }
    | expr IS type { $$ = Expression($1,t_IS,$3); }
    | expr IS type_name { $$ = Expression($1,t_IS,$3); }
    | expr IS array_type { $$ = Expression($1,t_IS,$3); }
    | expr AS type { $$ = Expression($1,t_AS,$3); }
    | expr AS type_name { $$ = Expression($1,t_AS,$3); }
    | expr AS array_type { $$ = Expression($1,t_AS,$3); }
    | expr EQUALITY expr { $$ = Expression($1,t_EQUALITY,$3); }
    | expr INEQUALITY expr { $$ = Expression($1,t_INEQUALITY,$3); }
    | expr AND expr { $$ = Expression($1,t_AND,$3); }
    | expr OR expr { $$ = Expression($1,t_OR,$3); }
    | expr '=' expr { $$ = Expression($1,t_IS,$3); }
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


array_creation_expr: NEW array_type { $$ = new ArrayType($1); }
                   | NEW array_type array_initializer { $$ = new ArrayCreation($2,$3); }
                   | NEW type '[' expr ']' { $$ = new ArrayCreation($2,$4); }
                   | NEW type '[' expr ']' array_initializer { $$ = new ArrayCreation($2,$4,$6); }
                   | NEW type_name '[' expr ']' { $$ = new ArrayCreation($2, $4); }
                   | NEW type_name '[' expr ']' array_initializer { $$ = new ArrayCreation($2,$4,$6); }
                   ;


array_initializer: '{' expr_list_em '}' { $$ = new ArrayInitializer($1); }
                 | '{' expr_list ',' '}' { $$ = ArrayInitializer($1); }
                 ;


expr_list: expr { $$ = new ExpressionList($1); }
         | expr_list ',' expr { $$ = ExpressionList::Append($1, $3); }
         ;


expr_list_em: /* empty*/ { $$ = null; }
            | expr_list { $$ = new ExpressionList($1); }
            ;


obj_creation_expr: NEW type '(' argm_list_em ')' { $$ = new ObjectCreation($2,$4); }
                 | NEW type '(' argm_list_em ')' obj_initializer { $$ = new ObjectCreation($2,$4,$6); }
                 | NEW type obj_initializer { $$ = new ObjectCreation($2,$3); }
                 | NEW type_name '(' argm_list_em ')' { $$ = new ObjectCreation($2,$4); }
                 | NEW type_name '(' argm_list_em ')' obj_initializer { $$ = new ObjectCreation($2,$4,$6); }
                 | NEW type_name obj_initializer { $$ = new ObjectCreation($2,$3); }
                 ;
                 

obj_initializer: '{' member_initializer_list_em '}' { $$ = new ObjectInitializer($1); }
               | '{' member_initializer_list ',' '}' { $$ = ObjectInitializer($1); }
               ;


member_initializer_list_em: /* empty */ { $$ = null; }
                          | member_initializer_list { $$ = new MemberInitializerList($1); }
                          ;


member_initializer_list: member_initializer { $$ = new MemberInitializerList($1); }
                       | member_initializer_list ',' member_initializer { $$ = MemberInitializerList::Append($1, $3); }
                       ;


member_initializer: ID '=' expr { $$ = MemberInitializer($1,$3); }
                  | '[' argm_list ']' '=' expr { $$ = MemberInitializer($2,$5); }
                  | ID '=' obj_initializer { $$ = MemberInitializer($1,$3); }
                  | '[' argm_list ']' '=' obj_initializer { $$ = MemberInitializer($2,$5); }
                  ;


argm_list_em: /* empty */ { $$ = null; }
            | argm_list { $$ = new ArgumentList($1);}
            ;


argm_list: argm { $$ = new ArgumentList($1);}
         | argm_list ',' argm { $$ = ArgumentList::Append($1,$3);}
         ;


argm: expr { $$ = new Argument($1); }
    | ID ':' expr { $$ = Argument($1,$3); }
    ;


array_type: type '[' ']' { $$ = new ArrayType($1); }
          | type_name '[' ']' { $$ = new ArrayType(t_TYPE_NAME); }
          ;


type: INT { $$ = new SimpleType(t_INT);}
    | CHAR { $$ = new SimpleType(t_CHAR);}
    | STRING { $$ = new SimpleType(t_STRING);}
    | BOOL { $$ = new SimpleType(t_BOOL);}
    ;


type_name: ID { $$ = new TypeName($1); }
         | type_name '.' ID { $$ = TypeName::Append($1,$3); }
         ;



%%

void yyerror(char const* s)
{
    cout << s << endl;
}
%{
/* Prologue */
%}

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
%token ELSE
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

%%

class_declaration: modifier_list_em CLASS ID '{' class_member_declaration_list_em '}'
                 | modifier_list_em CLASS ID ':' type_name '{' class_member_declaration_list_em '}'
                 ;


class_member_declaration_list_em: /* empty */
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


constructor_declaration: modifier_list_em ID '(' expr_list_em ')' ';'
                       | modifier_list_em ID '(' expr_list_em ')' '{' stmt_list_em '}'
                       | modifier_list_em ID '(' expr_list_em ')' ':' BASE '(' argm_list_em ')' ';'
                       | modifier_list_em ID '(' expr_list_em ')' ':' BASE '(' argm_list_em ')' '{' stmt_list_em '}'
                       | modifier_list_em ID '(' expr_list_em ')' ':' THIS '(' argm_list_em ')' ';'
                       | modifier_list_em ID '(' expr_list_em ')' ':' THIS '(' argm_list_em ')' '{' stmt_list_em '}'
                       ;


field_declaration: modifier_list_em type ID ';'
                 | modifier_list_em type ID '=' expr ';'
                 | modifier_list_em type_name ID ';'
                 | modifier_list_em type_name ID '=' expr ';'
                 | modifier_list_em array_type ID ';'
                 | modifier_list_em array_type ID '=' expr ';'
                 ;


method_declaration: modifier_list_em type ID '(' expr_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em type ID '(' expr_list_em ')' ';'
                  | modifier_list_em type_name ID '(' expr_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em type_name ID '(' expr_list_em ')' ';'
                  | modifier_list_em VOID ID '(' expr_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em VOID ID '(' expr_list_em ')' ';'
                  | modifier_list_em array_type ID '(' expr_list_em ')' '{' stmt_list_em '}'
                  | modifier_list_em array_type ID '(' expr_list_em ')' ';'
                  ;


modifier_list_em: /* empty*/
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


stmt: ';'
    | expr ';'
    | if_stmt
    | while_stmt
    | return_stmt
    | '{' stmt_list_em '}'
    ;


stmt_list_em: /* empty*/
            | stmt_list
            ;


stmt_list: stmt
         | stmt_list stmt
         ;


return_stmt: RETURN ';'
           | RETURN expr ';'
           ; 


while_stmt: WHILE '(' expr ')' stmt
          ;


if_stmt: IF '(' expr ')' stmt
       | IF '(' expr ')' stmt ELSE stmt
       ;


expr: INT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | BOOLEAN_LITERAL
    | '(' expr ')'
    | type '.' ID
    | type_name
    | type_name '[' argm_list ']'
    | expr '(' argm_list_em ')'
    | expr '(' argm_list_em ')' '[' argm_list ']'
    | '(' expr ')' '[' argm_list ']'
    | THIS
    | BASE '.' ID
    | BASE '[' argm_list ']'
    | obj_creation_expr
    | array_creation_expr
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
    | type ID
    | type_name ID
    | array_type ID
    | expr '=' expr
    ;


array_creation_expr: NEW array_type
                   | NEW array_type array_initializer
                   | NEW type '[' expr ']'
                   | NEW type '[' expr ']' array_initializer
                   | NEW type_name '[' expr ']'
                   | NEW type_name '[' expr ']' array_initializer
                   ;


array_initializer: '{' expr_list_em '}'
                 | '{' expr_list ',' '}'
                 ;


expr_list: expr
         | expr_list ',' expr
         ;


expr_list_em: /* empty*/
            | expr_list
            ;


obj_creation_expr: NEW type '(' argm_list_em ')'
                 | NEW type '(' argm_list_em ')' obj_initializer
                 | NEW type obj_initializer
                 | NEW type_name '(' argm_list_em ')'
                 | NEW type_name '(' argm_list_em ')' obj_initializer
                 | NEW type_name obj_initializer
                 ;
                 

obj_initializer: '{' member_initializer_list_em '}'
               | '{' member_initializer_list ',' '}'
               ;


member_initializer_list_em: /* empty */
                          | member_initializer_list
                          ;


member_initializer_list: member_initializer
                       | member_initializer_list ',' member_initializer
                       ;


member_initializer: ID '=' expr
                  | '[' argm_list ']' '=' expr
                  | ID '=' obj_initializer
                  | '[' argm_list ']' '=' obj_initializer
                  ;


argm_list_em: /* empty */
            | argm_list
            ;


argm_list: argm
         | argm_list ',' argm
         ;


argm: expr
    | ID ':' expr
    ;


array_type: type '[' ']'
          | type_name '[' ']'
          ;


type: INT
    | CHAR
    | STRING
    | BOOL
    ;


type_name: ID
         | type_name '.' ID
         ;



%%
/* Code */
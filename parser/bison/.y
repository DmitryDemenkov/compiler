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

expr: INT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | BOOLEAN_LITERAL
    | '(' expr ')'
    | type '.' ID
    | type_name
    | expr '(' argm_list_em ')'
    | expr '[' argm_list ']'
    | THIS
    | BASE '.' ID
    | BASE '[' argm_list ']'
    | obj_creation_expr
    | array_creation_expr
    | '-' expr %prec UNMINUS
    | '!' expr
    | '(' type ')' expr
    | '(' type '[' ']' ')' expr
    | '(' expr ')' expr         /* '(' type_name ')' expr */
    | '(' expr '[' ']' ')' expr /* '(' type_name '[' ']' ')' expr */
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
    | expr IS type '[' ']'
    | expr AS type
    | expr AS type '[' ']'
    | expr IS type_name
    | expr IS type_name '[' ']'
    | expr AS type_name
    | expr AS type_name '[' ']'
    | expr EQUALITY expr
    | expr INEQUALITY expr
    | expr AND expr
    | expr OR expr
    | type ID
    | type '[' ']' ID
    | type_name ID
    | expr '[' ']' ID  /* type_name '[' ']' ID */
    | expr '=' expr
    ;


array_creation_expr: NEW type '[' ']'
                   | NEW type '[' expr ']'
                   | NEW type '[' ']' array_initializer
                   | NEW type '[' expr ']' array_initializer
                   | NEW type_name '[' ']'
                   | NEW type_name '[' expr ']'
                   | NEW type_name '[' ']' array_initializer
                   | NEW type_name '[' expr ']' array_initializer
                   ;


array_initializer: '{' expr_list_em '}'
                 | '{' expr_list ',' '}'


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


member_initializer_list: member_initializer
                       | member_initializer_list ',' member_initializer
                       ;


member_initializer: ID '=' expr
                  | '[' argm_list ']' '=' expr
                  | ID '=' obj_initializer
                  | '[' argm_list ']' '=' obj_initializer


argm_list_em: /* empty */
            | argm_list
            ;


argm_list: argm
         | argm_list ',' argm
         ;


argm: expr
    | ID ':' expr
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
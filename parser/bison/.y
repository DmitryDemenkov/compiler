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
/* Rules */
%%
/* Code */
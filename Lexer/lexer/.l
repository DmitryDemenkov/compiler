%{
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
%}

%option noyywrap
%option never-interactive

%x STRING
%x CHAR

DIGIT     [0-9]
ID        [A-Za-z_][A-Za-z0-9_]*
HEX_DIGIT [0-9A-Fa-f]
BIN_DIGIT [0-1]

%{
void delete_underlines(char* str, char* newStr);
%}

%%

%{
int lineCounter = 0;
char literal[4096] = ""; 
char buffer[1024] = "";
char digit[1024] = "";
%}

abstract  { printf( "keyword: abstract\n" ); }
virtual   { printf( "keyword: virtual\n" ); }
override  { printf( "keyword: override\n"); }

base      { printf( "keyword: base\n" ); }
this      { printf( "keyword: this\n" ); }
class     { printf( "keyword: class\n" ); }
namespace { printf( "keyword: namespace\n" ); }
static    { printf( "keyword: static\n" ); }
return    { printf( "keyword: return\n" ); }

new       { printf( "keyword: new\n" ); }
as        { printf( "keyword: as\n" ); }
is        { printf( "keyword: is\n" ); }

public    { printf( "keyword: else\n" ); }
internal  { printf( "keyword: else\n" ); }
protected { printf( "keyword: else\n" ); }
private   { printf( "keyword: else\n" ); }

do        { printf( "keyword: do\n" ); }
if        { printf( "keyword: if\n"); }
else      { printf( "keyword: else\n" ); }
for       { printf( "keyword: for\n" ); }
foreach   { printf( "keyword: foreach\n" ); }
while     { printf( "keyword: while\n" ); }

void      { printf( "keyword: void\n" ); }
int       { printf( "keyword: int\n" ); }
char      { printf( "keyword: char\n" ); }
string    { printf( "keyword: string\n" ); }


\@?{ID} { 
    if (yytext[0] == '@') { yytext++; }
    printf( "identifier: %s\n", yytext ); 
}


"{"  { printf( "curly_bracket_open\n" ); }
"}"  { printf( "curly_bracket_close\n" ); }
"["  { printf( "square_brackets_open\n" ); }
"]"  { printf( "square_brackets_close\n" ); }
"("  { printf( "round_bracket_open\n" ); }
")"  { printf( "round_bracket_close\n" ); }

"."  { printf( "dot\n" ); }
","  { printf( "comma\n" ); }
":"  { printf( "colon\n" ); }
";"  { printf( "semicolon\n" ); }

"+"  { printf( "plus\n" ); }
"-"  { printf( "minus\n" ); }
"++" { printf( "increment\n" ); }
"--" { printf( "decrement\n" ); }
"*"  { printf( "asterisk\n" ); }
"/"  { printf( "slash\n" ); }
"%"  { printf( "percent\n" ); }

"<"  { printf( "less\n" ); }
">"  { printf( "greater\n" ); }
"<=" { printf( "less_equal\n" ); }
">=" { printf( "greater_equal\n" ); }
"==" { printf( "equality\n" ); }
"!=" { printf( "inequality\n" ); }

"="  { printf( "assignment\n" ); }
"+=" { printf( "assignment_increment\n" ); }
"-=" { printf( "assignment_decrement\n" ); }
"*=" { printf( "assignment_multiply\n" ); }
"/=" { printf( "assignment_division\n" ); }
"%=" { printf( "assignment_module\n" ); }


{DIGIT}(_*{DIGIT})* {
    delete_underlines(yytext, digit);
    printf( "integer_literal: %d\n", atoi(digit) ); 
}

(0x|0X)(_*{HEX_DIGIT})+ {
    yytext+=2;
    delete_underlines(yytext, digit);
    char* a;
    printf( "hexadecimal_literal %d\n", strtol(digit, &a, 16) );
}

(0b|0B)(_*{BIN_DIGIT})+ {
    yytext+=2;
    delete_underlines(yytext, digit);
    char* a;
    printf( "binary_literal %d\n", strtol(digit, &a, 2) );
}

\' { strcpy(literal, ""); BEGIN(CHAR); }
<CHAR>[^\'\\\r\n]+ { strcat(literal, yytext); }
<CHAR>[\r\n] { printf("ERROR line#%d: incorrect symbol in char literal\n", lineCounter+1); }
<CHAR>\' {
    if (strlen(literal) == 0)
        printf("ERROR line#%d: empty char literal\n", lineCounter+1);
    else if (strlen(literal) == 1) 
        printf("char_literal: %c\n", literal[0]);
    else
        printf("ERROR line#%d: too long char literal\n", lineCounter+1); 
    BEGIN(INITIAL); 
}
<CHAR><<EOF>> { printf("ERROR line#%d: no closed single quote\n", lineCounter+1); BEGIN(INITIAL); }

\" { strcpy(literal, ""); BEGIN(STRING); }
<STRING>[^\"\\\r\n]+ { strcat(literal, yytext); }
<STRING>[\r\n] { printf("ERROR line#%d: incorrect symbol in string literal\n", lineCounter+1); }
<STRING>\" { printf("string_literal: %s\n", literal); BEGIN(INITIAL); }
<STRING><<EOF>> { printf("ERROR line#%d: no closed double quote\n", lineCounter+1); BEGIN(INITIAL); }

<STRING,CHAR>\\\'  { strcat(literal, "\'");   }
<STRING,CHAR>\\\"  { strcat(literal, "\"");   }
<STRING,CHAR>\\\\  { strcat(literal, "\\");   }
<STRING,CHAR>\\0   { strcat(literal, "\0");   }
<STRING,CHAR>\\a   { strcat(literal, "\a");   }
<STRING,CHAR>\\b   { strcat(literal, "\b");   }
<STRING,CHAR>\\f   { strcat(literal, "\f");   }
<STRING,CHAR>\\n   { strcat(literal, "\n");   }
<STRING,CHAR>\\r   { strcat(literal, "\r");   }
<STRING,CHAR>\\t   { strcat(literal, "\t");   }
<STRING,CHAR>\\v   { strcat(literal, "\v");   }

\@\"[^\"]*\" { 
    yytext+=2;
    yytext[strlen(yytext)-1]='\0';
    printf("string_literal: %s\n", yytext); 
}


[\r\n] { lineCounter++; }

[ \t]+ { /* eat up whitespace */ }

%%

void delete_underlines(char* str, char* newStr)
{
    newStr[0] = 0;
    int j = 0;
    for(int i = 0; str[i] != '\0'; i++) {
        if (str[i] != '_') {
            newStr[j] = str[i]; j++;
        }
    }
    newStr[j] = '\0';
}

void main( int argc, char** argv )
{
    ++argv, --argc;
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    yylex();
}
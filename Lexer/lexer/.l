%{
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

using namespace std;
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
string literal = ""; 
char* buffer = 0;
char digit[1024] = "";
%}

abstract  { cout << "keyword: abstract" << endl; }
virtual   { cout << "keyword: virtual" << endl; }
override  { cout << "keyword: override" << endl; }

base      { cout << "keyword: base" << endl; }
this      { cout << "keyword: this" << endl; }
class     { cout << "keyword: class" << endl; }
namespace { cout << "keyword: namespace" << endl; }
static    { cout << "keyword: static" << endl; }
return    { cout << "keyword: return" << endl; }

new       { cout << "keyword: new" << endl; }
as        { cout << "keyword: as" << endl; }
is        { cout << "keyword: is" << endl; }

public    { cout << "keyword: public" << endl; }
internal  { cout << "keyword: internal" << endl; }
protected { cout << "keyword: protected" << endl; }
private   { cout << "keyword: private" << endl; }

do        { cout << "keyword: do" << endl; }
if        { cout << "keyword: if" << endl; }
else      { cout << "keyword: else" << endl; }
for       { cout << "keyword: for" << endl; }
foreach   { cout << "keyword: foreach" << endl; }
while     { cout << "keyword: while" << endl; }

void      { cout << "keyword: void" << endl; }
int       { cout << "keyword: int" << endl; }
char      { cout << "keyword: char" << endl; }
string    { cout << "keyword: string" << endl; }


\@?{ID} { 
    buffer = yytext;
    if (buffer[0] == '@') { buffer++; }
    cout << "identifier: " << buffer << endl;
}


"{"  { cout << "curly_bracket_open" << endl; }
"}"  { cout << "curly_bracket_close" << endl; }
"["  { cout << "square_brackets_open" << endl; }
"]"  { cout << "square_brackets_close" << endl; }
"("  { cout << "round_bracket_open" << endl; }
")"  { cout << "round_bracket_close" << endl; }

"."  { cout << "dot" << endl; }
","  { cout << "comma" << endl; }
":"  { cout << "colon" << endl; }
";"  { cout << "semicolon" << endl; }

"+"  { cout << "plus" << endl; }
"-"  { cout << "minus" << endl; }
"*"  { cout << "asterisk" << endl; }
"/"  { cout << "slash" << endl; }
"%"  { cout << "percent" << endl; }

"<"  { cout << "less" << endl; }
">"  { cout << "greater" << endl; }
"<=" { cout << "less_equal" << endl; }
">=" { cout << "greater_equal" << endl; }
"==" { cout << "equality" << endl; }
"!=" { cout << "inequality" << endl; }

"="  { cout << "assignment" << endl; }


{DIGIT}(_*{DIGIT})* {
    buffer = yytext;
    delete_underlines(buffer, digit);
    printf( "integer_literal: %d\n", atoi(digit) ); 
}

(0x|0X)(_*{HEX_DIGIT})+ {
    buffer = yytext + 2;
    delete_underlines(buffer, digit);
    char* a;
    printf( "hexadecimal_literal %d\n", strtol(digit, &a, 16) );
}

(0b|0B)(_*{BIN_DIGIT})+ {
    buffer = yytext + 2;
    delete_underlines(buffer, digit);
    char* a;
    printf( "binary_literal %d\n", strtol(digit, &a, 2) );
}

\' { literal = ""; BEGIN(CHAR); }
<CHAR>[^\'\\\r\n]+ { literal += yytext; }
<CHAR>[\r\n] { printf("ERROR line#%d: incorrect symbol in char literal\n", lineCounter+1); }
<CHAR>\' {
    if (literal.size() == 0)
        printf("ERROR line#%d: empty char literal\n", lineCounter+1);
    else if (literal.size() == 1) 
        printf("char_literal: %c\n", literal[0]);
    else
        printf("ERROR line#%d: too long char literal\n", lineCounter+1); 
    BEGIN(INITIAL); 
}
<CHAR><<EOF>> { printf("ERROR line#%d: no closed single quote\n", lineCounter+1); BEGIN(INITIAL); }

\" { literal = ""; BEGIN(STRING); }
<STRING>[^\"\\\r\n]+ { literal += yytext; }
<STRING>[\r\n] { printf("ERROR line#%d: incorrect symbol in string literal\n", lineCounter+1); }
<STRING>\" { cout << "string_literal: " + literal << endl; BEGIN(INITIAL); }
<STRING><<EOF>> { printf("ERROR line#%d: no closed double quote\n", lineCounter+1); BEGIN(INITIAL); }

<STRING,CHAR>\\\'  { literal.push_back('\''); }
<STRING,CHAR>\\\"  { literal.push_back('\"'); }
<STRING,CHAR>\\\\  { literal.push_back('\\'); }
<STRING,CHAR>\\0   { literal.push_back('\0'); }
<STRING,CHAR>\\a   { literal.push_back('\a'); }
<STRING,CHAR>\\b   { literal.push_back('\b'); }
<STRING,CHAR>\\f   { literal.push_back('\f'); }
<STRING,CHAR>\\n   { literal.push_back('\n'); }
<STRING,CHAR>\\r   { literal.push_back('\r'); }
<STRING,CHAR>\\t   { literal.push_back('\t'); }
<STRING,CHAR>\\v   { literal.push_back('\v'); }

\@\"[^\"]*\" { 
    buffer = yytext + 2;
    buffer[strlen(buffer)-1]='\0';
    printf("string_literal: %s\n", buffer); 
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
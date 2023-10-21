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

%x STRING_STATE
%x CHAR_STATE
%x VERBATIM_STRING_STATE
%x COMMENT_STATE

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
int lastState = 0;
string literal = "";
string comment = ""; 
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
in        { cout << "keyword: in" << endl; }

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
bool      { cout << "keyword: bool" << endl; }

true      { cout << "boolean_literal: true" << endl; }
false     { cout << "boolean_literal: false" << endl; }


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

"!"  { cout << "inversion" << endl; }
"&&" { cout << "conjunction" << endl; }
"||" { cout << "disjunction" << endl; }


{DIGIT}(_*{DIGIT})* {
    buffer = yytext;
    delete_underlines(buffer, digit);
    printf( "integer_literal: %d\n", atoi(digit) ); 
}

{DIGIT}(_*{DIGIT})*_+ {
    buffer = yytext;
    printf( "ERROR line#%d: invalid integer literal: %s\n", lineCounter+1, buffer ); 
}

(0x|0X)(_*{HEX_DIGIT})+ {
    buffer = yytext + 2;
    delete_underlines(buffer, digit);
    char* a;
    printf( "hexadecimal_literal %d\n", strtol(digit, &a, 16) );
}

(0x|0X)(_*{HEX_DIGIT})*_* {
    buffer = yytext;
    printf( "ERROR line#%d: invalid integer literal: %s\n", lineCounter+1, buffer ); 
}

(0b|0B)(_*{BIN_DIGIT})+ {
    buffer = yytext + 2;
    delete_underlines(buffer, digit);
    char* a;
    printf( "binary_literal %d\n", strtol(digit, &a, 2) );
}

(0b|0B)(_*{BIN_DIGIT})*_* {
    buffer = yytext;
    printf( "ERROR line#%d: invalid integer literal: %s\n", lineCounter+1, buffer ); 
}

\' { literal = ""; lastState = lineCounter+1; BEGIN(CHAR_STATE); }
<CHAR_STATE>[^\'\\\r\n]+ { literal += yytext; }
<CHAR_STATE>[\r\n] { printf("ERROR line#%d: incorrect character in a char literal\n", lineCounter+1); lineCounter++; }
<CHAR_STATE>\' {
    if (literal.size() == 0)
        printf("ERROR line#%d: empty char literal\n", lineCounter+1);
    else if (literal.size() == 1) 
        printf("char_literal: %c\n", literal[0]);
    else
        printf("ERROR line#%d: too long char literal: %s\n", lineCounter+1, literal.data()); 
    BEGIN(INITIAL); 
}
<CHAR_STATE><<EOF>> { printf("ERROR line#%d: no closing single quote\n", lastState); BEGIN(INITIAL); }

\" { literal = ""; lastState = lineCounter+1; BEGIN(STRING_STATE); }
<STRING_STATE>[^\"\\\r\n]+ { literal += yytext; }
<STRING_STATE>[\r\n] { printf("ERROR line#%d: incorrect character in a string literal\n", lineCounter+1); lineCounter++; }
<STRING_STATE>\" { cout << "string_literal: " + literal << endl; BEGIN(INITIAL); }
<STRING_STATE><<EOF>> { printf("ERROR line#%d: no closing double quote\n", lastState); BEGIN(INITIAL); }

<STRING_STATE,CHAR_STATE>\\\'  { literal.push_back('\''); }
<STRING_STATE,CHAR_STATE>\\\"  { literal.push_back('\"'); }
<STRING_STATE,CHAR_STATE>\\\\  { literal.push_back('\\'); }
<STRING_STATE,CHAR_STATE>\\0   { literal.push_back('\0'); }
<STRING_STATE,CHAR_STATE>\\a   { literal.push_back('\a'); }
<STRING_STATE,CHAR_STATE>\\b   { literal.push_back('\b'); }
<STRING_STATE,CHAR_STATE>\\f   { literal.push_back('\f'); }
<STRING_STATE,CHAR_STATE>\\n   { literal.push_back('\n'); }
<STRING_STATE,CHAR_STATE>\\r   { literal.push_back('\r'); }
<STRING_STATE,CHAR_STATE>\\t   { literal.push_back('\t'); }
<STRING_STATE,CHAR_STATE>\\v   { literal.push_back('\v'); }
<STRING_STATE,CHAR_STATE>\\x{HEX_DIGIT}{1,4}|\\u{HEX_DIGIT}{4} {
    buffer = yytext + 2;
    char* a;
    int num = strtol(buffer, &a, 16);
    if (num < 127){
        literal.push_back(num);
    }
    else{
        printf("ERROR line%d: unsupported character 0x%s\n", lineCounter+1, buffer);
    }
}

\@\" { literal = ""; lastState = lineCounter+1; BEGIN(VERBATIM_STRING_STATE); }
<VERBATIM_STRING_STATE>[^\"]+ { literal += yytext; }
<VERBATIM_STRING_STATE>\"\"   { literal.push_back('\"'); }
<VERBATIM_STRING_STATE>\" { cout << "string_literal: " + literal << endl; BEGIN(INITIAL); }
<VERBATIM_STRING_STATE><<EOF>> { printf("ERROR line#%d: no closing double quote\n", lastState); BEGIN(INITIAL); }


"//"[^\r\n]* {
    buffer = yytext + 2; 
    printf("single_line_comment: %s\n", buffer);
}

"/*" { comment = ""; lastState = lineCounter+1; BEGIN(COMMENT_STATE); }
<COMMENT_STATE>[^*\r\n]*       { comment += yytext; }
<COMMENT_STATE>"*"+[^*\/\r\n]* { comment += yytext; }
<COMMENT_STATE>[\r\n]          { comment.push_back('\n'); lineCounter++; }
<COMMENT_STATE>"*"+"/"         { cout << "delimited_comment: " << comment << endl; BEGIN(INITIAL); }
<COMMENT_STATE><<EOF>>         { printf("ERROR line#%d: no closing delimited comment", lastState); BEGIN(INITIAL); }


[\r\n] { lineCounter++; }

[ \t]+ { /* eat up whitespace */ }

. { printf( "ERROR line#%d: unrecognized character: %s\n", lineCounter+1, yytext ); }

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
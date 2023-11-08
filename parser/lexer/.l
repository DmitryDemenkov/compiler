%{
#include <fstream>
#include "_tab.h"
#include "classes.h"
extern int yyparse();
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

abstract  { return ABSTRACT; }
virtual   { return VIRTUAL; }
override  { return OVERRIDE; }

base      { return BASE; }
this      { return THIS; }
class     { return CLASS; }
namespace { return NAMESPACE; }
static    { return STATIC; }
return    { return RETURN; }

new       { return NEW; }
as        { return AS; }
is        { return IS; }
in        { return IN; }

public    { return PUBLIC; }
internal  { return INTERNAL; }
protected { return PROTECTED; }
private   { return PRIVATE; }

do        { return DO; }
if        { return IF; }
else      { return ELSE; }
for       { return FOR; }
foreach   { return FOREACH; }
while     { return WHILE; }

void      { return VOID; }
int       { return INT; }
char      { return CHAR; }
string    { return STRING; }
bool      { return BOOL; }

true      { yylval.boolean_literal = true; return BOOLEAN_LITERAL; }
false     { yylval.boolean_literal = false; return BOOLEAN_LITERAL; }


\@?{ID} { 
    buffer = yytext;
    if (buffer[0] == '@') { buffer++; }
    yylval.identifier = new string(buffer);
    return ID;
}


"{"  { return '{'; }
"}"  { return '}'; }
"["  { return '['; }
"]"  { return ']'; }
"("  { return '('; }
")"  { return ')'; }

"."  { return '.'; }
","  { return ','; }
":"  { return ':'; }
";"  { return ';'; }

"+"  { return '+'; }
"-"  { return '-'; }
"*"  { return '*'; }
"/"  { return '/'; }
"%"  { return '%'; }

"<"  { return '<'; }
">"  { return '>'; }
"<=" { return LESS_EQUAL;}
">=" { return GREATER_EQUAL;}
"==" { return EQUALITY;}
"!=" { return INEQUALITY;}

"="  { return '='; }

"!"  { return '!'; }
"&&" { return AND; }
"||" { return OR; }


{DIGIT}(_*{DIGIT})* {
    buffer = yytext;
    delete_underlines(buffer, digit);
	yylval.int_literal = atoi(digit);
	return INT_LITERAL;
}

{DIGIT}(_*{DIGIT})*_+ {
    buffer = yytext;
	printf( "ERROR line#%d: invalid integer literal: %s\n", lineCounter+1, buffer ); 
	return -1;
}

(0x|0X)(_*{HEX_DIGIT})+ {
    buffer = yytext + 2;
    delete_underlines(buffer, digit);
    char* a;
	yylval.int_literal = strtol(digit, &a, 16);
	return INT_LITERAL;
}

(0x|0X)(_*{HEX_DIGIT})*_* {
    buffer = yytext;
    printf( "ERROR line#%d: invalid integer literal: %s\n", lineCounter+1, buffer );
	return -1;
}

(0b|0B)(_*{BIN_DIGIT})+ {
    buffer = yytext + 2;
    delete_underlines(buffer, digit);
    char* a;
	yylval.int_literal = strtol(digit, &a, 2);
	return INT_LITERAL;
	
}

(0b|0B)(_*{BIN_DIGIT})*_* {
    buffer = yytext;
    printf( "ERROR line#%d: invalid integer literal: %s\n", lineCounter+1, buffer ); 
	return -1;
}

\' { literal = ""; lastState = lineCounter+1; BEGIN(CHAR_STATE); }
<CHAR_STATE>[^\'\\\r\n]+ { literal += yytext; }
<CHAR_STATE>[\r\n] { printf("ERROR line#%d: incorrect character in a char literal\n", lineCounter+1); lineCounter++; return -1; }
<CHAR_STATE>\' {
    if (literal.size() == 0) {
        printf("ERROR line#%d: empty char literal\n", lineCounter+1);
		return -1;
    }
    else if (literal.size() == 1) {
		yylval.char_literal = literal[0];
        BEGIN(INITIAL);
		return CHAR_LITERAL;
    }
    else {
        printf("ERROR line#%d: too long char literal: %s\n", lineCounter+1, literal.data()); 
		return -1; 
    }
}
<CHAR_STATE><<EOF>> { printf("ERROR line#%d: no closing single quote\n", lastState); return -1; }

\" { literal = ""; lastState = lineCounter+1; BEGIN(STRING_STATE); }
<STRING_STATE>[^\"\\\r\n]+ { literal += yytext; }
<STRING_STATE>[\r\n] { printf("ERROR line#%d: incorrect character in a string literal\n", lineCounter+1); lineCounter++; return -1; }
<STRING_STATE>\" { yylval.string_literal = new string(literal); BEGIN(INITIAL); return STRING_LITERAL; }
<STRING_STATE><<EOF>> { printf("ERROR line#%d: no closing double quote\n", lastState); return -1; }

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
		return -1;
    }
}

\@\" { literal = ""; lastState = lineCounter+1; BEGIN(VERBATIM_STRING_STATE); }
<VERBATIM_STRING_STATE>[^\"]+ { literal += yytext; }
<VERBATIM_STRING_STATE>\"\"   { literal.push_back('\"'); }
<VERBATIM_STRING_STATE>\" { yylval.string_literal = new string(literal); BEGIN(INITIAL); return STRING_LITERAL; }
<VERBATIM_STRING_STATE><<EOF>> { printf("ERROR line#%d: no closing double quote\n", lastState); return -1; }


"//"[^\r\n]* /* eat up single line comments */

"/*" { comment = ""; lastState = lineCounter+1; BEGIN(COMMENT_STATE); }
<COMMENT_STATE>[^*\r\n]*       { comment += yytext; }
<COMMENT_STATE>"*"+[^*\/\r\n]* { comment += yytext; }
<COMMENT_STATE>[\r\n]          { comment.push_back('\n'); lineCounter++; }
<COMMENT_STATE>"*"+"/"         { BEGIN(INITIAL); }
<COMMENT_STATE><<EOF>>         { printf("ERROR line#%d: no closing delimited comment", lastState); return -1; }


[\r\n] { lineCounter++; }

[ \t]+ { /* eat up whitespace */ }

. { printf( "ERROR line#%d: unrecognized character: %s\n", lineCounter+1, yytext ); return -1; }

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
	if (argc > 0)
		yyin = fopen(argv[0], "r");
	else
		yyin = stdin;

	cout << "start parsing" << endl;
	yyparse();

	cout << "parsed" << endl;

	if (Programm::main == NULL)
		cout << "null prg" << endl;

	string dotStr = "digraph tree{ rankdir=\"LR\"\n" + *Programm::main->ToDOT() + "}";
	ofstream file;
	file.open("tree.gv");
	if (file.is_open())
	{
		file << dotStr;
		cout << "Success";
	}
	else
	{
		cout << "Error";
	}
}
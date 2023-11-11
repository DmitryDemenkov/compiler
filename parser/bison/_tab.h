#include <iostream>
#include <string>
#include "classes.h"
using namespace std;
typedef union {
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
	ObjectCreation* objectCreation;
	MemberInitializer* memberInitializer;
	MemberInitializerList* memberInitializerList;
	Expression* expression;
	ExpressionList* expressionList;
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
	Constructor* construct;
	ClassDeclaration* classDeclaration;
	NamespaceMember* namespaceMember;
	NamespaceMemberList* namespaceMemberList;
	NamespaceDeclaration* namespaceDeclaration;
	Programm* programm;	
} YYSTYPE;
#define	ABSTRACT	258
#define	VIRTUAL	259
#define	OVERRIDE	260
#define	BASE	261
#define	THIS	262
#define	CLASS	263
#define	NAMESPACE	264
#define	STATIC	265
#define	RETURN	266
#define	PUBLIC	267
#define	INTERNAL	268
#define	PROTECTED	269
#define	PRIVATE	270
#define	DO	271
#define	IF	272
#define	FOR	273
#define	FOREACH	274
#define	IN	275
#define	WHILE	276
#define	VOID	277
#define	INT	278
#define	CHAR	279
#define	STRING	280
#define	BOOL	281
#define	INT_LITERAL	282
#define	CHAR_LITERAL	283
#define	STRING_LITERAL	284
#define	BOOLEAN_LITERAL	285
#define	ID	286
#define	OR	287
#define	AND	288
#define	EQUALITY	289
#define	INEQUALITY	290
#define	LESS_EQUAL	291
#define	GREATER_EQUAL	292
#define	IS	293
#define	AS	294
#define	UNMINUS	295
#define	NEW	296
#define	THEN	297
#define	ELSE	298


extern YYSTYPE yylval;

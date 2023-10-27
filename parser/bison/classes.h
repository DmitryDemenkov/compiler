#pragma once

#include <iostream>
#include <string>
#include <list>
using namespace std;

class Expression;
class MemberInitializerList;
class ArrayInitializer;

class SimpleType
{
public:
    enum Type
    {
        t_INT,
        t_CHAR,
        t_BOOL,
        t_STRING
    };

    int id;
    Type type;

    SimpleType(Type type);

    string* ToDOT();
};

class TypeName
{
public:
    int id;
    list<string*>* identifiers;

    TypeName(string* identifier);
    static void Append(TypeName* typeName, string* identifier);

    string* ToDOT();
};

class ArrayType
{
public:
    enum Type
    {
        t_INT,
        t_CHAR,
        t_BOOL,
        t_STRING,
        t_TYPE_NAME
    };

    int id;
    Type type;

    ArrayType(Type type);

    string* ToDOT();
};

class Argument
{
public:
    int id;
    string* identifier = NULL;
    Expression* expression = NULL;

    Argument(Expression* expression, string* identifier = NULL);

    string* ToDOT();
};

class ArgumentList
{
public:
    int id;
    list<Argument*>* arguments = NULL;

    ArgumentList(Argument* argument);
    static void Append(ArgumentList* list, Argument* argument);

    string* ToDOT();
};

class ObjectInitializer
{
public:
    int id;
    MemberInitializerList* initializers = NULL;

    ObjectInitializer(MemberInitializerList* initializers);

    string* ToDOT();
};

class MemberInitializer
{
public:
    int id;
    string* identifier = NULL;
    Expression* expression = NULL;
    ArgumentList* argumentList = NULL;
    ObjectInitializer* objectInitializer = NULL;

    MemberInitializer(string* identifier, Expression* expression);
    MemberInitializer(string* identifier, ObjectInitializer* objectInitializer);
    MemberInitializer(ArgumentList* argumentList, Expression* expression);
    MemberInitializer(ArgumentList* argumentList, ObjectInitializer* objectInitializer);

    string* ToDOT();
};

class MemberInitializerList
{
public:
    int id;
    list<MemberInitializer*>* initializers = NULL;

    MemberInitializerList(MemberInitializer* memberInitializer);
    static void Append(MemberInitializerList* list, MemberInitializer* memberInitializer);

    string* ToDOT();
};


class Expression
{
public:
    enum Type
    {
        t_INT_LITER,
        t_CHAR_LITER,
        t_BOOL_LITER,
        t_STRING_LITER,
        t_ID,
        t_SIMPLE_TYPE,
        t_THIS,
        t_BASE,
        t_OBJ_CREATION,
        t_ARR_CREATION,
        t_ELEMENT_ACCESS,
        t_MEMBER_ACCESS,
        t_INVOCATION,
        t_PARENTHESIZED,
        t_UNMINUS,
        t_NOT,
        t_SIMPLE_TYPE_CAST,
        t_ARRAY_CAST,
        t_TYPENAME_CAST,
        t_MUL,
        t_DIV,
        t_MOD,
        t_SUM,
        t_SUB,
        t_LESS,
        t_GREATER,
        t_LESS_EQUAL,
        t_GREATER_EQUAL,
        t_IS,
        t_AS,
        t_EQUALITY,
        t_INEQUALITY,
        t_AND,
        t_OR,
        t_ASSIGNMENT
    };

    int id;
    Type type;
    int intLiteral = 0;
    bool boolLiteral = false;
    char charLiteral = 0;
    string* name = NULL;
    SimpleType* simpleType = NULL;
    TypeName* typeName = NULL;
    ArrayType* arrayType = NULL;
    ArgumentList* argumentList = NULL;
    ObjectInitializer* objInitializer = NULL;
    ArrayInitializer* arrayInitializer = NULL;
    Expression* left = NULL;
    Expression* right = NULL;

    Expression(Type type, string* name = NULL);
    Expression(int intLiteral);
    Expression(char charLiteral);
    Expression(string* stringliteral);
    Expression(bool boolLiteral);
    Expression(Type type, SimpleType* simpleType, Expression* expr = NULL);
    Expression(Type type, ArrayType* arrayType, Expression* expr = NULL);
    Expression(Type type, TypeName* typeName, Expression* expr = NULL);
    Expression(Type type, Expression* left, Expression* right = NULL);
    Expression(Type type, Expression* left, ArgumentList* arguments);

    string* ToDOT();
};

class ObjectCreation : public Expression
{
public:
    ObjectCreation(SimpleType* simpleType,
        ArgumentList* argumentList = NULL, ObjectInitializer* objInit = NULL);

    ObjectCreation(TypeName* typeName,
        ArgumentList* argumentList = NULL, ObjectInitializer* objInit = NULL);
};

class ExpressionList
{
public:
    int id;
    list<Expression*>* expressions = NULL;

    ExpressionList(Expression* expression);
    static void Append(ExpressionList* list, Expression* expression);

    string* ToDOT();
};

class ArrayInitializer
{
public:
    ExpressionList* expressions = NULL;

    ArrayInitializer(ExpressionList* expressions);

    string* ToDOT();
};

class ArrayCreation : public Expression
{
public:
    ArrayCreation(ArrayType* arrType, ArrayInitializer* arrInit = NULL);
    
    ArrayCreation(SimpleType* simpleType,
        Expression* expr = NULL, ArrayInitializer* arrInit = NULL);
    
    ArrayCreation(TypeName* typeName,
        Expression* expr = NULL, ArrayInitializer* arrInit = NULL);
};

class MemberAccess : public Expression
{
public:
    static Expression* FromTypeName(TypeName* typeName, Expression* left = NULL);
};

class ElementAccess : public Expression
{
public:
    ElementAccess(Expression* expr, ArgumentList* arguments);
};

class InvocationExpression : public Expression
{
public:
    InvocationExpression(Expression* expr, ArgumentList* arguments);
};


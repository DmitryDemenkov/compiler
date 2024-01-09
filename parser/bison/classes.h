#pragma once

#include <iostream>
#include <string>
#include <list>
#include <queue>
#include "../Parser/Parser/tables.h"
using namespace std;

class Expression;
class MemberInitializerList;
class ExpressionList;
class StatementList;
class ClassMemberList;
class NamespaceDeclaration;

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
    SimpleType(SimpleType* other);

    string* ToDOT();
    string* GetName();
};

class TypeName
{
public:
    int id;
    list<string*>* identifiers = NULL;

    TypeName(string* identifier);
    TypeName(TypeName* other);
    static TypeName* Append(TypeName* typeName, string* identifier);

    string* ToDOT();
    string ToString();
};

class ArrayType
{
public:
    enum Type
    {
        t_SIMPLE_TYPE,
        t_TYPE_NAME
    };

    int id;
    Type type;

    SimpleType* simpleType = NULL;
    TypeName* typeName = NULL;

    ArrayType(SimpleType* simpleType);
    ArrayType(TypeName* typeName);
    ArrayType(ArrayType* other);

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

    void DetermineDataType(Class* owner, MethodTable* methodInfo);
};

class ArgumentList
{
public:
    int id;
    list<Argument*>* arguments = NULL;

    ArgumentList(Argument* argument);
    static ArgumentList* Append(ArgumentList* list, Argument* argument);

    string* ToDOT();
};

class MemberInitializer
{
public:
    int id;
    string* identifier = NULL;
    Expression* expression = NULL;
    ArgumentList* argumentList = NULL;
    MemberInitializerList* objectInitializer = NULL;

    DataType* dataType = NULL;

    MemberInitializer(string* identifier, Expression* expression);
    MemberInitializer(string* identifier, MemberInitializerList* objectInitializer);
    MemberInitializer(ArgumentList* argumentList, Expression* expression);
    MemberInitializer(ArgumentList* argumentList, MemberInitializerList* objectInitializer);

    string* ToDOT();

    void DetermineDataType(Class* owner, MethodTable* methodInfo, Class* creatingClass);
};

class MemberInitializerList
{
public:
    int id;
    list<MemberInitializer*>* initializers = NULL;

    MemberInitializerList(MemberInitializer* memberInitializer);
    static MemberInitializerList* Append(MemberInitializerList* list, MemberInitializer* memberInitializer);

    string* ToDOT();

    void DetermineDataType(Class* owner, MethodTable* methodInfo, Class* creatingClass);
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
        t_ASSIGNMENT,

        t_CLASS,
        t_OBJECT
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
    MemberInitializerList* objInitializer = NULL;
    ExpressionList* arrayInitializer = NULL;
    Expression* left = NULL;
    Expression* right = NULL;

    DataType* dataType = NULL;

    Expression(Type type, string* name = NULL);
    Expression(Type type, int intLiteral);
    Expression(Type type, char charLiteral);
    Expression(Type type, bool boolLiteral);
    Expression(Type type, SimpleType* simpleType, Expression* expr = NULL);
    Expression(Type type, ArrayType* arrayType, Expression* expr = NULL);
    Expression(Type type, TypeName* typeName, Expression* expr = NULL);
    Expression(Type type, Expression* left, Expression* right = NULL);
    Expression(Type type, Expression* left, ArgumentList* arguments);

    virtual string* ToDOT();

    void DetermineDataType(Class* owner, MethodTable* methodInfo);

private:
    string* GetName();

    DataType* GetDataTypeOfId(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfInvocation(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfMemberAccess(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfObjectCreation(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfArrayCreation(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfElementAccess(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfTypeCast(Class* owner, MethodTable* methodInfo);
    DataType* GetDataTypeOfArithmetic(Class* owner, MethodTable* methodInfo);

    void CheckErrorsOfInvokation(Class* owner, MethodTable* methodInfo);
    void CheckErrorsOfFieldAccess(Class* owner, MethodTable* methodInfo);
    void CheckErrorsOfClassesAccess(Class* owner, Class* classInfo);
    void CheckErrorsOfObjectCreation(Class* owner, Class* classInfo);
    void CheckErrorsOfArrayCreation(Class* owner, DataType* arrayType);
};

class ObjectCreation : public Expression
{
public:
    ObjectCreation(SimpleType* simpleType,
        ArgumentList* argumentList = NULL, MemberInitializerList* objInit = NULL);

    ObjectCreation(TypeName* typeName,
        ArgumentList* argumentList = NULL, MemberInitializerList* objInit = NULL);

    string* ToDOT();
};

class ExpressionList
{
public:
    int id;
    list<Expression*>* expressions = NULL;

    ExpressionList(Expression* expression);
    static ExpressionList* Append(ExpressionList* list, Expression* expression);

    string* ToDOT();
};

class ArrayCreation : public Expression
{
public:
    ArrayCreation(ArrayType* arrType, ExpressionList* arrInit = NULL);
    
    ArrayCreation(SimpleType* simpleType,
        Expression* expr = NULL, ExpressionList* arrInit = NULL);
    
    ArrayCreation(TypeName* typeName,
        Expression* expr = NULL, ExpressionList* arrInit = NULL);

    string* ToDOT();
};

class MemberAccess : public Expression
{
public:
    static Expression* FromTypeName(TypeName* typeName, Expression* left = NULL);
    static TypeName* ToTypeName(Expression* memberAccess);
};

class ElementAccess : public Expression
{
public:
    ElementAccess(Expression* expr, ArgumentList* arguments);

    string* ToDOT();
};

class InvocationExpression : public Expression
{
public:
    InvocationExpression(Expression* expr, ArgumentList* arguments);

    string* ToDOT();
};

class VarDeclarator
{
public:
    enum Type
    {
        t_SIMPLE_TYPE,
        t_TYPE_NAME,
        t_ARRAY_TYPE
    };

    int id;
    Type type;
    SimpleType* simpleType = NULL;
    TypeName* typeName = NULL;
    ArrayType* arrayType = NULL;
    string* identifier = NULL;
    Expression* initializer = NULL;

    DataType* dataType = NULL;

    VarDeclarator(SimpleType* simpletype, string* identifier, Expression* expression = NULL);
    VarDeclarator(TypeName* typeName, string* identifier, Expression* expression = NULL);
    VarDeclarator(ArrayType* arraytype, string* identifier, Expression* expression = NULL);

    void Semantic(Class* owner, MethodTable* methodInfo);

    string* ToDOT();
};

class VarDeclaratorList
{
public:
    int id;
    VarDeclarator::Type type;
    list<VarDeclarator*>* declarators = NULL;
    VarDeclaratorList(VarDeclarator* declarator, Expression* expression = NULL);
    
    static VarDeclaratorList* Append(VarDeclaratorList* declarators, string* identifier, Expression* expression = NULL);

    string* ToDOT();
};

class Statement
{
public:
    enum Type
    {
        t_EMPTY,
        t_EXPRESSION,
        t_DECLARATOR,
        t_IF,
        t_WHILE,
        t_DO,
        t_FOR,
        t_FOREACH,
        t_RETURN,
        t_BLOCK
    };

    int id;
    Type type;
  
    VarDeclaratorList* declarators = NULL;
    ExpressionList* expressions = NULL;
    StatementList* statements = NULL;

    ExpressionList* inits = NULL;
    ExpressionList* incrs = NULL;

    Statement(Type type, Expression* expression = NULL);
    Statement(Type type, VarDeclaratorList* declarators);
    Statement(Type type, StatementList* statements);

    virtual string* ToDOT();

    void Semantic(Class* owner, MethodTable* methodInfo);
};

class StatementList
{
public:
    int id;
    list<Statement*>* statements = NULL;

    StatementList(Statement* statement);
    static StatementList* Append(StatementList *statements, Statement* statement);

    string* ToDOT();
};

class IfStatement : public Statement
{
public:
    IfStatement(Expression* expression, Statement* main, Statement* alternative = NULL);

    string* ToDOT();
};

class WhileStatement : public Statement
{
public:
    WhileStatement(Expression* expression, Statement* statement);

    string* ToDOT();
};

class DoStatement : public Statement
{
public:
    DoStatement(Statement* statement, Expression* expression);

    string* ToDOT();
};

class ForeachStatement : public Statement
{
public:
    ForeachStatement(VarDeclarator* declarator, Expression* expression, Statement* statement);

    string* ToDOT();
};

class ForStatement : public Statement
{
public:
    ForStatement(ExpressionList* init, Expression* cond, ExpressionList* increment, Statement* statement);
    ForStatement(VarDeclaratorList* declarations, Expression* cond, ExpressionList* increment, Statement* statement);

    string* ToDOT();
};

class ReturnStatement : public Statement
{
public:
    ReturnStatement(Expression* expression = NULL);

    string* ToDOT();
};

class ParamList
{
public:
    int id;
    list<VarDeclarator*>* params = NULL;

    ParamList(VarDeclarator* param);
    static ParamList* Append(ParamList* params, VarDeclarator* param);

    string* ToDOT();
};

class Modifier
{
public:
    enum Type
    {
        t_PRIVATE,
        t_PROTECTED,
        t_PUBLIC,
        t_INTERNAL,
        t_ABSTRACT,
        t_STATIC,
        t_OVERRIDE,
        t_VIRTUAL
    };

    int id;
    Type type;

    Modifier(Type type);

    string GetName();
    string* ToDOT();
};

class ModifielrList
{
public:
    int id;
    list<Modifier*>* modifiers = NULL;

    ModifielrList(Modifier* modifier);
    static ModifielrList* Append(ModifielrList* modifiers, Modifier* modifier);

    string* ToDOT();
};

class ClassMember
{
public:
    enum Type
    {
        t_FIELD,
        t_METHOD,
        t_CONSTRUCTOR,
        t_CLASS
    };

    enum ReturnValueType
    {
        t_EMPTY,
        t_SIMPLE_TYPE,
        t_TYPENAME,
        t_ARRAY,
        t_VOID
    };

    enum BaseConstructorType
    {
        t_NULL,
        t_BASE,
        t_THIS
    };

    int id;
    Type type;
    ReturnValueType returnValue;
    BaseConstructorType baseConstructor;

    ModifielrList* modifiers = NULL;
    SimpleType* simpleType = NULL;
    TypeName* typeName = NULL;
    ArrayType* arrayType = NULL;
    string* identifier = NULL;
    Expression* expression = NULL;
    ParamList* paramList = NULL;
    StatementList* statementList = NULL;
    ArgumentList* argumentList = NULL;
    ClassMemberList* classMemberList = NULL;

    ClassMember(Type type, ReturnValueType returnValue, BaseConstructorType baseConstructor);

    virtual string* ToDOT();
};

class ClassMemberList
{
public:
    int id;
    list<ClassMember*>* members = NULL;

    ClassMemberList(ClassMember* member);
    static ClassMemberList* Append(ClassMemberList *members, ClassMember* member);

    string* ToDOT();
};

class Method : public ClassMember
{
public:
    Method(ModifielrList* modifiers, ReturnValueType returnValue,
        string* identifiers, ParamList* params, StatementList* statements = NULL);

    Method(ModifielrList* modifiers, ReturnValueType returnValue, 
        SimpleType* simpleType, string* identifiers, ParamList* params, StatementList* statements = NULL);

    Method(ModifielrList* modifiers, ReturnValueType returnValue,
        TypeName* typeName, string* identifiers, ParamList* params, StatementList* statements = NULL);

    Method(ModifielrList* modifiers, ReturnValueType returnValue,
        ArrayType* arrayType, string* identifiers, ParamList* params, StatementList* statements = NULL);

    string* ToDOT();
};

class Field : public ClassMember
{
public:
    Field(ModifielrList* modifiers, ReturnValueType returnValue, 
        SimpleType* simpleType, string* identifier, Expression* expression = NULL);

    Field(ModifielrList* modifiers, ReturnValueType returnValue,
        TypeName* typeName, string* identifier, Expression* expression = NULL);

    Field(ModifielrList* modifiers, ReturnValueType returnValue,
        ArrayType* arraType, string* identifier, Expression* expression = NULL);

    string* ToDOT();
};

class Constructor : public ClassMember
{
public:
    Constructor(ModifielrList* modifiers, string* identifier, 
        ParamList* params, BaseConstructorType baseConstructor, StatementList* statements = NULL, ArgumentList* args = NULL);

    string* ToDOT();
};

class ClassDeclaration : public ClassMember
{
public:
    ClassDeclaration(ModifielrList* modifiers, 
        string* identifier, ClassMemberList* members, TypeName* baseClass = NULL);

    string* ToDOT();
    AbstractNamespaceMember* CreateClassTable(AbstractNamespaceMember* outer);
};

class NamespaceMember
{
public:
    enum Type
    {
        t_NAMESPACE,
        t_CLASS
    };

    int id;
    Type type;

    ClassDeclaration* classDecl = NULL;
    NamespaceDeclaration* namespaceDecl = NULL;

    NamespaceMember(ClassDeclaration* decl);
    NamespaceMember(NamespaceDeclaration* decl);

    string* ToDOT();

    AbstractNamespaceMember* CreateClassTable(AbstractNamespaceMember* outer);
};

class NamespaceMemberList
{
public:
    int id;
    list<NamespaceMember*>* members = NULL;

    NamespaceMemberList(NamespaceMember* member);
    static NamespaceMemberList* Append(NamespaceMemberList* members, NamespaceMember* member);

    string* ToDOT();

    AbstractNamespaceMember* CreateClassTable(AbstractNamespaceMember* outer);
};

class NamespaceDeclaration
{
public:
    int id;
    TypeName* typeName = NULL;
    NamespaceMemberList* members = NULL;

    NamespaceDeclaration(TypeName* typeName, NamespaceMemberList* members);

    string* ToDOT();
    AbstractNamespaceMember* CreateClassTable(AbstractNamespaceMember* outer);
};

class Programm
{
public:
    static Programm* main;

    int id;
    NamespaceMemberList* members = NULL;

    Programm(NamespaceMemberList* members);

    string* ToDOT();

    AbstractNamespaceMember* CreateClassTable();
};
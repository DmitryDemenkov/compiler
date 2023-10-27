#include "classes.h"

int maxId = 0;

SimpleType::SimpleType(Type type)
{
}

string* SimpleType::ToDOT()
{
	return nullptr;
}

TypeName::TypeName(string* identifier)
{
}

void TypeName::Append(TypeName* typeName, string* identifier)
{
}

string* TypeName::ToDOT()
{
	return nullptr;
}

ArrayType::ArrayType(Type type)
{
}

string* ArrayType::ToDOT()
{
	return nullptr;
}

Argument::Argument(Expression* expression, string* identifier)
{
}

string* Argument::ToDOT()
{
	return nullptr;
}

ArgumentList::ArgumentList(Argument* argument)
{
}

void ArgumentList::Append(ArgumentList* list, Argument* argument)
{
}

string* ArgumentList::ToDOT()
{
	return nullptr;
}

ObjectInitializer::ObjectInitializer(MemberInitializerList* initializers)
{
}

string* ObjectInitializer::ToDOT()
{
	return nullptr;
}

MemberInitializer::MemberInitializer(string* identifier, Expression* expression)
{
}

MemberInitializer::MemberInitializer(string* identifier, ObjectInitializer* objectInitializer)
{
}

MemberInitializer::MemberInitializer(ArgumentList* argumentList, Expression* expression)
{
}

MemberInitializer::MemberInitializer(ArgumentList* argumentList, ObjectInitializer* objectInitializer)
{
}

string* MemberInitializer::ToDOT()
{
	return nullptr;
}

MemberInitializerList::MemberInitializerList(MemberInitializer* memberInitializer)
{
}

void MemberInitializerList::Append(MemberInitializerList* list, MemberInitializer* memberInitializer)
{
}

string* MemberInitializerList::ToDOT()
{
	return nullptr;
}

Expression::Expression(Type type, string* name)
{
}

Expression::Expression(int literal)
{
}

Expression::Expression(char literal)
{
}

Expression::Expression(string* literal)
{
}

Expression::Expression(bool literal)
{
}

Expression::Expression(Type type, SimpleType* simpleType, Expression* expr)
{
}

Expression::Expression(Type type, ArrayType* arrayType, Expression* expr)
{
}

Expression::Expression(Type type, TypeName* typeName, Expression* expr)
{
}

Expression::Expression(Type type, Expression* left, Expression* right)
{
}

Expression::Expression(Type type, Expression* left, ArgumentList* arguments)
{
}

string* Expression::ToDOT()
{
	return nullptr;
}

ObjectCreation::ObjectCreation(SimpleType* simleType, 
	ArgumentList* argumentList, ObjectInitializer* objInit) : Expression(Expression::t_OBJ_CREATION)
{
}

ObjectCreation::ObjectCreation(TypeName* typeName, 
	ArgumentList* argumentList, ObjectInitializer* objInit) : Expression(Expression::t_OBJ_CREATION)
{
}

ExpressionList::ExpressionList(Expression* expression)
{
}

void ExpressionList::Append(ExpressionList* list, Expression* expression)
{
}

string* ExpressionList::ToDOT()
{
	return nullptr;
}

ArrayInitializer::ArrayInitializer(ExpressionList* expressions)
{
}

string* ArrayInitializer::ToDOT()
{
	return nullptr;
}

ArrayCreation::ArrayCreation(ArrayType* arrType, 
	ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
}

ArrayCreation::ArrayCreation(SimpleType* simpleType, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
}

ArrayCreation::ArrayCreation(TypeName* typeName, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
}

Expression* MemberAccess::FromTypeName(TypeName* typeName, Expression* left)
{
	return nullptr;
}

ElementAccess::ElementAccess(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_ELEMENT_ACCESS)
{
}

InvocationExpression::InvocationExpression(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_INVOCATION)
{
}

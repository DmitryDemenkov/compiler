#include "classes.h"

int maxId = 0;

SimpleType::SimpleType(Type type)
{
	this->id = ++maxId;
	this->type = type; 
}

string* SimpleType::ToDOT()
{
	return nullptr;
}

TypeName::TypeName(string* identifier)
{
	this->id = ++maxId;
	this->identifiers = new list <string*>{ identifier };

}

void TypeName::Append(TypeName* typeName, string* identifier)
{
	
	typeName->identifiers->push_back(identifier);

}

string* TypeName::ToDOT()
{
	return nullptr;
}

ArrayType::ArrayType(Type type)
{
	this->id = ++maxId;
	this->type = type;
}

string* ArrayType::ToDOT()
{
	return nullptr;
}

Argument::Argument(Expression* expression, string* identifier)
{
	this->id = ++maxId;
	this->expression = expression;
	this->identifier = identifier;
}

string* Argument::ToDOT()
{
	return nullptr;
}

ArgumentList::ArgumentList(Argument* argument)
{
	this->id = ++maxId;
	this->arguments = new list <Argument*>{ argument };

}

void ArgumentList::Append(ArgumentList* list, Argument* argument)
{
	list->arguments->push_back(argument);
}

string* ArgumentList::ToDOT()
{
	return nullptr;
}

ObjectInitializer::ObjectInitializer(MemberInitializerList* initializers)
{
	this->id = ++maxId;
	this->initializers = initializers;
}

string* ObjectInitializer::ToDOT()
{
	return nullptr;
}

MemberInitializer::MemberInitializer(string* identifier, Expression* expression)
{
	this->id = ++maxId;
	this->identifier = identifier;
	this->expression = expression;
	
}

MemberInitializer::MemberInitializer(string* identifier, ObjectInitializer* objectInitializer)
{
	this->id = ++maxId;
	this->objectInitializer = objectInitializer;
	this->identifier = identifier;
	
}

MemberInitializer::MemberInitializer(ArgumentList* argumentList, Expression* expression)
{
	this->id = ++maxId;
	this->argumentList = argumentList;
	this->expression = expression;
	
}

MemberInitializer::MemberInitializer(ArgumentList* argumentList, ObjectInitializer* objectInitializer)
{
	this->id = ++maxId;
	this->argumentList = argumentList;
	this->objectInitializer = objectInitializer;
}

string* MemberInitializer::ToDOT()
{
	return nullptr;
}

MemberInitializerList::MemberInitializerList(MemberInitializer* memberInitializer)
{
	this->id = ++maxId;
	this->initializers = new list <MemberInitializer*>{ memberInitializer };
}

void MemberInitializerList::Append(MemberInitializerList* list, MemberInitializer* memberInitializer)
{
	list->initializers->push_back(memberInitializer);
}

string* MemberInitializerList::ToDOT()
{
	return nullptr;
}

Expression::Expression(Type type, string* name)
{
	this->id = ++maxId;
	this->type = type;
	this->name = name;
}

Expression::Expression(int intLiteral)
{
	this->id = ++maxId;
	this->type = Expression::t_INT_LITER;
	this->intLiteral = intLiteral;
}

Expression::Expression(char charLiteral)
{
	this->id = ++maxId;
	this->type = Expression::t_CHAR_LITER;
	this->charLiteral = charLiteral;
}

Expression::Expression(string* stringliteral)
{
	this->id = ++maxId;
	this->type = Expression::t_STRING_LITER;
	this->name = stringliteral;
}

Expression::Expression(bool boolLiteral)
{
	this->id = ++maxId;
	this->type = Expression::t_BOOL_LITER;
	this->boolLiteral = boolLiteral;
}

Expression::Expression(Type type, SimpleType* simpleType, Expression* expr)
{
	this->id = ++maxId;
	this->type = type;
	this->simpleType = simpleType;
	this->left = expr;
}

Expression::Expression(Type type, ArrayType* arrayType, Expression* expr)
{
	this->id = ++maxId;
	this->type = type;
	this->arrayType = arrayType;
	this->left = expr;
}

Expression::Expression(Type type, TypeName* typeName, Expression* expr)
{
	this->id = ++maxId;
	this->type = type;
	this->typeName = typeName;
	this->left = expr;
}

Expression::Expression(Type type, Expression* left, Expression* right)
{
	this->id = ++maxId;
	this->type = type;
	this->left = left;
	this->right = right;
}

Expression::Expression(Type type, Expression* left, ArgumentList* arguments)
{
	this->id = ++maxId;
	this->type = type;
	this->left = left;
	this->argumentList = arguments;
}

string* Expression::ToDOT()
{
	return nullptr;
}

ObjectCreation::ObjectCreation(SimpleType* simpleType, 
	ArgumentList* argumentList, ObjectInitializer* objInit) : Expression(Expression::t_OBJ_CREATION)
{
	this->simpleType = simpleType;
	this->argumentList = argumentList;
	this->objInitializer = objInit;
		

}

ObjectCreation::ObjectCreation(TypeName* typeName, 
	ArgumentList* argumentList, ObjectInitializer* objInit) : Expression(Expression::t_OBJ_CREATION)
{
	this->typeName = typeName;
	this->argumentList = argumentList;
	this->objInitializer = objInit;
}

ExpressionList::ExpressionList(Expression* expression)
{
	this->id = ++maxId;
	this->expressions = new list <Expression*>{ expression };
}

void ExpressionList::Append(ExpressionList* list, Expression* expression)
{
	list->expressions->push_back(expression);
}

string* ExpressionList::ToDOT()
{
	return nullptr;
}

ArrayInitializer::ArrayInitializer(ExpressionList* expressions)
{
	this->expressions = expressions;
}

string* ArrayInitializer::ToDOT()
{
	return nullptr;
}

ArrayCreation::ArrayCreation(ArrayType* arrType, 
	ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->arrayType = arrType;
	this->arrayInitializer = arrInit;

}

ArrayCreation::ArrayCreation(SimpleType* simpleType, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->simpleType = simpleType;
	this->left = expr;
}

ArrayCreation::ArrayCreation(TypeName* typeName, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->typeName = typeName;
	this->left = expr;
	this->arrayInitializer = arrInit;
}

Expression* MemberAccess::FromTypeName(TypeName* typeName, Expression* left)
{
	return nullptr;
}

ElementAccess::ElementAccess(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_ELEMENT_ACCESS)
{
	this->left = expr;
	this->argumentList = arguments;
}

InvocationExpression::InvocationExpression(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_INVOCATION)
{
	this->left = expr;
	this->argumentList = arguments;
}

VarDeclarator::VarDeclarator(SimpleType* simpletype, Expression* expression)
{
}

VarDeclarator::VarDeclarator(TypeName* typeName, Expression* expression)
{
}

VarDeclarator::VarDeclarator(ArrayType* arraytype, Expression* expression)
{
}

void VarDeclarator::AddInitializer(VarDeclarator* declarator, Expression* expression)
{
}

VarDeclaratorList::VarDeclaratorList(VarDeclarator* declarator, Expression* expression)
{
}

void VarDeclaratorList::Append(string* identifier, Expression* expression)
{
}

Statement::Statement(Type type, Expression* expression)
{
}

Statement::Statement(Type type, VarDeclaratorList* declarators)
{
}

Statement::Statement(Type type, StatementList* statements)
{
}

StatementList::StatementList(Statement* statement)
{
}

void StatementList::Append(StatementList statements, Statement* statement)
{
}

IfStatement::IfStatement(Expression* expression, 
	Statement* main, Statement* alternative) : Statement(Statement::t_IF)
{
}

WhileStatement::WhileStatement(Expression* expression, 
	Statement* statement) : Statement(Statement::t_WHILE)
{
}

DoStatement::DoStatement(Statement* statement, 
	Expression* expression) : Statement(Statement::t_DO)
{
}

ForeachStatement::ForeachStatement(VarDeclarator* declarator, 
	Expression* expression, Statement* statement) : Statement(Statement::t_FOREACH)
{
}

ForStatement::ForStatement(Expression* init, 
	Expression* cond, Expression* increment, Statement* statement) : Statement(Statement::t_FOR)
{
}

ForStatement::ForStatement(VarDeclaratorList* declarations, 
	Expression* cond, Expression* increment, Statement* statement) : Statement(Statement::t_FOR)
{
}

ReturnStatement::ReturnStatement(Expression* expression) : Statement(Statement::t_RETURN, expression)
{
}

ParamList::ParamList(VarDeclarator* param)
{
}

void ParamList::Append(ParamList* params, VarDeclarator* param)
{
}

Modifier::Modifier(Type type)
{
}

ModifielrList::ModifielrList(Modifier* modifier)
{
}

void ModifielrList::Append(ModifielrList* modifiers, Modifier* modifier)
{
}

ClassMember::ClassMember(Type type, ReturnValueType returnValue, BaseConstructorType baseConstructor)
{
}

ClassMemberList::ClassMemberList(ClassMember* member)
{
}

void ClassMemberList::Append(ClassMemberList members, ClassMember* member)
{
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	SimpleType* simpleType, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	TypeName* typeName, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	ArrayType* arrayType, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	SimpleType* simpleType, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	TypeName* typeName, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	ArrayType* arraType, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
}

Constructor::Constructor(ModifielrList* modifiers, string* identifier, 
	ParamList* params, BaseConstructorType baseConstructor, StatementList* statements, ArgumentList* args)
	: ClassMember(ClassMember::t_CONSTRUCTOR, ClassMember::t_EMPTY, baseConstructor)
{
}

ClassDeclaration::ClassDeclaration(ModifielrList* modifiers, 
	string* identifier, ClassMemberList* members, TypeName* baseClass)
	: ClassMember(ClassMember::t_CLASS, ClassMember::t_EMPTY, ClassMember::t_NULL)
{
}

NamespaceMember::NamespaceMember(ClassDeclaration* decl)
{
}

NamespaceMember::NamespaceMember(NamespaceDeclaration* decl)
{
}

NamespaceMemberList::NamespaceMemberList(NamespaceMember* member)
{
}

void NamespaceMemberList::Append(NamespaceMemberList* members, NamespaceMember* member)
{
}

NamespaceDeclaration::NamespaceDeclaration(TypeName* typeName, NamespaceMemberList* members)
{
}

Programm::Programm(NamespaceMemberList* members)
{
}
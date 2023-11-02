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
	this->id = ++maxId;
	this->expressions = expressions;
}

string* ArrayInitializer::ToDOT()
{
	return nullptr;
}

ArrayCreation::ArrayCreation(ArrayType* arrType, 
	ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->id = ++maxId;
	this->arrayType = arrType;
	this->arrayInitializer = arrInit;

}

ArrayCreation::ArrayCreation(SimpleType* simpleType, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->id = ++maxId;
	this->simpleType = simpleType;
	this->left = expr;
}

ArrayCreation::ArrayCreation(TypeName* typeName, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->id = ++maxId;
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
	this->id = ++maxId;
	this->left = expr;
	this->argumentList = arguments;
}

InvocationExpression::InvocationExpression(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_INVOCATION)
{
	this->id = ++maxId;
	this->left = expr;
	this->argumentList = arguments;
}

VarDeclarator::VarDeclarator(SimpleType* simpletype, Expression* expression)
{
	this->id = ++maxId;
	this->simpleType = simpletype;
	this->initializer = expression;
}

VarDeclarator::VarDeclarator(TypeName* typeName, Expression* expression)
{
	this->id = ++maxId;
	this->typeName = typeName;
	this->initializer = expression;
}

VarDeclarator::VarDeclarator(ArrayType* arraytype, Expression* expression)
{
	this->id = ++maxId;
	this->arrayType = arraytype;
	this->initializer = expression;
}

void VarDeclarator::AddInitializer(VarDeclarator* declarator, Expression* expression)
{
	////declarator->declarator->push_back(expression);??????????????????????
	////////////////////////////////////////////////////////////
}

VarDeclaratorList::VarDeclaratorList(VarDeclarator* declarator, Expression* expression)
{
	this->id = ++maxId;
	this->declarators = new list<VarDeclarator*>{ declarator };
	//this->declarators->push_back(declarator);
	this->type = type;
}

void VarDeclaratorList::Append(VarDeclaratorList* declarators, Expression* expression)
{
	declarators->declarators->push_back(expression);/////////////////////////
}

Statement::Statement(Type type, Expression* expression)
{
	this->id = ++maxId;
	this->type = type;
	this->left = expression;
}

Statement::Statement(Type type, VarDeclaratorList* declarators)
{
	this->id = ++maxId;
	this->type = type;
	this->declarators = declarators;
}

Statement::Statement(Type type, StatementList* statements)
{
	this->id = ++maxId;
	this->type = type;
	this->statements = statements;
}

StatementList::StatementList(Statement* statement)
{
	this->id = ++maxId;
	this->statements = new list <Statement*>{ statement };
}

void StatementList::Append(StatementList * statements, Statement* statement)
{
	statements->statements->push_back(statement);
}

IfStatement::IfStatement(Expression* expression, 
	Statement* main, Statement* alternative) : Statement(Statement::t_IF)
{
	this->id = ++maxId;
	this->left = expression;
	this->main = main;//////////////////////
	this->alternative = alternative;//////////////////////

}

WhileStatement::WhileStatement(Expression* expression, 
	Statement* statement) : Statement(Statement::t_WHILE)
{
	this->id = ++maxId;
	this->left = expression;
	this->statement = statement;///////////////////////
}

DoStatement::DoStatement(Statement* statement, 
	Expression* expression) : Statement(Statement::t_DO)
{
	this->id = ++maxId;
}

ForeachStatement::ForeachStatement(VarDeclarator* declarator, 
	Expression* expression, Statement* statement) : Statement(Statement::t_FOREACH)
{
	this->id = ++maxId;
	////////////
}

ForStatement::ForStatement(Expression* init, 
	Expression* cond, Expression* increment, Statement* statement) : Statement(Statement::t_FOR)
{
	this->id = ++maxId;
	/////////////
}

ForStatement::ForStatement(VarDeclaratorList* declarations, 
	Expression* cond, Expression* increment, Statement* statement) : Statement(Statement::t_FOR)
{
	this->id = ++maxId;
	/////////////
}

ReturnStatement::ReturnStatement(Expression* expression) : Statement(Statement::t_RETURN, expression)
{
	this->id = ++maxId;
	this->left = expression;
}

ParamList::ParamList(VarDeclarator* param)
{
	this->id = ++maxId;
	this->params = new list < VarDeclarator*>{ param };
}

void ParamList::Append(ParamList* params, VarDeclarator* param)
{
	params->params->push_back(param);
}

Modifier::Modifier(Type type)
{
	this->id = ++maxId;
	this->type = type;
}

ModifielrList::ModifielrList(Modifier* modifier)
{
	this->id = ++maxId;
	this->modifiers = new list < Modifier*>{ modifier };
}

void ModifielrList::Append(ModifielrList* modifiers, Modifier* modifier)
{
	modifiers->modifiers->push_back(modifier);
}

ClassMember::ClassMember(Type type, ReturnValueType returnValue, BaseConstructorType baseConstructor)
{
	this->id = ++maxId;
	this->type = type;
	this->returnValue = returnValue;
	this->baseConstructor = baseConstructor;
}

ClassMemberList::ClassMemberList(ClassMember* member)
{
	this->id = ++maxId;
	this->members = new list < ClassMember*>{ member };
}

void ClassMemberList::Append(ClassMemberList *members, ClassMember* member)
{
	members->members->push_back(member);
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifiers;
	this->paramList = paramList;
	this->statementList = statements;

}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	SimpleType* simpleType, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifiers;
	this->simpleType = simpleType;
	this->paramList = params;
	this->statementList = statements;
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	TypeName* typeName, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifiers;
	this->typeName = typeName;
	this->paramList = params;
	this->statementList = statements;
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	ArrayType* arrayType, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifiers;
	this->arrayType = arrayType;
	this->paramList = params;
	this->statementList = statements;
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	SimpleType* simpleType, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifier;
	this->simpleType = simpleType;
	this->expression = expression;
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	TypeName* typeName, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifier;
	this->typeName = typeName;
	this->expression = expression;
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	ArrayType* arraType, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->returnValue = returnValue;
	this->identifier = identifier;
	this->arrayType = arraType;
	this->expression = expression;
}

Constructor::Constructor(ModifielrList* modifiers, string* identifier, 
	ParamList* params, BaseConstructorType baseConstructor, StatementList* statements, ArgumentList* args)
	: ClassMember(ClassMember::t_CONSTRUCTOR, ClassMember::t_EMPTY, baseConstructor)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->baseConstructor = baseConstructor;
	this->identifier = identifier;
	this->statementList = statements;
	this->paramList = params;
	this->argumentList = args;
}

ClassDeclaration::ClassDeclaration(ModifielrList* modifiers, 
	string* identifier, ClassMemberList* members, TypeName* baseClass)
	: ClassMember(ClassMember::t_CLASS, ClassMember::t_EMPTY, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->classMemberList = members;
	this->identifier = identifier;
	this->typeName = baseClass;
}

NamespaceMember::NamespaceMember(ClassDeclaration* decl)
{
	this->id = ++maxId;
	this->classDecl = decl;
}

NamespaceMember::NamespaceMember(NamespaceDeclaration* decl)
{
	this->id = ++maxId;
	this->namespaceDecl = decl;
}

NamespaceMemberList::NamespaceMemberList(NamespaceMember* member)
{
	this->id = ++maxId;
	this->members = new list < NamespaceMember*>{ member };
}

void NamespaceMemberList::Append(NamespaceMemberList* members, NamespaceMember* member)
{
	members->members->push_back(member);
}

NamespaceDeclaration::NamespaceDeclaration(TypeName* typeName, NamespaceMemberList* members)
{
	this->id = ++maxId;
	this->typeName = typeName;
	this->members = members;
}

Programm::Programm(NamespaceMemberList* members)
{
	this->id = ++maxId;
	this->members = members;
}
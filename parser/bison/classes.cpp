#include "classes.h"

int maxId = 0;

SimpleType::SimpleType(Type type)
{
	this->id = ++maxId;
	this->type = type; 
}

string* SimpleType::ToDOT()
{
	string* dotStr = GetName();
	*dotStr = to_string(id) + "[label=\"" + *dotStr + "\"];\n";
	return dotStr;
}

string* SimpleType::GetName()
{
	switch (this->type)
	{
	case SimpleType::t_BOOL:   return new string("bool");
	case SimpleType::t_INT:    return new string("int");
	case SimpleType::t_CHAR:   return new string("char");
	case SimpleType::t_STRING: return new string("string"); 
	}
	return NULL;
}

TypeName::TypeName(string* identifier)
{
	this->id = ++maxId;
	this->identifiers = new list <string*>{ identifier };
}

TypeName* TypeName::Append(TypeName* typeName, string* identifier)
{
	typeName->identifiers->push_back(identifier);
	return typeName;
}

string* TypeName::ToDOT()
{
	string* dotStr = new string();
	int n = identifiers->size() - 1;
	for (auto i = identifiers->rbegin(); i != identifiers->rend(); i++, n--)
	{
		string strId = to_string(id);
		if (n != 0)
		{
			strId += "." + to_string(n);
		}

		*dotStr += strId + "[label=\"" + **i + "\"];\n";
		if (i != identifiers->rbegin())
		{
			string strPreviousId = to_string(id) + "." + to_string(n + 1);
			*dotStr += strId + "->" + strPreviousId + "[label=\"next\"];\n";
		}
	}
	return dotStr;
}

ArrayType::ArrayType(SimpleType* simpleType)
{
	this->id = ++maxId;
	this->type = ArrayType::t_SIMPLE_TYPE;
	this->simpleType = simpleType;
}

ArrayType::ArrayType(TypeName* typeName)
{
	this->id = ++maxId;
	this->type = ArrayType::t_TYPE_NAME;
	this->typeName = typeName;
}

string* ArrayType::ToDOT()
{
	string* dotStr = NULL;
	string childId;
	switch (this->type)
	{
	case ArrayType::t_SIMPLE_TYPE:
		childId = to_string(simpleType->id);
		dotStr = simpleType->ToDOT();
		break;
	case ArrayType::t_TYPE_NAME:
		childId = to_string(typeName->id);
		dotStr = typeName->ToDOT();
		break;
	}
	*dotStr += to_string(id) + "[label=\"array\"];\n";
	*dotStr += to_string(id) + "->" + childId + "[label=\"type\"];\n";
	return dotStr;
}

Argument::Argument(Expression* expression, string* identifier)
{
	this->id = ++maxId;
	this->expression = expression;
	this->identifier = identifier;
}

string* Argument::ToDOT()
{
	string* dotStr = expression->ToDOT();
	*dotStr += to_string(id) + "[label=\"argm\"];\n";
	*dotStr += to_string(id) + "->" + to_string(expression->id) + "[label=\"expr\"];\n";
	if (identifier != NULL)
	{
		string identifierId = to_string(id) + ".1";
		*dotStr += identifierId + "[label=\"" + *identifier + "\"];\n";
		*dotStr += to_string(id) + "->" + identifierId + "[label=\"id\"];\n";
	}
	return dotStr;
}

ArgumentList::ArgumentList(Argument* argument)
{
	this->id = argument->id;
	this->arguments = new list <Argument*>{ argument };
}

ArgumentList* ArgumentList::Append(ArgumentList* list, Argument* argument)
{
	list->arguments->push_back(argument);
	return list;
}

string* ArgumentList::ToDOT()
{
	string* dotStr = new string();
	Argument* previous = NULL;
	for (auto i = arguments->begin(); i != arguments->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

ObjectInitializer::ObjectInitializer(MemberInitializerList* initializers)
{
	if (initializers != NULL)
	{
		this->id = initializers->id;
		this->initializers = initializers;
	}
	else
	{
		this->id = ++maxId;
	}
}

string* ObjectInitializer::ToDOT()
{
	if (initializers != NULL)
	{
		return initializers->ToDOT();
	}
	else
	{
		string* dotStr = new string();
		*dotStr += to_string(id) + "[label=\"empty\"];\n";
		return dotStr;
	}
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
	string memberId = "";
	string* memberDot = new string();
	if (identifier != NULL)
	{
		memberId = to_string(id) + ".1";
		*memberDot += memberId + "[label=\"" + *identifier + "\"];\n";
	}
	else
	{
		memberId = to_string(argumentList->id);
		memberDot = argumentList->ToDOT();
	}

	string initId = "";
	string* initDot = NULL;
	if (expression != NULL)
	{
		initId = to_string(expression->id);
		initDot = expression->ToDOT();
	}
	else
	{
		initId = objectInitializer->id;
		initDot = objectInitializer->ToDOT();
	}

	string* dotStr = new string();
	*dotStr += *memberDot;
	*dotStr += *initDot;
	*dotStr += to_string(id) + "[label=\"memberInit\"];\n";
	*dotStr += to_string(id) + "->" + memberId + "[label=\"member\"];\n";
	*dotStr += to_string(id) + "->" + initId + "[label=\"initializer\"];\n";
	return dotStr;
}

MemberInitializerList::MemberInitializerList(MemberInitializer* memberInitializer)
{
	this->id = memberInitializer->id;
	this->initializers = new list <MemberInitializer*>{ memberInitializer };
}

MemberInitializerList* MemberInitializerList::Append(MemberInitializerList* list, MemberInitializer* memberInitializer)
{
	list->initializers->push_back(memberInitializer);
	return list;
}

string* MemberInitializerList::ToDOT()
{
	string* dotStr = new string();
	MemberInitializer* previous = NULL;
	for (auto i = initializers->begin(); i != initializers->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

Expression::Expression(Type type, string* name)
{
	this->id = ++maxId;
	this->type = type;
	this->name = name;
}

Expression::Expression(Type type, int intLiteral)
{
	this->id = ++maxId;
	this->type = Expression::t_INT_LITER;
	this->intLiteral = intLiteral;
}

Expression::Expression(Type type, char charLiteral)
{
	this->id = ++maxId;
	this->type = Expression::t_CHAR_LITER;
	this->charLiteral = charLiteral;
}

Expression::Expression(Type type, bool boolLiteral)
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
	string* dotStr = GetName();
	*dotStr = to_string(id) + "[label=\"" + *dotStr + "\"];\n";

	if (left != NULL)
	{
		*dotStr += *left->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(left->id) + "[label=\"left\"];\n";
	}
	if (right != NULL)
	{
		*dotStr += *right->ToDOT();;
		*dotStr += to_string(id) + "->" + to_string(right->id) + "[label=\"right\"];\n";
	}
	if (type != Expression::t_SIMPLE_TYPE && simpleType != NULL)
	{
		*dotStr += *simpleType->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(simpleType->id) + "[label=\"type\"];\n";
	}
	if (arrayType != NULL)
	{
		*dotStr += *arrayType->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(arrayType->id) + "[label=\"type\"];\n";
	}
	if (typeName != NULL)
	{
		*dotStr += *typeName->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(typeName->id) + "[label=\"type\"];\n";
	}

	return dotStr;
}

string* Expression::GetName()
{
	switch (type)
	{
	case Expression::t_INT_LITER: return new string(to_string(intLiteral));
	case Expression::t_CHAR_LITER: return new string("\'" + string(1, charLiteral) + "\'");
	case Expression::t_BOOL_LITER: return new string(to_string(boolLiteral));
	case Expression::t_STRING_LITER: return new string("\\\"" + *name + "\\\"");
	case Expression::t_ID: return name;
	case Expression::t_SIMPLE_TYPE: simpleType->GetName();
	case Expression::t_THIS: return new string("this");
	case Expression::t_BASE: return new string("base");
	case Expression::t_MEMBER_ACCESS: return new string(".");
	case Expression::t_PARENTHESIZED: return new string("(expr)");
	case Expression::t_UNMINUS: return new string("-");
	case Expression::t_NOT: return new string("!");
	case Expression::t_SIMPLE_TYPE_CAST: return new string("cast");
	case Expression::t_ARRAY_CAST: return new string("cast");
	case Expression::t_TYPENAME_CAST: return new string("cast");
	case Expression::t_MUL: return new string("*");
	case Expression::t_DIV: return new string("/");
	case Expression::t_MOD: return new string("%");
	case Expression::t_SUM: return new string("+");
	case Expression::t_SUB: return new string("-");
	case Expression::t_LESS: return new string("<");
	case Expression::t_GREATER: return new string(">");
	case Expression::t_LESS_EQUAL: return new string("<=");
	case Expression::t_GREATER_EQUAL: return new string(">=");
	case Expression::t_IS: return new string("is");
	case Expression::t_AS: return new string("as");
	case Expression::t_EQUALITY: return new string("==");
	case Expression::t_INEQUALITY: return new string("!=");
	case Expression::t_AND: return new string("&&");
	case Expression::t_OR: return new string("||");
	case Expression::t_ASSIGNMENT: return new string("=");
	default: return NULL;
	}
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

string* ObjectCreation::ToDOT()
{
	string typeId;
	string* dotStr;
	if (simpleType != NULL)
	{
		typeId = to_string(simpleType->id);
		dotStr = simpleType->ToDOT();
	}
	else
	{
		typeId = to_string(typeName->id);
		dotStr = typeName->ToDOT();
	}
	*dotStr += to_string(id) + "[label=\"objCreation\"];\n";
	*dotStr += to_string(id) + "->" + typeId + "[label=\"type\"];\n";

	if (argumentList != NULL)
	{
		*dotStr += *argumentList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(argumentList->id) + "[label=\"args\"];\n";
	}

	if (objInitializer != NULL)
	{
		*dotStr += *objInitializer->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(objInitializer->id) + "[label=\"init\"];\n";
	}

	return dotStr;
}

ExpressionList::ExpressionList(Expression* expression)
{
	this->id = expression->id;
	this->expressions = new list <Expression*>{ expression };
}

ExpressionList* ExpressionList::Append(ExpressionList* list, Expression* expression)
{
	list->expressions->push_back(expression);
	return list;
}

string* ExpressionList::ToDOT()
{
	string* dotStr = new string();
	Expression* previous = NULL;
	for (auto i = expressions->begin(); i != expressions->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

ArrayInitializer::ArrayInitializer(ExpressionList* expressions)
{
	this->id = ++maxId;
	this->expressions = expressions;
}

string* ArrayInitializer::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + "[label=\"arrInit\"];\n";
	if (expressions != NULL)
	{
		*dotStr += *expressions->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(expressions->id) + "[label=\"expressions\"];\n";
	}
	return dotStr;
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
	this->arrayInitializer = arrInit;
}

ArrayCreation::ArrayCreation(TypeName* typeName, 
	Expression* expr, ArrayInitializer* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->id = ++maxId;
	this->typeName = typeName;
	this->left = expr;
	this->arrayInitializer = arrInit;
}

string* ArrayCreation::ToDOT()
{
	string typeId;
	string* dotStr;
	if (arrayType != NULL)
	{
		typeId = to_string(arrayType->id);
		dotStr = arrayType->ToDOT();
	}
	else if (simpleType != NULL)
	{
		typeId = to_string(simpleType->id);
		dotStr = simpleType->ToDOT();
	}
	else
	{
		typeId = to_string(typeName->id);
		dotStr = typeName->ToDOT();
	}
	*dotStr += to_string(id) + "[label=\"arrCreation\"];\n";
	*dotStr += to_string(id) + "->" + typeId + "[label=\"type\"];\n";

	if (left != NULL)
	{
		*dotStr += *left->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(left->id) + "[label=\"size\"];\n";
	}

	if (arrayInitializer != NULL)
	{
		*dotStr += *arrayInitializer->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(arrayInitializer->id) + "[label=\"arrInit\"];\n";
	}

	return dotStr;
}

Expression* MemberAccess::FromTypeName(TypeName* typeName, Expression* left)
{
	for (string* s : *typeName->identifiers)
	{
		if (left == NULL)
		{
			left = new Expression(Expression::t_ID, s);
		}
		else
		{
			left = new Expression(Expression::t_MEMBER_ACCESS, left, new Expression(Expression::t_ID, s));
		}
	}
	return left;
}

ElementAccess::ElementAccess(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_ELEMENT_ACCESS)
{
	this->id = ++maxId;
	this->left = expr;
	this->argumentList = arguments;
}

string* ElementAccess::ToDOT()
{
	string* dotStr = left->ToDOT();
	*dotStr += *argumentList->ToDOT();
	*dotStr += to_string(id) + "[label\"elemAccess\"];\n";
	*dotStr += to_string(id) + "->" + to_string(left->id) + "[label=\"expr\"];\n";
	*dotStr += to_string(id) + "->" + to_string(argumentList->id) + "[label=\"args\"];\n";
	return dotStr;
}

InvocationExpression::InvocationExpression(Expression* expr, 
	ArgumentList* arguments) : Expression(Expression::t_INVOCATION)
{
	this->id = ++maxId;
	this->left = expr;
	this->argumentList = arguments;
}

string* InvocationExpression::ToDOT()
{
	string* dotStr = left->ToDOT();
	*dotStr += to_string(id) + "[label=\"invocation\"];\n";
	*dotStr += to_string(id) + "->" + to_string(left->id) + "[label=\"expr\"];\n";

	if (argumentList != NULL)
	{
		*dotStr += *argumentList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(argumentList->id) + "[label=\"args\"];\n";
	}

	return dotStr;
}

VarDeclarator::VarDeclarator(SimpleType* simpletype, string* identifier, Expression* expression)
{
	this->type = VarDeclarator::t_SIMPLE_TYPE;
	this->id = ++maxId;
	this->simpleType = simpletype;
	this->identifier = identifier;
	this->initializer = expression;
}

VarDeclarator::VarDeclarator(TypeName* typeName, string* identifier, Expression* expression)
{
	this->type = VarDeclarator::t_TYPE_NAME;
	this->id = ++maxId;
	this->typeName = typeName;
	this->identifier = identifier;
	this->initializer = expression;
}

VarDeclarator::VarDeclarator(ArrayType* arraytype, string* identifier, Expression* expression)
{
	this->type = VarDeclarator::t_ARRAY_TYPE;
	this->id = ++maxId;
	this->arrayType = arraytype;
	this->identifier = identifier;
	this->initializer = expression;
}

string* VarDeclarator::ToDOT()
{
	string typeId;
	string* dotStr;
	if (type == VarDeclarator::t_ARRAY_TYPE)
	{
		typeId = to_string(arrayType->id);
		dotStr = arrayType->ToDOT();
	}
	else if (type == VarDeclarator::t_SIMPLE_TYPE)
	{
		typeId = to_string(simpleType->id);
		dotStr = simpleType->ToDOT();
	}
	else
	{
		typeId = to_string(typeName->id);
		dotStr = typeName->ToDOT();
	}
	*dotStr += to_string(id) + ".1[label=\"" + *identifier + "\"];\n";
	*dotStr += to_string(id) + "[label=\"varDecl\"];\n";
	*dotStr += to_string(id) + "->" + typeId + "[label=\"type\"];\n";
	*dotStr += to_string(id) + "->" + to_string(id) + ".1[label=\"identifier\"];\n";

	if (initializer != NULL)
	{
		*dotStr += *initializer->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(initializer->id) + "[label=\"init\"];\n";
	}

	return dotStr;
}

VarDeclaratorList::VarDeclaratorList(VarDeclarator* declarator, Expression* expression)
{
	declarator->initializer = expression;
	this->id = declarator->id;
	this->type = declarator->type;
	this->declarators = new list<VarDeclarator*>{ declarator };
}

VarDeclaratorList* VarDeclaratorList::Append(VarDeclaratorList* declarators, string* identifier, Expression* expression)
{
	VarDeclarator* varDecl = NULL;
	switch (declarators->type)
	{
	case VarDeclarator::t_SIMPLE_TYPE:
		varDecl = new VarDeclarator(declarators->declarators->front()->simpleType, identifier, expression);
		break;
	case VarDeclarator::t_ARRAY_TYPE:
		varDecl = new VarDeclarator(declarators->declarators->front()->arrayType, identifier, expression);
		break;
	case VarDeclarator::t_TYPE_NAME:
		varDecl = new VarDeclarator(declarators->declarators->front()->typeName, identifier, expression);
		break;
	}
	declarators->declarators->push_back(varDecl);
	return declarators;
}

string* VarDeclaratorList::ToDOT()
{
	string* dotStr = new string();
	VarDeclarator* previous = NULL;
	for (auto i = declarators->begin(); i != declarators->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

Statement::Statement(Type type, Expression* expression)
{
	this->id = ++maxId;
	this->type = type;
	if (expression != NULL)
	{
		this->expressions = new ExpressionList(expression);
	}
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

string* Statement::ToDOT()
{
	string childId;
	string exprName;
	string* dotStr = NULL;
	switch (type)
	{
	case Statement::t_EXPRESSION:
		exprName = ";";
		childId = to_string(expressions->id);
		dotStr = expressions->ToDOT();
		break;
	case Statement::t_DECLARATOR:
		exprName = ";";
		childId = to_string(declarators->id);
		dotStr = declarators->ToDOT();
		break;
	case Statement::t_BLOCK:
		exprName = "block";
		childId = to_string(statements->id);
		dotStr = statements->ToDOT();
		break;
	}

	if (dotStr != NULL)
	{
		*dotStr += to_string(id) + "[label=\"" + exprName + "\"];\n";
		*dotStr += to_string(id) + "->" + childId + "[label=\"\"];\n";
	}
	else
	{
		dotStr = new string();
		*dotStr += to_string(id) + "[label=\";\"];\n";
	}

	return dotStr;
}

StatementList::StatementList(Statement* statement)
{
	this->id = statement->id;
	this->statements = new list <Statement*>{ statement };
}

StatementList* StatementList::Append(StatementList * statements, Statement* statement)
{
	statements->statements->push_back(statement);
	return statements;
}

string* StatementList::ToDOT()
{
	string* dotStr = new string();
	Statement* previous = NULL;
	for (auto i = statements->begin(); i != statements->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

IfStatement::IfStatement(Expression* expression, 
	Statement* main, Statement* alternative) : Statement(Statement::t_IF)
{
	this->id = ++maxId;
	this->expressions = new ExpressionList(expression);
	this->statements = new StatementList(main);

	if (alternative != NULL)
	{
		this->statements = StatementList::Append(this->statements, alternative);
	}
}

string* IfStatement::ToDOT()
{
	Expression* cond = expressions->expressions->front();
	Statement* main = statements->statements->front();
	Statement* alternative = NULL;
	if (statements->statements->size() > 1)
	{
		alternative = statements->statements->back();
	}

	string* dotStr = cond->ToDOT();
	*dotStr += *main->ToDOT();
	*dotStr += to_string(id) + "[label=\"if_stmt\"];\n";
	*dotStr += to_string(id) + "->" + to_string(cond->id) + "[label=\"cond\"];\n";
	*dotStr += to_string(id) + "->" + to_string(main->id) + "[label=\"if\"];\n";

	if (alternative != NULL)
	{
		*dotStr += *alternative->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(alternative->id) + "[label=\"else\"];\n";
	}

	return dotStr;
}

WhileStatement::WhileStatement(Expression* expression, 
	Statement* statement) : Statement(Statement::t_WHILE)
{
	this->id = ++maxId;
	this->expressions = new ExpressionList(expression);
	this->statements = new StatementList(statement);
}

string* WhileStatement::ToDOT()
{
	Expression* cond = expressions->expressions->front();
	Statement* stmt = statements->statements->front();

	string* dotStr = cond->ToDOT();
	*dotStr += *stmt->ToDOT();
	*dotStr += to_string(id) + "[label=\"while_stmt\"];\n";
	*dotStr += to_string(id) + "->" + to_string(cond->id) + "[label=\"cond\"];\n";
	*dotStr += to_string(id) + "->" + to_string(stmt->id) + "[label=\"stmt\"];\n";

	return dotStr;
}

DoStatement::DoStatement(Statement* statement, 
	Expression* expression) : Statement(Statement::t_DO)
{
	this->id = ++maxId;
	this->expressions = new ExpressionList(expression);
	this->statements = new StatementList(statement);
}

string* DoStatement::ToDOT()
{
	Expression* cond = expressions->expressions->front();
	Statement* stmt = statements->statements->front();

	string* dotStr = cond->ToDOT();
	*dotStr += *stmt->ToDOT();
	*dotStr += to_string(id) + "[label=\"do_stmt\"];\n";
	*dotStr += to_string(id) + "->" + to_string(cond->id) + "[label=\"cond\"];\n";
	*dotStr += to_string(id) + "->" + to_string(stmt->id) + "[label=\"stmt\"];\n";

	return dotStr;
}

ForeachStatement::ForeachStatement(VarDeclarator* declarator, 
	Expression* expression, Statement* statement) : Statement(Statement::t_FOREACH)
{
	this->id = ++maxId;
	this->declarators = new VarDeclaratorList(declarator);
	this->expressions = new ExpressionList(expression);
	this->statements = new StatementList(statement);
}

string* ForeachStatement::ToDOT()
{
	VarDeclarator* decl = declarators->declarators->front();
	Expression* cond = expressions->expressions->front();
	Statement* stmt = statements->statements->front();

	string* dotStr = cond->ToDOT();
	*dotStr += *stmt->ToDOT();
	*dotStr += to_string(id) + "[label=\"foreach_stmt\"];\n";
	*dotStr += to_string(id) + "->" + to_string(decl->id) + "[label=\"decl\"];\n";
	*dotStr += to_string(id) + "->" + to_string(cond->id) + "[label=\"cond\"];\n";
	*dotStr += to_string(id) + "->" + to_string(stmt->id) + "[label=\"stmt\"];\n";

	return dotStr;
}

ForStatement::ForStatement(Expression* init, 
	Expression* cond, Expression* increment, Statement* statement) : Statement(Statement::t_FOR)
{
	this->id = ++maxId;
	this->expressions = new ExpressionList(init);
	ExpressionList::Append(this->expressions, cond);
	ExpressionList::Append(this->expressions, increment);
	this->statements = new StatementList(statement);
}

ForStatement::ForStatement(VarDeclaratorList* declarations, 
	Expression* cond, Expression* increment, Statement* statement) : Statement(Statement::t_FOR)
{
	this->id = ++maxId;
	this->declarators = declarations;
	this->expressions = new ExpressionList(cond);
	ExpressionList::Append(this->expressions, increment);
	this->statements = new StatementList(statement);
}

string* ForStatement::ToDOT()
{
	VarDeclaratorList* decls = NULL;
	Expression* init = NULL;
	Expression* cond = NULL;
	Expression* incr = NULL;
	Statement*  stmt = NULL;

	if (declarators != NULL)
	{
		decls = declarators;
		cond = expressions->expressions->front();
		incr = expressions->expressions->back();
	}
	else
	{
		auto iter = expressions->expressions->begin();
		init = *iter; iter++;
		cond = *iter; iter++;
		incr = *iter;
	}
	stmt = statements->statements->front();

	string initID;
	string* dotStr;
	if (decls != NULL)
	{
		initID = to_string(decls->id);
		dotStr = decls->ToDOT();
	}
	else
	{
		initID = to_string(init->id);
		dotStr = init->ToDOT();
	}

	*dotStr += *cond->ToDOT();
	*dotStr += *incr->ToDOT();
	*dotStr += *stmt->ToDOT();

	*dotStr += to_string(id) + "[label=\"for_stmt\"];\n";
	*dotStr += to_string(id) + "->" + initID + "[label=\"init\"];\n";
	*dotStr += to_string(id) + "->" + to_string(cond->id) + "[label=\"cond\"];\n";
	*dotStr += to_string(id) + "->" + to_string(incr->id) + "[label=\"incr\"];\n";
	*dotStr += to_string(id) + "->" + to_string(stmt->id) + "[label=\"stmt\"];\n";

	return dotStr;
}

ReturnStatement::ReturnStatement(Expression* expression) : Statement(Statement::t_RETURN, expression)
{
	this->id = ++maxId;
	this->expressions = new ExpressionList(expression);
}

string* ReturnStatement::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + "[label=\"return\"];\n";
	if (expressions != NULL)
	{
		*dotStr += *expressions->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(expressions->id) + "[label=\"expr\"];\n";
	}

	return dotStr;
}

ParamList::ParamList(VarDeclarator* param)
{
	this->id = param->id;
	this->params = new list < VarDeclarator*>{ param };
}

ParamList* ParamList::Append(ParamList* params, VarDeclarator* param)
{
	params->params->push_back(param);
	return params;
}

string* ParamList::ToDOT()
{
	string* dotStr = new string();
	VarDeclarator* previous = NULL;
	for (auto i = params->begin(); i != params->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

Modifier::Modifier(Type type)
{
	this->id = ++maxId;
	this->type = type;
}

string* Modifier::ToDOT()
{
	string name;
	switch (type)
	{
	case Modifier::t_PRIVATE:   
		name = "private";
		break;
	case Modifier::t_PROTECTED: 
		name = "protected";
		break;
	case Modifier::t_PUBLIC:    
		name = "public";
		break;
	case Modifier::t_INTERNAL:  
		name = "internal";
		break;
	case Modifier::t_ABSTRACT:  
		name = "abstract";
		break;
	case Modifier::t_STATIC:   
		name = "static";
		break;
	case Modifier::t_OVERRIDE:  
		name = "override";
		break;
	case Modifier::t_VIRTUAL:   
		name = "virtual";
		break;
	}
	
	string* dotStr = new string();
	*dotStr = to_string(id) + "[label=\"" + name + "\"];\n";
	return dotStr;
}

ModifielrList::ModifielrList(Modifier* modifier)
{
	this->id = modifier->id;
	this->modifiers = new list < Modifier*>{ modifier };
}

ModifielrList* ModifielrList::Append(ModifielrList* modifiers, Modifier* modifier)
{
	modifiers->modifiers->push_back(modifier);
	return modifiers;
}

string* ModifielrList::ToDOT()
{
	string* dotStr = new string();
	Modifier* previous = NULL;
	for (auto i = modifiers->rbegin(); i != modifiers->rend(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string((*i)->id) + "->" + to_string(previous->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

ClassMember::ClassMember(Type type, ReturnValueType returnValue, BaseConstructorType baseConstructor)
{
	this->id = ++maxId;
	this->type = type;
	this->returnValue = returnValue;
	this->baseConstructor = baseConstructor;
}

string* ClassMember::ToDOT() 
{
	return NULL;
}

ClassMemberList::ClassMemberList(ClassMember* member)
{
	this->id = member->id;
	this->members = new list < ClassMember*>{ member };
}

ClassMemberList* ClassMemberList::Append(ClassMemberList *members, ClassMember* member)
{
	members->members->push_back(member);
	return members;
}

string* ClassMemberList::ToDOT()
{
	string* dotStr = new string();
	ClassMember* previous = NULL;
	for (auto i = members->begin(); i != members->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->identifier = identifiers;
	this->paramList = params;
	this->statementList = statements;

}

Method::Method(ModifielrList* modifiers, ReturnValueType returnValue, 
	SimpleType* simpleType, string* identifiers, ParamList* params, StatementList* statements)
	: ClassMember(ClassMember::t_METHOD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
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
	this->identifier = identifiers;
	this->arrayType = arrayType;
	this->paramList = params;
	this->statementList = statements;
}

string* Method::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + ".1[label=\"" + *identifier + "\"];\n";
	*dotStr += to_string(id) + "[label=\"method\"];\n";
	*dotStr += to_string(id) + "->" + to_string(id) + ".1[label=\"id\"];\n";

	if (modifiers != NULL) 
	{
		*dotStr += *modifiers->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(modifiers->id) + "[label=\"modifs\"];\n";
	}

	if (returnValue == Method::t_SIMPLE_TYPE)
	{
		*dotStr += *simpleType->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(simpleType->id) + "[label=\"type\"];\n";
	}
	else if (returnValue == Method::t_TYPENAME)
	{
		*dotStr += *typeName->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(typeName->id) + "[label=\"type\"];\n";
	}
	else if (returnValue == Method::t_ARRAY)
	{
		*dotStr += *arrayType->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(arrayType->id) + "[label=\"type\"];\n";
	}
	else
	{
		*dotStr += to_string(id) + ".2[label=\"void\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".2[label=\"type\"];\n";
	}

	if (paramList != NULL)
	{
		*dotStr += *paramList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(paramList->id) + "[label=\"params\"];\n";
	}

	if (statementList != NULL)
	{
		*dotStr += *statementList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(statementList->id) + "[label=\"body\"];\n";
	}

	return dotStr;
}

Field::Field(ModifielrList* modifiers, ReturnValueType returnValue, 
	SimpleType* simpleType, string* identifier, Expression* expression)
	: ClassMember(ClassMember::t_FIELD, returnValue, ClassMember::t_NULL)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
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
	this->identifier = identifier;
	this->arrayType = arraType;
	this->expression = expression;
}

string* Field::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + ".1[label=\"" + *identifier + "\"];\n";
	*dotStr += to_string(id) + "[label=\"field\"];\n";
	*dotStr += to_string(id) + "->" + to_string(id) + ".1[label=\"id\"];\n";

	if (modifiers != NULL)
	{
		*dotStr += *modifiers->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(modifiers->id) + "[label=\"modifs\"];\n";
	}

	if (returnValue == Method::t_SIMPLE_TYPE)
	{
		*dotStr += *simpleType->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(simpleType->id) + "[label=\"type\"];\n";
	}
	else if (returnValue == Method::t_TYPENAME)
	{
		*dotStr += *typeName->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(typeName->id) + "[label=\"type\"];\n";
	}
	else if (returnValue == Method::t_ARRAY)
	{
		*dotStr += *arrayType->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(arrayType->id) + "[label=\"type\"];\n";
	}

	if (expression != NULL)
	{
		*dotStr += *expression->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(expression->id) + "[label=\"init\"];\n";
	}

	return dotStr;
}

Constructor::Constructor(ModifielrList* modifiers, string* identifier, 
	ParamList* params, BaseConstructorType baseConstructor, StatementList* statements, ArgumentList* args)
	: ClassMember(ClassMember::t_CONSTRUCTOR, ClassMember::t_EMPTY, baseConstructor)
{
	this->id = ++maxId;
	this->modifiers = modifiers;
	this->identifier = identifier;
	this->statementList = statements;
	this->paramList = params;
	this->argumentList = args;
}

string* Constructor::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + ".1[label=\"" + *identifier + "\"];\n";
	*dotStr += to_string(id) + "[label=\"constructor\"];\n";
	*dotStr += to_string(id) + "->" + to_string(id) + ".1[label=\"id\"];\n";

	if (modifiers != NULL)
	{
		*dotStr += *modifiers->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(modifiers->id) + "[label=\"modifs\"];\n";
	}

	if (paramList != NULL)
	{
		*dotStr += *paramList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(paramList->id) + "[label=\"params\"];\n";
	}

	if (statementList != NULL)
	{
		*dotStr += *statementList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(statementList->id) + "[label=\"stmt\"];\n";
	}

	if (baseConstructor == Constructor::t_BASE)
	{
		*dotStr += to_string(id) + ".2[label=\"base\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".2[label=\"baseConstruct\"];\n";
	}
	else if (baseConstructor == Constructor::t_THIS)
	{
		*dotStr += to_string(id) + ".2[label=\"this\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".2[label=\"thisConstruct\"];\n";
	}

	if (argumentList != NULL)
	{
		*dotStr += *argumentList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(argumentList->id) + "[label=\"args\"];\n";
	}

	return dotStr;
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

string* ClassDeclaration::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + ".1[label=\"" + *identifier + "\"];\n";
	*dotStr += to_string(id) + "[label=\"class\"];\n";
	*dotStr += to_string(id) + "->" + to_string(id) + ".1[label=\"id\"];\n";

	if (modifiers != NULL)
	{
		*dotStr += *modifiers->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(modifiers->id) + "[label=\"modifs\"];\n";
	}

	if (typeName != NULL)
	{
		*dotStr += *typeName->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(typeName->id) + "[label=\"baseClass\"];\n";
	}

	if (classMemberList != NULL)
	{
		*dotStr += *classMemberList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(classMemberList->id) + "[label=\"body\"];\n";
	}

	return dotStr;
}

NamespaceMember::NamespaceMember(ClassDeclaration* decl)
{
	this->id = decl->id;
	this->type = NamespaceMember::t_CLASS;
	this->classDecl = decl;
}

NamespaceMember::NamespaceMember(NamespaceDeclaration* decl)
{
	this->id = decl->id;
	this->type = NamespaceMember::t_NAMESPACE;
	this->namespaceDecl = decl;
}

string* NamespaceMember::ToDOT()
{
	switch (type)
	{
	case NamespaceMember::t_NAMESPACE: return namespaceDecl->ToDOT();
	case NamespaceMember::t_CLASS:     return classDecl->ToDOT();
	default: return NULL;
	}
}

NamespaceMemberList::NamespaceMemberList(NamespaceMember* member)
{
	this->id = member->id;
	this->members = new list < NamespaceMember*>{ member };
}

NamespaceMemberList* NamespaceMemberList::Append(NamespaceMemberList* members, NamespaceMember* member)
{
	members->members->push_back(member);
	return members;
}

string* NamespaceMemberList::ToDOT()
{
	string* dotStr = new string();
	NamespaceMember* previous = NULL;
	for (auto i = members->begin(); i != members->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		if (previous != NULL)
		{
			*dotStr += to_string(previous->id) + "->" + to_string((*i)->id) + "[label=\"next\"];\n";
		}
		previous = *i;
	}
	return dotStr;
}

NamespaceDeclaration::NamespaceDeclaration(TypeName* typeName, NamespaceMemberList* members)
{
	this->id = ++maxId;
	this->typeName = typeName;
	this->members = members;
}

string* NamespaceDeclaration::ToDOT()
{
	string* dotStr = typeName->ToDOT();
	*dotStr += to_string(id) + "[label=\"namespace\"];\n";
	*dotStr += to_string(id) + "->" + to_string(typeName->id) + "[label=\"name\"];\n";

	if (members != NULL)
	{
		*dotStr += *members->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(members->id) + "[label=\"body\"];\n";
	}

	return dotStr;
}

Programm::Programm(NamespaceMemberList* members)
{
	this->id = ++maxId;
	this->members = members;
}

Programm* Programm::main = NULL;

string* Programm::ToDOT()
{
	return members->ToDOT();
}
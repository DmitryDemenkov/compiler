#include "classes.h"
#include "../Parser/Parser/bytecode.h";

int maxId = 0;

SimpleType::SimpleType(Type type)
{
	this->id = ++maxId;
	this->type = type;
}

SimpleType::SimpleType(SimpleType* other)
{
	this->id = ++maxId;
	this->type = other->type;
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

TypeName::TypeName(TypeName* other)
{
	this->id = ++maxId;
	for (auto identifier : *other->identifiers)
	{
		if (this->identifiers == NULL)
		{
			this->identifiers = new list <string*>{ identifier };
		}
		else
		{
			this->identifiers->push_back(identifier);
		}
	}
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

string TypeName::ToString()
{
	string str = "";
	for (auto identifier : *identifiers)
	{
		str += *identifier + ".";
	}
	str.pop_back();
	return str;
}

TypeName* TypeName::FromClass(Class* classInfo)
{
	TypeName* tName = new TypeName(classInfo->GetName());
	AbstractNamespaceMember* member = classInfo->GetOuterMember();
	while (member != NULL)
	{
		tName->identifiers->push_front(member->GetName());
		member = member->GetOuterMember();
	}
	tName->identifiers->pop_front();
	return tName;
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

ArrayType::ArrayType(ArrayType* other)
{
	this->id = ++maxId;
	this->type = other->type;

	if (this->type == t_SIMPLE_TYPE)
		this->simpleType = new SimpleType(other->simpleType);
	else
		this->typeName = new TypeName(other->typeName);
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

void Argument::DetermineDataType(Class* owner, MethodTable* methodInfo)
{
	this->expression->DetermineDataType(owner, methodInfo);
	if (this->expression->dataType == NULL)
	{
		string err = "There is no such identifier \"" + this->expression->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}
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

void MemberInitializer::CheckFieldAccess(Class* owner, MethodTable* methodInfo, Class* creatingClass, FieldTable* field)
{
	if (field == NULL)
	{
		string err = "Field \"" + *identifier + "\" does not exist in class \"" + creatingClass->GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	if (field->IsStatic())
	{
		string err = "Static fields should be invoked on class";
		throw std::exception(err.c_str());
	}

	switch (field->GetAccessModifier())
	{
	case AccessModifier::e_PRIVATE:
		if (creatingClass->GetFullName() != owner->GetFullName())
		{
			string err = "Field access are not allown";
			throw std::exception(err.c_str());
		}
		break;
	case AccessModifier::e_PROTECTED:
		if (!owner->InstanceOf(creatingClass))
		{
			string err = "Field access are not allown";
			throw std::exception(err.c_str());
		}
		break;
	default:
		break;
	}
}

MemberInitializer::MemberInitializer(string* identifier, Expression* expression)
{
	this->id = ++maxId;
	this->identifier = identifier;
	this->expression = expression;
}

MemberInitializer::MemberInitializer(string* identifier, MemberInitializerList* objectInitializer)
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

MemberInitializer::MemberInitializer(ArgumentList* argumentList, MemberInitializerList* objectInitializer)
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
		initId = to_string(objectInitializer->id);
		initDot = objectInitializer->ToDOT();
	}

	string* dotStr = new string();
	*dotStr += *memberDot;
	*dotStr += *initDot;
	*dotStr += to_string(id) + "[label=\"memberInit\"];\n";
	*dotStr += to_string(id) + "->" + memberId + "[label=\"member\"];\n";
	*dotStr += to_string(id) + "->" + initId + "[label=\"initializer\"];\n";

	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += memberId + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
	}

	return dotStr;
}

void MemberInitializer::DetermineDataType(Class* owner, MethodTable* methodInfo, Class* creatingClass)
{
	FieldTable* fieldInfo = creatingClass->GetField(*identifier);
	CheckFieldAccess(owner, methodInfo, creatingClass, fieldInfo);
	this->dataType = fieldInfo->GetType();

	owner->AppendFieldRefConstant(creatingClass, fieldInfo);

	if (this->objectInitializer != NULL)
	{
		this->expression = new ObjectCreation(TypeName::FromClass(this->dataType->classType), NULL, objectInitializer);
	}
	if (this->expression != NULL)
	{
		this->expression->DetermineDataType(owner, methodInfo);
		if (this->expression->dataType == NULL)
		{
			string err = "There is no such identifier \"" + this->expression->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		if (!(*fieldInfo->GetType() == *expression->dataType))
		{
			string err = "it is not possible to convert " + *expression->dataType->ToString() + " to " + *fieldInfo->GetType()->ToString();
			throw std::exception(err.c_str());
		}
	}
}

int MemberInitializer::ToByteCode(Class* owner, MethodTable* methodInfo, Class* creatingClass, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	expression->ToByteCode(owner, methodInfo, byteCode);
	FieldTable* fieldInfo = creatingClass->GetField(*identifier);
	char* fieldRef = Constant::IntToByteCode(owner->AppendFieldRefConstant(creatingClass, fieldInfo));

	byteCode->push_back(ByteCode::putfield);
	byteCode->push_back(fieldRef[2]);
	byteCode->push_back(fieldRef[3]);

	return byteCode->size() - oldSize;
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

void MemberInitializerList::DetermineDataType(Class* owner, MethodTable* methodInfo, Class* creatingClass)
{
	for (auto objInit : *initializers)
	{
		objInit->DetermineDataType(owner, methodInfo, creatingClass);
	}
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
	if (left != NULL)
	{
		*dotStr += *left->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(left->id) + "[label=\"left\"];\n";
	}
	if (right != NULL)
	{
		*dotStr += *right->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(right->id) + "[label=\"right\"];\n";
	}
	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
	}

	return dotStr;
}

void Expression::DetermineDataType(Class* owner, MethodTable* methodInfo)
{
	switch (type)
	{
	case Expression::t_INT_LITER:
		dataType = new DataType(DataType::t_INT, NULL, false, owner);
		if (this->intLiteral < -128 || this->intLiteral > 127)
		{
			owner->AppendIntegerConstant(this->intLiteral);
		}
		break;
	case Expression::t_CHAR_LITER:
		dataType = new DataType(DataType::t_CHAR, NULL, false, owner);
		break;
	case Expression::t_BOOL_LITER:
		dataType = new DataType(DataType::t_BOOL, NULL, false, owner);
		break;
	case Expression::t_STRING_LITER:
		dataType = new DataType(DataType::t_STRING, NULL, false, owner);
		owner->AppendStringConstant(this->name);
		break;
	case Expression::t_ID:
		dataType = GetDataTypeOfId(owner, methodInfo);
		break;
	case Expression::t_SIMPLE_TYPE:
		dataType = GetDataTypeOfSympleType(owner, methodInfo);
		break;
	case Expression::t_THIS:
		dataType = new DataType(DataType::t_TYPENAME, owner, false, owner);
		break;
	case Expression::t_BASE:
		dataType = new DataType(DataType::t_TYPENAME, owner->GetParent(), false, owner);
		break;
	case Expression::t_OBJ_CREATION:
		dataType = GetDataTypeOfObjectCreation(owner, methodInfo);
		break;
	case Expression::t_ARR_CREATION:
		dataType = GetDataTypeOfArrayCreation(owner, methodInfo);
		break;
	case Expression::t_ELEMENT_ACCESS:
		dataType = GetDataTypeOfElementAccess(owner, methodInfo);
		break;
	case Expression::t_MEMBER_ACCESS:
		dataType = GetDataTypeOfMemberAccess(owner, methodInfo);
		break;
	case Expression::t_INVOCATION:
		dataType = GetDataTypeOfInvocation(owner, methodInfo);
		break;
	case Expression::t_UNMINUS:
		left->DetermineDataType(owner, methodInfo);
		if (left->dataType == NULL)
		{
			string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		dataType = new DataType(left->dataType->type, left->dataType->classType, false, owner);

		if (left->dataType->type != DataType::t_INT && left->dataType->type != DataType::t_CHAR || left->dataType->isArray)
		{
			string err = "Unsupported operator for " + *left->dataType->ToString();
			throw std::exception(err.c_str());
		}
		break;
	case Expression::t_NOT:
		left->DetermineDataType(owner, methodInfo);
		if (left->dataType == NULL)
		{
			string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		dataType = new DataType(DataType::t_BOOL, NULL, false, owner);

		if (left->dataType->type != DataType::t_BOOL || left->dataType->isArray)
		{
			string err = "Unsupported logical operator for " + *left->dataType->ToString();
			throw std::exception(err.c_str());
		}
		break;
	case Expression::t_SIMPLE_TYPE_CAST:
	case Expression::t_ARRAY_CAST:
	case Expression::t_TYPENAME_CAST:
	case Expression::t_AS:
		dataType = GetDataTypeOfTypeCast(owner, methodInfo);
		break;
	case Expression::t_MUL:
	case Expression::t_DIV:
	case Expression::t_MOD:
	case Expression::t_SUM:
	case Expression::t_SUB:
		dataType = GetDataTypeOfArithmetic(owner, methodInfo);
		break;
	case Expression::t_LESS:
	case Expression::t_GREATER:
	case Expression::t_LESS_EQUAL:
	case Expression::t_GREATER_EQUAL:
	case Expression::t_EQUALITY:
	case Expression::t_INEQUALITY:
		dataType = GetDataTypeOfComprasion(owner, methodInfo);
		break;
	case Expression::t_IS:
		left->DetermineDataType(owner, methodInfo);
		if (left->dataType == NULL)
		{
			string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}

		if (this->typeName == NULL)
		{
			string err = "Unsupported \"is\" operator for simple types and arrays";
			throw std::exception(err.c_str());
		}
		owner->FindClass(this->typeName);
		dataType = new DataType(DataType::t_BOOL, NULL, false, owner);

		if (left->dataType->type != DataType::t_TYPENAME || left->dataType->isArray)
		{
			string err = "Unsupported \"is\" operator for " + *left->dataType->ToString();
			throw std::exception(err.c_str());
		}
		break;
	case Expression::t_AND:
	case Expression::t_OR:
		left->DetermineDataType(owner, methodInfo);
		if (left->dataType == NULL)
		{
			string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		right->DetermineDataType(owner, methodInfo);
		if (right->dataType == NULL)
		{
			string err = "There is no such identifier \"" + right->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		dataType = new DataType(DataType::t_BOOL, NULL, false, owner);

		if (!(*left->dataType == *right->dataType) || left->dataType->type != DataType::t_BOOL || left->dataType->isArray)
		{
			string err = "Unsupported logical operators for " + *left->dataType->ToString() + " and " + *right->dataType->ToString();
			throw std::exception(err.c_str());
		}
		break;
	case Expression::t_ASSIGNMENT:
		left->DetermineDataType(owner, methodInfo);
		if (left->dataType == NULL)
		{
			string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		right->DetermineDataType(owner, methodInfo);
		if (right->dataType == NULL)
		{
			string err = "There is no such identifier \"" + right->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		dataType = this->right->dataType;

		if (!(*left->dataType == *right->dataType))
		{
			string err = "it is not possible to convert " + *right->dataType->ToString() + " to " + *left->dataType->ToString();
			throw std::exception(err.c_str());
		}
		break;
	default:
		break;
	}
}

int Expression::ToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	switch (type)
	{
	case Expression::t_INT_LITER:
	case Expression::t_CHAR_LITER:
	case Expression::t_BOOL_LITER:
	case Expression::t_STRING_LITER:
		LiteralToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_ID:
		break;
	case Expression::t_SIMPLE_TYPE:
		break;
	case Expression::t_THIS:
	case Expression::t_BASE:
		ThisToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_OBJ_CREATION:
		ObjectCreationToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_ARR_CREATION:
		break;
	case Expression::t_ELEMENT_ACCESS:
		break;
	case Expression::t_MEMBER_ACCESS:
		break;
	case Expression::t_INVOCATION:
		InvokationToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_UNMINUS:
	case Expression::t_MUL:
	case Expression::t_DIV:
	case Expression::t_MOD:
	case Expression::t_SUM:
	case Expression::t_SUB:
		ArithmeticToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_SIMPLE_TYPE_CAST:
		break;
	case Expression::t_ARRAY_CAST:
		break;
	case Expression::t_TYPENAME_CAST:
		break;
	case Expression::t_LESS:
	case Expression::t_GREATER:
	case Expression::t_LESS_EQUAL:
	case Expression::t_GREATER_EQUAL:
	case Expression::t_EQUALITY:
	case Expression::t_INEQUALITY:
		ComprasionToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_NOT:
		NotToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_AND:
		AndToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_OR:
		OrToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_ASSIGNMENT:
		AssigmentToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_IS:
		break;
	case Expression::t_AS:
		break;
	case Expression::t_CLASS:
		break;
	case Expression::t_OBJECT:
		ObjectToByteCode(owner, methodInfo, byteCode);
		break;
	case Expression::t_LOCALVAR:
		LocalToByteCode(owner, methodInfo, byteCode);
		break;
	default:
		break;
	}

	return byteCode->size() - oldSize;
}

string* Expression::GetName()
{
	switch (type)
	{
	case Expression::t_INT_LITER: return new string(to_string(intLiteral));
	case Expression::t_CHAR_LITER: return new string("\'" + string(1, charLiteral) + "\'");
	case Expression::t_BOOL_LITER: return new string(to_string(boolLiteral));
	case Expression::t_STRING_LITER: return new string("\\\"" + *name + "\\\"");
	case Expression::t_LOCALVAR:
	case Expression::t_ID: return name;
	case Expression::t_INVOCATION: return name;
	case Expression::t_SIMPLE_TYPE: return simpleType->GetName();
	case Expression::t_THIS: return new string("this");
	case Expression::t_BASE: return new string("base");
	case Expression::t_CLASS: return new string("class");
	case Expression::t_OBJECT: return new string("object");
	case Expression::t_MEMBER_ACCESS: return new string(".");
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

DataType* Expression::GetDataTypeOfId(Class* owner, MethodTable* methodInfo)
{
	Variable* localVar = methodInfo->GetLocalVariable(this->name);
	if (localVar != NULL) 
	{
		this->type = t_LOCALVAR;
		return localVar->type; 
	}

	FieldTable* fieldInfo = owner->GetField(*this->name);
	if (fieldInfo != NULL)
	{
		this->type = t_OBJECT;
		this->right = new Expression(Expression::t_ID, this->name);
		this->right->dataType = fieldInfo->GetType();
		if (!fieldInfo->IsStatic())
		{
			this->left = new Expression(t_THIS);
			this->left->DetermineDataType(owner, methodInfo);
		}
		else
		{
			this->left = new Expression(Expression::t_CLASS);
			this->left->dataType = new DataType(DataType::t_TYPENAME, owner, false, owner);
		}

		CheckErrorsOfFieldAccess(owner, methodInfo);

		owner->AppendFieldRefConstant(owner, fieldInfo);

		return fieldInfo->GetType();
	}

	TypeName* tName = new TypeName(this->name);
	Class* classInfo = NULL;

	try { classInfo = owner->FindClass(tName); }
	catch (std::exception) {}

	this->typeName = tName;

	if (classInfo != NULL)
	{
		DataType* dType = new DataType(DataType::t_TYPENAME, classInfo, false, owner);
		this->type = t_CLASS;

		CheckErrorsOfClassesAccess(owner, classInfo);

		return dType;
	}

	return NULL;
}

DataType* Expression::GetDataTypeOfInvocation(Class* owner, MethodTable* methodInfo)
{
	DataType* dType = NULL;
	if (this->left->type == Expression::t_ID)
	{
		this->name = this->left->name;

		if (owner->GetMethod(*this->name) == NULL)
		{
			string err = "There is no such method " + *this->name + " in class " + owner->GetFullName();
			throw std::exception(err.c_str());
		}

		if (owner->GetMethod(*this->name)->IsStatic())
		{
			this->left = new Expression(t_CLASS);
			this->left->dataType = new DataType(DataType::t_TYPENAME, owner, false, owner);
		}
		else
		{
			this->left = new Expression(t_THIS);
			this->left->DetermineDataType(owner, methodInfo);
		}

		dType = owner->GetMethod(*this->name)->GetReturnValue();
	}
	else
	{
		this->name = this->left->right->name;
		this->left = this->left->left;
		this->left->DetermineDataType(owner, methodInfo);

		if (this->left->dataType == NULL)
		{
			string err = "There is no such identifier \"" + this->left->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		if (this->left->dataType->isArray)
		{
			string err = "Unsupported method invocation on array object";
			throw std::exception(err.c_str());
		}
		if (this->left->dataType->classType->GetMethod(*this->name) == NULL)
		{
			string err = "There is no such method " + *this->name + " in class " + this->left->dataType->classType->GetFullName();
			throw std::exception(err.c_str());
		}
		dType = this->left->dataType->classType->GetMethod(*this->name)->GetReturnValue();
	}

	if (argumentList != NULL)
	{
		for (auto arg : *argumentList->arguments)
		{
			arg->DetermineDataType(owner, methodInfo);
		}
	}

	CheckErrorsOfInvokation(owner, methodInfo);

	return dType;
}

DataType* Expression::GetDataTypeOfMemberAccess(Class* owner, MethodTable* methodInfo)
{
	this->left->DetermineDataType(owner, methodInfo);
	if (this->left->dataType == NULL)
	{
		TypeName* tName = TypeName::Append(this->left->typeName, this->right->name);
		this->left = NULL;
		this->right = NULL;

		Class* classInfo = NULL;
		try { classInfo = owner->FindClass(tName); }
		catch (std::exception) {}
		this->typeName = tName;

		if (classInfo != NULL)
		{
			DataType* dType = new DataType(DataType::t_TYPENAME, classInfo, false, owner);
			this->type = t_CLASS;

			CheckErrorsOfClassesAccess(owner, classInfo);

			return dType;
		}
	}
	else if (this->left->dataType->isArray)
	{
		string err = "Unsupported method access on array object";
		throw std::exception(err.c_str());
	}
	else
	{
		FieldTable* fieldInfo = this->left->dataType->classType->GetField(*this->right->name);
		if (fieldInfo != NULL)
		{
			this->type = t_OBJECT;
			this->right->dataType = fieldInfo->GetType();

			CheckErrorsOfFieldAccess(owner, methodInfo);

			owner->AppendFieldRefConstant(this->left->dataType->classType, fieldInfo);

			return fieldInfo->GetType();
		}

		AbstractNamespaceMember* innerMember = this->left->dataType->classType->GetInnerMember(this->right->name);
		if (dynamic_cast<Class*>(innerMember) != NULL)
		{
			DataType* dType = new DataType(DataType::t_TYPENAME, (Class*)innerMember, false, owner);
			this->type = t_CLASS;

			CheckErrorsOfClassesAccess(owner, dType->classType);

			return dType;
		}

		string str = "There is no such identifier \"" + *this->right->name + "\"" +
			" in \"" + this->left->dataType->classType->GetFullName() + "\"";
		throw std::exception(str.c_str());
	}

	return NULL;
}

DataType* Expression::GetDataTypeOfObjectCreation(Class* owner, MethodTable* methodInfo)
{
	DataType::Type type = DataType::t_INT;
	Class* classType = NULL;
	if (this->simpleType != NULL)
	{
		type = DataType::GetType(this->simpleType);
	}
	else
	{
		type = DataType::t_TYPENAME;
		classType = owner->FindClass(this->typeName);
	}
	DataType* dType = new DataType(type, classType, false, owner);

	if (this->argumentList != NULL)
	{
		for (auto arg : *argumentList->arguments)
		{
			arg->DetermineDataType(owner, methodInfo);
			if (arg->expression->dataType == NULL)
			{
				string err = "There is no such identifier \"" + arg->expression->typeName->ToString() + "\"";
				throw std::exception(err.c_str());
			}
		}
	}

	if (this->objInitializer != NULL)
	{
		objInitializer->DetermineDataType(owner, methodInfo, dType->classType);
	}

	CheckErrorsOfObjectCreation(owner, dType->classType);
	
	owner->AppendMethofRefConstant(dType->classType, dType->classType->GetMethod("<init>"));

	return dType;
}

DataType* Expression::GetDataTypeOfArrayCreation(Class* owner, MethodTable* methodInfo)
{
	DataType::Type type = DataType::t_INT;
	Class* classType = NULL;
	bool isArray = true;

	if (simpleType != NULL)
	{
		type = DataType::GetType(simpleType);
	}
	else if (typeName != NULL)
	{
		type = DataType::t_TYPENAME;
		classType = owner->FindClass(typeName);
	}
	else
	{
		if (arrayType->simpleType != NULL)
		{
			type = DataType::GetType(arrayType->simpleType);
		}
		else
		{
			type = DataType::t_TYPENAME;
			classType = owner->FindClass(arrayType->typeName);
		}
	}

	DataType* dType = new DataType(type, classType, isArray, owner);

	if (this->left == NULL)
	{
		if (arrayInitializer == NULL)
		{
			this->left = new Expression(t_INT_LITER, 0);
		}
		else
		{
			this->left = new Expression(t_INT_LITER, (int)arrayInitializer->expressions->size());
		}
	}

	this->left->DetermineDataType(owner, methodInfo);
	if (this->left->dataType == NULL)
	{
		string err = "There is no such identifier \"" + this->left->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	if (arrayInitializer != NULL)
	{
		for (auto init : *arrayInitializer->expressions)
		{
			init->DetermineDataType(owner, methodInfo);
			if (init->dataType == NULL)
			{
				string err = "There is no such identifier \"" + init->typeName->ToString() + "\"";
				throw std::exception(err.c_str());
			}
		}
	}

	CheckErrorsOfArrayCreation(owner, dType);

	return dType;
}

DataType* Expression::GetDataTypeOfElementAccess(Class* owner, MethodTable* methodInfo)
{
	left->DetermineDataType(owner, methodInfo);
	if (left->dataType == NULL)
	{
		string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}
	if (left->dataType->isArray == false)
	{
		string err = "Invalid element access";
		throw std::exception(err.c_str());
	}

	DataType* dType = new DataType(left->dataType->type, left->dataType->classType, false, owner);

	if (argumentList == NULL)
	{
		string err = "The index of the array element is required";
		throw std::exception(err.c_str());
	}

	if (argumentList->arguments->size() > 1)
	{
		string err = "Unsupported access to a multidimensional array";
		throw std::exception(err.c_str());
	}

	for (auto arg : *argumentList->arguments)
	{
		arg->DetermineDataType(owner, methodInfo);
		if (arg->expression->dataType == NULL)
		{
			string err = "There is no such identifier \"" + arg->expression->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		if (arg->expression->dataType->type != DataType::t_INT)
		{
			string err = "Invalid data type of array element index";
			throw std::exception(err.c_str());
		}
	}

	return dType;
}

DataType* Expression::GetDataTypeOfTypeCast(Class* owner, MethodTable* methodInfo)
{
	if (type == t_TYPENAME_CAST)
	{
		typeName = MemberAccess::ToTypeName(left);
		left = right;
		right = NULL;
	}

	DataType::Type type = DataType::t_INT;
	Class* classType = NULL;
	bool isArray = false;
	if (simpleType != NULL)
	{
		type = DataType::GetType(simpleType);
	}
	else if (arrayType != NULL)
	{
		if (arrayType->simpleType != NULL)
		{
			type = DataType::GetType(arrayType->simpleType);
		}
		else
		{
			type = DataType::t_TYPENAME;
			classType = owner->FindClass(arrayType->typeName);
		}
		isArray = true;
	}
	else
	{
		type = DataType::t_TYPENAME;
		classType = owner->FindClass(typeName);
	}
	DataType* dType = new DataType(type, classType, isArray, owner);

	left->DetermineDataType(owner, methodInfo);
	if (left->dataType == NULL)
	{
		string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	return dType;
}

DataType* Expression::GetDataTypeOfArithmetic(Class* owner, MethodTable* methodInfo)
{
	left->DetermineDataType(owner, methodInfo);
	if (left->dataType == NULL)
	{
		string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	right->DetermineDataType(owner, methodInfo);
	if (right->dataType == NULL)
	{
		string err = "There is no such identifier \"" + right->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	if (left->dataType->type == DataType::t_INT && !left->dataType->isArray
		&& right->dataType->type == DataType::t_CHAR && !right->dataType->isArray)
	{
		right->dataType->type = DataType::t_INT;
	}

	if (left->dataType->type == DataType::t_CHAR && !left->dataType->isArray
		&& right->dataType->type == DataType::t_INT && !right->dataType->isArray)
	{
		left->dataType->type = DataType::t_INT;
	}

	DataType* dType = new DataType(left->dataType->type, left->dataType->classType, left->dataType->isArray, owner);

	if (!(*left->dataType == *right->dataType) || (left->dataType->type != DataType::t_INT &&
		left->dataType->type != DataType::t_CHAR) || left->dataType->isArray)
	{
		string str = "Unsupported arithmetic operator for " + *left->dataType->ToString() +
			" and " + *right->dataType->ToString();
		throw std::exception(str.c_str());
	}

	return dType;
}

DataType* Expression::GetDataTypeOfComprasion(Class* owner, MethodTable* methodInfo)
{
	left->DetermineDataType(owner, methodInfo);
	if (left->dataType == NULL)
	{
		string err = "There is no such identifier \"" + left->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	right->DetermineDataType(owner, methodInfo);
	if (right->dataType == NULL)
	{
		string err = "There is no such identifier \"" + right->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	if (left->dataType->type == DataType::t_INT && !left->dataType->isArray
		&& right->dataType->type == DataType::t_CHAR && !right->dataType->isArray)
	{
		right->dataType->type = DataType::t_INT;
	}

	if (left->dataType->type == DataType::t_CHAR && !left->dataType->isArray
		&& right->dataType->type == DataType::t_INT && !right->dataType->isArray)
	{
		left->dataType->type = DataType::t_INT;
	}

	DataType* dType = new DataType(DataType::t_BOOL, NULL, false , owner);

	if (!(*left->dataType == *right->dataType) || (left->dataType->type != DataType::t_INT &&
		left->dataType->type != DataType::t_CHAR) || left->dataType->isArray)
	{
		string str = "Unsupported comprasion operator for " + *left->dataType->ToString() +
			" and " + *right->dataType->ToString();
		throw std::exception(str.c_str());
	}

	return dType;
}

DataType* Expression::GetDataTypeOfSympleType(Class* owner, MethodTable* methodInfo)
{
	this->type = t_CLASS;
	DataType::Type type = DataType::GetType(this->simpleType);
	return new DataType(type, NULL, false, owner);
}

void Expression::CheckErrorsOfInvokation(Class* owner, MethodTable* methodInfo)
{
	MethodTable* invoketedMethod = this->left->dataType->classType->GetMethod(*this->name);
	if (invoketedMethod->CompareArgsTypes(this->argumentList) == false)
	{
		string err = "Incorrect arguments data types in invokation of method \"" + *invoketedMethod->GetName() + "\"";
		throw std::exception(err.c_str());
	}

	if (invoketedMethod->IsStatic() && this->left->type != Expression::t_CLASS)
	{
		string err = "Static methods should be invoked on class";
		throw std::exception(err.c_str());
	}

	if (!invoketedMethod->IsStatic() && this->left->type == Expression::t_CLASS)
	{
		string err = "Dynamic methods should be invoked on object";
		throw std::exception(err.c_str());
	}

	if (!invoketedMethod->IsStatic() && methodInfo->IsStatic() &&
		(this->left->type == Expression::t_THIS || this->left->type == Expression::t_BASE))
	{
		string err = "Static methods can invoke only static methods";
		throw std::exception(err.c_str());
	}

	switch (invoketedMethod->GetAccessModifier())
	{
	case AccessModifier::e_PRIVATE:
		if (left->dataType->classType->GetFullName() != owner->GetFullName())
		{
			string err = "Invoketed method are not allown";
			throw std::exception(err.c_str());
		}
		break;
	case AccessModifier::e_PROTECTED:
		if (!owner->InstanceOf(left->dataType->classType))
		{
			string err = "Invoketed method are not allown";
			throw std::exception(err.c_str());
		}
		break;
	default:
		break;
	}

	owner->AppendMethofRefConstant(this->left->dataType->classType, invoketedMethod);
}

void Expression::CheckErrorsOfFieldAccess(Class* owner, MethodTable* methodInfo)
{
	FieldTable* fieldInfo = this->left->dataType->classType->GetField(*this->right->name);
	if (fieldInfo->IsStatic() && this->left->type != Expression::t_CLASS)
	{
		string err = "Static fields should be invoked on class";
		throw std::exception(err.c_str());
	}

	if (!fieldInfo->IsStatic() && this->left->type == Expression::t_CLASS)
	{
		string err = "Dynamic fields should be invoked on object";
		throw std::exception(err.c_str());
	}

	if (!fieldInfo->IsStatic() && methodInfo->IsStatic() &&
		(this->left->type == Expression::t_THIS || this->left->type == Expression::t_BASE))
	{
		string err = "Static methods can invoke only static fields";
		throw std::exception(err.c_str());
	}

	switch (fieldInfo->GetAccessModifier())
	{
	case AccessModifier::e_PRIVATE:
		if (left->dataType->classType->GetFullName() != owner->GetFullName())
		{
			string err = "Field access are not allown";
			throw std::exception(err.c_str());
		}
		break;
	case AccessModifier::e_PROTECTED:
		if (!owner->InstanceOf(left->dataType->classType))
		{
			string err = "Field access are not allown";
			throw std::exception(err.c_str());
		}
		break;
	default:
		break;
	}
}

void Expression::CheckErrorsOfClassesAccess(Class* owner, Class* classInfo)
{
	switch (classInfo->GetAccessModifier())
	{
	case AccessModifier::e_PRIVATE:
		if (classInfo->GetOuterMember()->GetFullName() != owner->GetFullName())
		{
			string err = "Class access are not allown";
			throw std::exception(err.c_str());
		}
		break;
	case AccessModifier::e_PROTECTED:
		if (!owner->InstanceOf((Class*)classInfo->GetOuterMember()))
		{
			string err = "Class access are not allown";
			throw std::exception(err.c_str());
		}
		break;
	default:
		break;
	}
}

void Expression::CheckErrorsOfObjectCreation(Class* owner, Class* classInfo)
{
	if (classInfo != NULL)
	{
		if (classInfo->IsAbstract())
		{
			string err = "Invalid creation of abstract class \"" + classInfo->GetFullName() + "\"";
			throw std::exception(err.c_str());
		}

		CheckErrorsOfClassesAccess(owner, classInfo);
		MethodTable* constructor = classInfo->GetMethod("<init>");
		if (constructor->CompareArgsTypes(this->argumentList) == false)
		{
			string err = "Incorrect arguments data types in consturtor of \"" + classInfo->GetFullName() + "\"";
			throw std::exception(err.c_str());
		}
	}
	else if (this->argumentList != NULL)
	{
		string err = "Incorrect arguments data types in consturtor of simple type";
		throw std::exception(err.c_str());
	}
}

void Expression::CheckErrorsOfArrayCreation(Class* owner, DataType* arrayType)
{
	if (arrayType->classType != NULL)
	{
		CheckErrorsOfClassesAccess(owner, arrayType->classType);
	}
	if (this->left->dataType->type != DataType::t_INT)
	{
		string err = "Invalid data type of array size expression";
		throw std::exception(err.c_str());
	}

	DataType elemType = DataType(arrayType->type, arrayType->classType, false, owner);
	if (this->arrayInitializer != NULL)
	{
		for (auto init : *arrayInitializer->expressions)
		{
			if (!(elemType == *init->dataType))
			{
				string err = "Invalid data type of array initializer element";
				throw std::exception(err.c_str());
			}
		}
	}
}

int Expression::ThisToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	byteCode->push_back(ByteCode::aload);
	byteCode->push_back(0x00);
	return 2;
}

int Expression::LiteralToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	char* constIndex;
	int oldSize = byteCode->size();
	switch (type)
	{
	case Expression::t_INT_LITER:
		if (intLiteral < -128 || intLiteral > 127)
		{
			constIndex = Constant::IntToByteCode(owner->AppendIntegerConstant(intLiteral));
			byteCode->push_back(ByteCode::ldc_w);
			byteCode->push_back(constIndex[2]);
			byteCode->push_back(constIndex[3]);
		}
		else
		{
			constIndex = Constant::IntToByteCode(intLiteral);
			byteCode->push_back(ByteCode::bipush);
			byteCode->push_back(constIndex[3]);
		}
		break;
	case Expression::t_CHAR_LITER:
		constIndex = Constant::IntToByteCode(charLiteral);
		byteCode->push_back(ByteCode::bipush);
		byteCode->push_back(constIndex[3]);
		break;
	case Expression::t_BOOL_LITER:
		if (boolLiteral)
		{
			byteCode->push_back(ByteCode::iconst_1);
		}
		else
		{
			byteCode->push_back(ByteCode::iconst_0);
		}
		break;
	case Expression::t_STRING_LITER:
		constIndex = Constant::IntToByteCode(owner->AppendStringConstant(name));
		byteCode->push_back(ByteCode::ldc_w);
		byteCode->push_back(constIndex[2]);
		byteCode->push_back(constIndex[3]);
		break;
	default:
		break;
	}
	return byteCode->size() - oldSize;
}

int Expression::InvokationToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	if (this->left->type != t_CLASS)
	{
		this->left->ToByteCode(owner, methodInfo, byteCode);
	}

	MethodTable* invokatedMethod = this->left->dataType->classType->GetMethod(*this->name);

	if (this->argumentList != NULL)
	{
		vector<Expression*> sortedArgs = invokatedMethod->SortArguments(this->argumentList);
		for (auto arg : sortedArgs)
		{
			arg->ToByteCode(owner, methodInfo, byteCode);
		}
	}

	if (invokatedMethod->IsStatic() || left->dataType->type != DataType::t_TYPENAME)
	{
		byteCode->push_back(ByteCode::invokestatic);
	}
	else if (*invokatedMethod->GetName() == "<init>")
	{
		byteCode->push_back(ByteCode::invokespecial);
	}
	else
	{
		byteCode->push_back(ByteCode::invokevirtual);
	}

	char* methodRef = Constant::IntToByteCode(owner->AppendMethofRefConstant(left->dataType->classType, invokatedMethod));
	byteCode->push_back(methodRef[2]);
	byteCode->push_back(methodRef[3]);

	return byteCode->size() - oldSize;
}

int Expression::LocalToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int localIndex = methodInfo->GetLocalIndex(name);
	if (dataType->type == DataType::t_STRING || dataType->type == DataType::t_TYPENAME)
	{
		byteCode->push_back(ByteCode::aload);
	}
	else
	{
		byteCode->push_back(ByteCode::iload);
	}
	byteCode->push_back(Constant::IntToByteCode(localIndex)[3]);
	return 2;
}

int Expression::AssigmentToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	if (left->type == t_LOCALVAR)
	{
		right->ToByteCode(owner, methodInfo, byteCode);
		byteCode->push_back(ByteCode::dup);
		int localIndex = methodInfo->GetLocalIndex(left->name);
		if (left->dataType->type == DataType::t_STRING || left->dataType->type == DataType::t_TYPENAME)
		{
			byteCode->push_back(ByteCode::astore);
		}
		else
		{
			byteCode->push_back(ByteCode::istore);
		}
		byteCode->push_back(Constant::IntToByteCode(localIndex)[3]);
	}
	else if (left->type == t_OBJECT)
	{
		FieldTable* field = left->left->dataType->classType->GetField(*left->right->name);
		char* fieldRef = Constant::IntToByteCode(owner->AppendFieldRefConstant(owner, field));
		if (field->IsStatic())
		{
			right->ToByteCode(owner, methodInfo, byteCode);
			byteCode->push_back(ByteCode::dup);
			byteCode->push_back(ByteCode::putstatic);
		}
		else
		{
			left->left->ToByteCode(owner, methodInfo, byteCode);
			right->ToByteCode(owner, methodInfo, byteCode);
			byteCode->push_back(ByteCode::dup_x1);
			byteCode->push_back(ByteCode::putfield);
		}
		byteCode->push_back(fieldRef[2]);
		byteCode->push_back(fieldRef[3]);
	}
	return byteCode->size() - oldSize;
}

int Expression::ArithmeticToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	left->ToByteCode(owner, methodInfo, byteCode);
	if (type != t_UNMINUS)
	{
		right->ToByteCode(owner, methodInfo, byteCode);
	}

	switch (type)
	{
	case Expression::t_UNMINUS:
		byteCode->push_back(ByteCode::ineg);
		break;
	case Expression::t_MUL:
		byteCode->push_back(ByteCode::imul);
		break;
	case Expression::t_DIV:
		byteCode->push_back(ByteCode::idiv);
		break;
	case Expression::t_MOD:
		byteCode->push_back(ByteCode::irem);
		break;
	case Expression::t_SUM:
		byteCode->push_back(ByteCode::iadd);
		break;
	case Expression::t_SUB:
		byteCode->push_back(ByteCode::isub);
		break;
	default:
		break;
	}

	return byteCode->size() - oldSize;
}

int Expression::ComprasionToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	left->ToByteCode(owner, methodInfo, byteCode);
	right->ToByteCode(owner, methodInfo, byteCode);

	switch (type)
	{
	case Expression::t_LESS:
		byteCode->push_back(ByteCode::if_icmplt);
		break;
	case Expression::t_GREATER:
		byteCode->push_back(ByteCode::if_icmpgt);
		break;
	case Expression::t_LESS_EQUAL:
		byteCode->push_back(ByteCode::if_icmple);
		break;
	case Expression::t_GREATER_EQUAL:
		byteCode->push_back(ByteCode::if_icmpge);
		break;
	case Expression::t_EQUALITY:
		byteCode->push_back(ByteCode::if_icmpeq);
		break;
	case Expression::t_INEQUALITY:
		byteCode->push_back(ByteCode::if_icmpne);
		break;
	default:
		break;
	}

	byteCode->push_back(0x00);
	byteCode->push_back(0x07);

	byteCode->push_back(ByteCode::iconst_0);

	byteCode->push_back(ByteCode::goto_);
	byteCode->push_back(0x00);
	byteCode->push_back(0x04);

	byteCode->push_back(ByteCode::iconst_1);

	return byteCode->size() - oldSize;
}

int Expression::NotToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	left->ToByteCode(owner, methodInfo, byteCode);

	byteCode->push_back(ByteCode::ifne);
	byteCode->push_back(0x00);
	byteCode->push_back(0x07);

	byteCode->push_back(ByteCode::iconst_1);

	byteCode->push_back(ByteCode::goto_);
	byteCode->push_back(0x00);
	byteCode->push_back(0x04);

	byteCode->push_back(ByteCode::iconst_0);

	return byteCode->size() - oldSize;
}

int Expression::AndToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	left->ToByteCode(owner, methodInfo, byteCode);

	byteCode->push_back(ByteCode::ifeq);
	int firstEqIndex = byteCode->size();
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	int rightSize = right->ToByteCode(owner, methodInfo, byteCode);
	char* rightSizeBytes = Constant::IntToByteCode(rightSize + 10);
	byteCode->operator[](firstEqIndex) = rightSizeBytes[2];
	byteCode->operator[](firstEqIndex + 1) = rightSizeBytes[3];

	byteCode->push_back(ByteCode::ifeq);
	byteCode->push_back(0x00);
	byteCode->push_back(0x07);

	byteCode->push_back(ByteCode::iconst_1);

	byteCode->push_back(ByteCode::goto_);
	byteCode->push_back(0x00);
	byteCode->push_back(0x04);

	byteCode->push_back(ByteCode::iconst_0);

	return byteCode->size() - oldSize;
}

int Expression::OrToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	left->ToByteCode(owner, methodInfo, byteCode);

	byteCode->push_back(ByteCode::ifne);
	int firstEqIndex = byteCode->size();
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	int rightSize = right->ToByteCode(owner, methodInfo, byteCode);
	char* rightSizeBytes = Constant::IntToByteCode(rightSize + 6);
	byteCode->operator[](firstEqIndex) = rightSizeBytes[2];
	byteCode->operator[](firstEqIndex + 1) = rightSizeBytes[3];

	byteCode->push_back(ByteCode::ifeq);
	byteCode->push_back(0x00);
	byteCode->push_back(0x07);

	byteCode->push_back(ByteCode::iconst_1);

	byteCode->push_back(ByteCode::goto_);
	byteCode->push_back(0x00);
	byteCode->push_back(0x04);

	byteCode->push_back(ByteCode::iconst_0);

	return byteCode->size() - oldSize;
}

int Expression::ObjectToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	if (this->left->type != t_CLASS)
	{
		this->left->ToByteCode(owner, methodInfo, byteCode);
	}

	FieldTable* field = this->left->dataType->classType->GetField(*this->right->name);

	if (field->IsStatic())
	{
		byteCode->push_back(ByteCode::getstatic);
	}
	else
	{
		byteCode->push_back(ByteCode::getfield);
	}

	char* fieldRef = Constant::IntToByteCode(owner->AppendFieldRefConstant(left->dataType->classType, field));
	byteCode->push_back(fieldRef[2]);
	byteCode->push_back(fieldRef[3]);

	return byteCode->size() - oldSize;
}

int Expression::ObjectCreationToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	
	byteCode->push_back(ByteCode::new_);
	char* classRef = Constant::IntToByteCode(owner->AppendClassConstant(dataType->classType));
	byteCode->push_back(classRef[2]);
	byteCode->push_back(classRef[3]);

	byteCode->push_back(ByteCode::dup);
	MethodTable* constructor = dataType->classType->GetMethod("<init>");
	if (argumentList != NULL)
	{
		vector<Expression*> sortedArgs = constructor->SortArguments(argumentList);
		for (auto arg : sortedArgs)
		{
			arg->ToByteCode(owner, methodInfo, byteCode);
		}
	}
	char* construcRef = Constant::IntToByteCode(owner->AppendMethofRefConstant(dataType->classType, constructor));
	byteCode->push_back(ByteCode::invokespecial);
	byteCode->push_back(construcRef[2]);
	byteCode->push_back(construcRef[3]);

	if (objInitializer != NULL)
	{
		for (auto init : *objInitializer->initializers)
		{
			byteCode->push_back(ByteCode::dup);
			init->ToByteCode(owner, methodInfo, dataType->classType, byteCode);
		}
	}

	return byteCode->size() - oldSize;
}

ObjectCreation::ObjectCreation(SimpleType* simpleType,
	ArgumentList* argumentList, MemberInitializerList* objInit) : Expression(Expression::t_OBJ_CREATION)
{
	this->simpleType = simpleType;
	this->argumentList = argumentList;
	this->objInitializer = objInit;
}

ObjectCreation::ObjectCreation(TypeName* typeName,
	ArgumentList* argumentList, MemberInitializerList* objInit) : Expression(Expression::t_OBJ_CREATION)
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

	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
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

ArrayCreation::ArrayCreation(ArrayType* arrType,
	ExpressionList* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->id = ++maxId;
	this->arrayType = arrType;
	this->arrayInitializer = arrInit;
}

ArrayCreation::ArrayCreation(SimpleType* simpleType,
	Expression* expr, ExpressionList* arrInit) : Expression(Expression::t_ARR_CREATION)
{
	this->id = ++maxId;
	this->simpleType = simpleType;
	this->left = expr;
	this->arrayInitializer = arrInit;
}

ArrayCreation::ArrayCreation(TypeName* typeName,
	Expression* expr, ExpressionList* arrInit) : Expression(Expression::t_ARR_CREATION)
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

	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
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

TypeName* MemberAccess::ToTypeName(Expression* memberAccess)
{
	TypeName* typeName = NULL;
	do
	{
		if (memberAccess->type == t_ID)
		{
			if (typeName == NULL)
				typeName = new TypeName(memberAccess->name);
			else
				typeName->identifiers->push_front(memberAccess->name);
		}
		else if (memberAccess->type == t_MEMBER_ACCESS)
		{
			if (typeName == NULL)
				typeName = new TypeName(memberAccess->right->name);
			else
				typeName->identifiers->push_front(memberAccess->right->name);
		}
		else
		{
			string err = "Incorrect exprression type";
			throw std::exception(err.c_str());
		}
		memberAccess = memberAccess->left;
	} while (memberAccess != NULL);
	return typeName;
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
	*dotStr += to_string(id) + "[label=\"elemAccess\"];\n";
	*dotStr += to_string(id) + "->" + to_string(left->id) + "[label=\"expr\"];\n";
	*dotStr += to_string(id) + "->" + to_string(argumentList->id) + "[label=\"args\"];\n";

	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
	}

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

	if (name != NULL)
	{
		*dotStr += to_string(id) + ".1[label=\"" + *name + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".1[label=\"name\"];\n";
	}

	if (argumentList != NULL)
	{
		*dotStr += *argumentList->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(argumentList->id) + "[label=\"args\"];\n";
	}

	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
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

void VarDeclarator::Semantic(Class* owner, MethodTable* methodInfo)
{
	DataType* dType = owner->CreateDataType(this);
	methodInfo->AddLocalVariable(this->identifier, dType);

	dataType = dType;

	if (initializer == NULL && !dType->isArray)
	{
		switch (dType->type)
		{
		case DataType::t_BOOL:
			initializer = new Expression(Expression::t_BOOL_LITER, false); break;
		case DataType::t_INT:
			initializer = new Expression(Expression::t_INT_LITER, 0); break;
		case DataType::t_CHAR:
			initializer = new Expression(Expression::t_CHAR_LITER, 0); break;
		case DataType::t_STRING:
			initializer = new Expression(Expression::t_STRING_LITER, new string("")); break;
		default: break;
		}
	}

	if (initializer != NULL)
	{
		initializer->DetermineDataType(owner, methodInfo);
		if (!(*dType == *initializer->dataType))
		{
			string err = "it is not possible to convert " + *initializer->dataType->ToString() + " to " + *dType->ToString();
			throw std::exception(err.c_str());
		}
	}
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

	if (dataType != NULL)
	{
		*dotStr += to_string(id) + ".01[label=\"" + *dataType->ToString() + "\"];\n";
		*dotStr += to_string(id) + "->" + to_string(id) + ".01[label=\"dataType\"];\n";
	}

	return dotStr;
}

int VarDeclarator::ToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	if (initializer != NULL)
	{
		initializer->ToByteCode(owner, methodInfo, byteCode);
		int localIndex = methodInfo->GetLocalIndex(identifier);
		if (dataType->type == DataType::t_STRING || dataType->type == DataType::t_TYPENAME)
		{
			byteCode->push_back(ByteCode::astore);
		}
		else
		{
			byteCode->push_back(ByteCode::istore);
		}
		byteCode->push_back(Constant::IntToByteCode(localIndex)[3]);
	}

	return byteCode->size() - oldSize;
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
		varDecl = new VarDeclarator(new SimpleType(declarators->declarators->front()->simpleType), identifier, expression);
		break;
	case VarDeclarator::t_ARRAY_TYPE:
		varDecl = new VarDeclarator(new ArrayType(declarators->declarators->front()->arrayType), identifier, expression);
		break;
	case VarDeclarator::t_TYPE_NAME:
		varDecl = new VarDeclarator(new TypeName(declarators->declarators->front()->typeName), identifier, expression);
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

int VarDeclaratorList::ToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();

	for (auto decl : *declarators)
	{
		decl->ToByteCode(owner, methodInfo, byteCode);
	}

	return byteCode->size() - oldSize;
}

void Statement::CheckIfStmtError(Class* owner, MethodTable* methodInfo)
{
	Expression* cond = expressions->expressions->front();
	cond->DetermineDataType(owner, methodInfo);
	if (cond->dataType == NULL)
	{
		string err = "There is no such identifier \"" + cond->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	if (cond->dataType->type != DataType::t_BOOL || cond->dataType->isArray)
	{
		string err = "Data type of if statement condition must be boolean";
		throw std::exception(err.c_str());
	}

	if (statements != NULL)
	{
		for (auto stmt : *statements->statements)
		{
			stmt->Semantic(owner, methodInfo);
		}
	}
}

void Statement::CheckWhileStmtError(Class* owner, MethodTable* methodInfo)
{
	Expression* cond = expressions->expressions->front();
	cond->DetermineDataType(owner, methodInfo);
	if (cond->dataType == NULL)
	{
		string err = "There is no such identifier \"" + cond->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	if (cond->dataType->type != DataType::t_BOOL || cond->dataType->isArray)
	{
		string err = "Data type of while statement condition must be boolean";
		throw std::exception(err.c_str());
	}

	if (statements != NULL)
	{
		for (auto stmt : *statements->statements)
		{
			stmt->Semantic(owner, methodInfo);
		}
	}
}

void Statement::CheckDoStmtError(Class* owner, MethodTable* methodInfo)
{
	Expression* cond = expressions->expressions->front();
	cond->DetermineDataType(owner, methodInfo);
	if (cond->dataType == NULL)
	{
		string err = "There is no such identifier \"" + cond->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}

	if (cond->dataType->type != DataType::t_BOOL || cond->dataType->isArray)
	{
		string err = "Data type of do while statement condition must be boolean";
		throw std::exception(err.c_str());
	}

	if (statements != NULL)
	{
		for (auto stmt : *statements->statements)
		{
			stmt->Semantic(owner, methodInfo);
		}
	}
}

void Statement::CheckForStmtError(Class* owner, MethodTable* methodInfo)
{
	if (declarators != NULL)
	{
		for (auto decl : *declarators->declarators)
		{
			decl->Semantic(owner, methodInfo);
		}
	}

	if (inits != NULL)
	{
		for (auto init : *inits->expressions)
		{
			if (init->type != Expression::t_ASSIGNMENT)
			{
				string err = "Invalid initial expression of for statement";
				throw std::exception(err.c_str());
			}

			init->DetermineDataType(owner, methodInfo);
			if (init->dataType == NULL)
			{
				string err = "There is no such identifier \"" + init->typeName->ToString() + "\"";
				throw std::exception(err.c_str());
			}
		}
	}

	if (expressions != NULL)
	{
		Expression* cond = expressions->expressions->front();
		cond->DetermineDataType(owner, methodInfo);
		if (cond->dataType == NULL)
		{
			string err = "There is no such identifier \"" + cond->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
		if (cond->dataType->type != DataType::t_BOOL || cond->dataType->isArray)
		{
			string err = "Data type of for statement condition must be boolean";
			throw std::exception(err.c_str());
		}
	}

	if (incrs != NULL)
	{
		for (auto incr : *incrs->expressions)
		{
			if (incr->type != Expression::t_ASSIGNMENT)
			{
				string err = "Invalid increment expression of for statement";
				throw std::exception(err.c_str());
			}

			incr->DetermineDataType(owner, methodInfo);
			if (incr->dataType == NULL)
			{
				string err = "There is no such identifier \"" + incr->typeName->ToString() + "\"";
				throw std::exception(err.c_str());
			}
		}
	}

	if (statements != NULL)
	{
		for (auto stmt : *statements->statements)
		{
			stmt->Semantic(owner, methodInfo);
		}
	}
}

void Statement::CheckForeachStmtError(Class* owner, MethodTable* methodInfo)
{
	VarDeclarator* decl = declarators->declarators->front();
	decl->Semantic(owner, methodInfo);

	Expression* iter = expressions->expressions->front();
	iter->DetermineDataType(owner, methodInfo);
	if (iter->dataType == NULL)
	{
		string err = "There is no such identifier \"" + iter->typeName->ToString() + "\"";
		throw std::exception(err.c_str());
	}
	if (!iter->dataType->isArray)
	{
		string err = "iterator in foreach statement must be array";
		throw std::exception(err.c_str());
	}

	DataType dTypeIterElem = DataType(iter->dataType->type, iter->dataType->classType, false, owner);
	if (!(*decl->dataType == dTypeIterElem))
	{
		string err = "it is not possible to convert " + *dTypeIterElem.ToString() + " to " + *decl->dataType->ToString();
		throw std::exception(err.c_str());
	}

	if (statements != NULL)
	{
		for (auto stmt : *statements->statements)
		{
			stmt->Semantic(owner, methodInfo);
		}
	}
}

void Statement::CheckReturnStmtError(Class* owner, MethodTable* methodInfo)
{
	if (expressions != NULL)
	{
		Expression* expr = expressions->expressions->front();
		expr->DetermineDataType(owner, methodInfo);
		if (expr->dataType == NULL)
		{
			string err = "There is no such identifier \"" + expr->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}

		if (!(*methodInfo->GetReturnValue() == *expr->dataType))
		{
			string err = "it is not possible to convert " + *expr->dataType->ToString() + " to " + *methodInfo->GetReturnValue()->ToString();
			throw std::exception(err.c_str());
		}
	}
	else
	{
		if (methodInfo->GetReturnValue()->type != DataType::t_VOID)
		{
			string err = "it is not possible to convert VOID to " + *methodInfo->GetReturnValue()->ToString();
			throw std::exception(err.c_str());
		}
	}
}

int Statement::ReturnToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	if (expressions != NULL)
	{
		expressions->expressions->front()->ToByteCode(owner, methodInfo, byteCode);
		if (expressions->expressions->front()->dataType->type == DataType::t_STRING || expressions->expressions->front()->dataType->type == DataType::t_TYPENAME)
		{
			byteCode->push_back(ByteCode::areturn);
		}
		else
		{
			byteCode->push_back(ByteCode::ireturn);
		}
	}
	else
	{
		byteCode->push_back(ByteCode::return_);
	}
	return byteCode->size() - oldSize;
}

int Statement::IfToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	
	expressions->expressions->front()->ToByteCode(owner, methodInfo, byteCode);
	byteCode->push_back(ByteCode::ifeq);
	int ifIndex = byteCode->size();
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	auto stmtIter = statements->statements->begin();
	Statement* ifStmt = *stmtIter; stmtIter++;
	Statement* elseStmt = NULL;
	if (stmtIter != statements->statements->end())
	{
		elseStmt = *stmtIter;
	}

	int ifStmtSize = ifStmt->ToByteCode(owner, methodInfo, byteCode);
	char* ifStmtBytes = Constant::IntToByteCode(ifStmtSize + 6);
	byteCode->operator[](ifIndex) = ifStmtBytes[2];
	byteCode->operator[](ifIndex + 1) = ifStmtBytes[3];

	byteCode->push_back(ByteCode::goto_);
	int elseIndex = byteCode->size();
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	int elseStmtSize = 0;
	if (elseStmt != NULL)
	{
		elseStmtSize = elseStmt->ToByteCode(owner, methodInfo, byteCode);
	}
	char* elseStmtBytes = Constant::IntToByteCode(elseStmtSize + 3);
	byteCode->operator[](elseIndex) = elseStmtBytes[2];
	byteCode->operator[](elseIndex + 1) = elseStmtBytes[3];

	return byteCode->size() - oldSize;
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

void Statement::Semantic(Class* owner, MethodTable* methodInfo)
{
	if (type == t_EXPRESSION)
	{
		expressions->expressions->front()->DetermineDataType(owner, methodInfo);
		if (expressions->expressions->front()->dataType == NULL)
		{
			string err = "There is no such identifier \"" + expressions->expressions->front()->typeName->ToString() + "\"";
			throw std::exception(err.c_str());
		}
	}
	else if (type == t_DECLARATOR)
	{
		for (auto decl : *declarators->declarators)
		{
			decl->Semantic(owner, methodInfo);
		}
	}
	else if (type == t_IF)
	{
		CheckIfStmtError(owner, methodInfo);
	}
	else if (type == t_WHILE)
	{
		CheckWhileStmtError(owner, methodInfo);
	}
	else if (type == t_DO)
	{
		CheckDoStmtError(owner, methodInfo);
	}
	else if (type == t_FOR)
	{
		CheckForStmtError(owner, methodInfo);
	}
	else if (type == t_FOREACH)
	{
		CheckForeachStmtError(owner, methodInfo);
	}
	else if (type == t_RETURN)
	{
		CheckReturnStmtError(owner, methodInfo);
	}
	else if (type == t_BLOCK)
	{
		if (statements != NULL)
		{
			for (auto stmt : *statements->statements)
			{
				stmt->Semantic(owner, methodInfo);
			}
		}
	}
}

int Statement::ToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	switch (type)
	{
	case Statement::t_EMPTY:
		break;
	case Statement::t_EXPRESSION:
		expressions->expressions->front()->ToByteCode(owner, methodInfo, byteCode);
		if (expressions->expressions->front()->type == Expression::t_ASSIGNMENT)
		{
			byteCode->push_back(ByteCode::pop);
		}
		break;
	case Statement::t_DECLARATOR:
		declarators->ToByteCode(owner, methodInfo, byteCode);
		break;
	case Statement::t_IF:
		IfToByteCode(owner, methodInfo, byteCode);
		break;
	case Statement::t_WHILE:
		break;
	case Statement::t_DO:
		break;
	case Statement::t_FOR:
		break;
	case Statement::t_FOREACH:
		break;
	case Statement::t_RETURN:
		ReturnToByteCode(owner, methodInfo, byteCode);
		break;
	case Statement::t_BLOCK:
		for (auto stmt : *statements->statements)
		{
			stmt->ToByteCode(owner, methodInfo, byteCode);
		}
		break;
	default:
		break;
	}
	return byteCode->size() - oldSize;
}

StatementList::StatementList(Statement* statement)
{
	this->id = statement->id;
	this->statements = new list <Statement*>{ statement };
}

StatementList* StatementList::Append(StatementList* statements, Statement* statement)
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

int StatementList::ToByteCode(Class* owner, MethodTable* methodInfo, vector<char>* byteCode)
{
	int oldSize = byteCode->size();
	for (auto stmt : *statements)
	{
		stmt->ToByteCode(owner, methodInfo, byteCode);
	}
	return byteCode->size() - oldSize;
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

	string* dotStr = decl->ToDOT();
	*dotStr += *cond->ToDOT();
	*dotStr += *stmt->ToDOT();
	*dotStr += to_string(id) + "[label=\"foreach_stmt\"];\n";
	*dotStr += to_string(id) + "->" + to_string(decl->id) + "[label=\"decl\"];\n";
	*dotStr += to_string(id) + "->" + to_string(cond->id) + "[label=\"iterator\"];\n";
	*dotStr += to_string(id) + "->" + to_string(stmt->id) + "[label=\"stmt\"];\n";

	return dotStr;
}

ForStatement::ForStatement(ExpressionList* init,
	Expression* cond, ExpressionList* increment, Statement* statement) : Statement(Statement::t_FOR)
{
	this->id = ++maxId;
	if (init != NULL)
		this->inits = init;
	if (cond != NULL)
		this->expressions = new ExpressionList(cond);
	if (increment != NULL)
		this->incrs = increment;
	this->statements = new StatementList(statement);
}

ForStatement::ForStatement(VarDeclaratorList* declarations,
	Expression* cond, ExpressionList* increment, Statement* statement) : Statement(Statement::t_FOR)
{
	this->id = ++maxId;
	if (declarations != NULL)
		this->declarators = declarations;
	if (cond != NULL)
		this->expressions = new ExpressionList(cond);
	if (increment != NULL)
		this->incrs = increment;
	this->statements = new StatementList(statement);
}

string* ForStatement::ToDOT()
{
	string* dotStr = new string();
	*dotStr += to_string(id) + "[label=\"for_stmt\"];\n";

	if (declarators != NULL)
	{
		*dotStr += *declarators->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(declarators->id) + "[label=\"init\"];\n";
	}
	if (inits != NULL)
	{
		*dotStr += *inits->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(inits->id) + "[label=\"init\"];\n";
	}
	if (expressions != NULL)
	{
		*dotStr += *expressions->expressions->front()->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(expressions->id) + "[label=\"cond\"];\n";
	}
	if (incrs != NULL)
	{
		*dotStr += *incrs->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(incrs->id) + "[label=\"incr\"];\n";
	}
	if (statements != NULL)
	{
		*dotStr += *statements->ToDOT();
		*dotStr += to_string(id) + "->" + to_string(statements->id) + "[label=\"stmt\"];\n";
	}

	return dotStr;
}

ReturnStatement::ReturnStatement(Expression* expression) : Statement(Statement::t_RETURN, expression)
{
	this->id = ++maxId;
	if (expression != NULL)
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

string Modifier::GetName()
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
	return name;
}

string* Modifier::ToDOT()
{
	string name = GetName();
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
	this->id = ++maxId;
	this->members = new list < ClassMember*>{ member };
}

ClassMemberList* ClassMemberList::Append(ClassMemberList* members, ClassMember* member)
{
	members->members->push_back(member);
	return members;
}

string* ClassMemberList::ToDOT()
{
	string* dotStr = new string();
	*dotStr = to_string(id) + "[label=\"members\"];\n";
	for (auto i = members->begin(); i != members->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		*dotStr += to_string(id) + "->" + to_string((*i)->id) + ";\n";
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
	*dotStr += to_string(id) + "[label=\"constructor\"];\n";

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
		if (classMemberList->members->size() > 1)
		{
			*dotStr += *classMemberList->ToDOT();
			*dotStr += to_string(id) + "->" + to_string(classMemberList->id) + "[label=\"body\"];\n";
		}
		else
		{
			*dotStr += *classMemberList->members->front()->ToDOT();
			*dotStr += to_string(id) + "->" + to_string(classMemberList->members->front()->id) + "[label=\"body\"];\n";
		}
	}

	return dotStr;
}

AbstractNamespaceMember* ClassDeclaration::CreateClassTable(AbstractNamespaceMember* outer)
{
	if (outer->GetInnerMember(identifier) != NULL)
	{
		string err = "Identifier \"" + *identifier + "\" already exists in \"" + outer->GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	Class* current = new Class(identifier, outer, this);
	if (modifiers == NULL)
	{
		if (dynamic_cast<Class*>(outer))
			modifiers = new ModifielrList(new Modifier(Modifier::t_PRIVATE));
		else
			modifiers = new ModifielrList(new Modifier(Modifier::t_PUBLIC));
	}

	for (auto modifier : *modifiers->modifiers)
	{
		switch (modifier->type)
		{
		case Modifier::t_PUBLIC:
			current->SetAccesModifier(e_PUBLIC);
			break;
		case Modifier::t_PROTECTED:
			current->SetAccesModifier(e_PROTECTED);
			break;
		case Modifier::t_INTERNAL:
			current->SetAccesModifier(e_INTERNAL);
			break;
		case Modifier::t_PRIVATE:
			current->SetAccesModifier(e_PRIVATE);
			break;
		case Modifier::t_ABSTRACT:
			current->SetAbstract(true);
			break;
		case Modifier::t_STATIC:
			current->SetStatic(true);
			break;
		default:
			string err = "Illegal class modifier \"" + modifier->GetName() + "\" of class \"" + current->GetFullName() + "\"";
			throw std::exception(err.c_str());
		}
	}
	if (current->GetAccessModifier() == e_NONE)
	{
		current->SetAccesModifier(e_PUBLIC);
	}

	if (classMemberList != NULL)
	{
		for (auto member : *classMemberList->members)
		{
			if (member->type == ClassMember::t_CLASS)
			{
				current->Append(((ClassDeclaration*)member)->CreateClassTable(current));
			}
		}
	}
	return current;
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

AbstractNamespaceMember* NamespaceMember::CreateClassTable(AbstractNamespaceMember* outer)
{
	AbstractNamespaceMember* current;
	if (type == t_NAMESPACE)
	{
		current = namespaceDecl->CreateClassTable(outer);
	}
	else
	{
		current = classDecl->CreateClassTable(outer);
	}
	return current;
}

NamespaceMemberList::NamespaceMemberList(NamespaceMember* member)
{
	this->id = ++maxId;
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
	*dotStr = to_string(id) + "[label=\"members\"];\n";
	for (auto i = members->begin(); i != members->end(); i++)
	{
		*dotStr += *((*i)->ToDOT());
		*dotStr += to_string(id) + "->" + to_string((*i)->id) + ";\n";
	}
	return dotStr;
}

AbstractNamespaceMember* NamespaceMemberList::CreateClassTable(AbstractNamespaceMember* outer)
{
	for (auto namespaceMember : *members)
	{
		outer->Append(namespaceMember->CreateClassTable(outer));
	}
	return outer;
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
		if (members->members->size() > 1)
		{
			*dotStr += *members->ToDOT();
			*dotStr += to_string(id) + "->" + to_string(members->id) + "[label=\"body\"];\n";
		}
		else
		{
			*dotStr += *members->members->front()->ToDOT();
			*dotStr += to_string(id) + "->" + to_string(members->members->front()->id) + "[label=\"body\"];\n";
		}
	}

	return dotStr;
}

AbstractNamespaceMember* NamespaceDeclaration::CreateClassTable(AbstractNamespaceMember* outer)
{
	AbstractNamespaceMember* current = outer->GetInnerMember(typeName->identifiers->front());
	if (current != NULL && dynamic_cast<Class*>(current))
	{
		string err = "Identifier \"" + *typeName->identifiers->front() + "\" already exists in \"" + outer->GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	if (current == NULL)
	{
		current = new Namespace(typeName->identifiers->front(), outer);
	}

	if (typeName->identifiers->size() > 1)
	{
		TypeName* childName = typeName;
		typeName = new TypeName(childName->identifiers->front());
		childName->identifiers->pop_front();
		NamespaceDeclaration* child = new NamespaceDeclaration(childName, members);
		members = new NamespaceMemberList(new NamespaceMember(child));
		current->Append(child->CreateClassTable(current));
	}
	else
	{
		if (members != NULL)
		{
			current = members->CreateClassTable(current);
		}
	}
	return current;
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

AbstractNamespaceMember* Programm::CreateClassTable()
{
	AbstractNamespaceMember* global = new Namespace(new string("global"), NULL);
	global = members->CreateClassTable(global);
	return global;
}
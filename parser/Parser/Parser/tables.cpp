#include "tables.h"
#include "../../bison/classes.h"

int maxTabelId = 0;

string Namespace::GetFullName()
{
	string fullName = *GetName();
	if (outerMember != NULL)
	{
		fullName = outerMember->GetFullName() + "/" + fullName;
	}
	return fullName;
}

AbstractNamespaceMember* Namespace::GetOuterMember()
{
	return outerMember;
}

Namespace::Namespace(string* name, AbstractNamespaceMember* outer)
{
	this->id = ++maxTabelId;
	this->name = name;
	this->outerMember = outer;
}

int Namespace::GetId()
{
	return id;
}

string* Namespace::GetName()
{
	return name;
}

AbstractNamespaceMember* Namespace::GetInnerMember(string* name)
{
	AbstractNamespaceMember* neededMember = NULL;
	for (auto member : members)
	{
		if (*member->GetName() == *name)
		{
			neededMember = member;
		}
	}
	return neededMember;
}

vector<AbstractNamespaceMember*> Namespace::GetAllMembers()
{
	return members;
}

void Namespace::Append(AbstractNamespaceMember* member)
{
	if (GetInnerMember(member->GetName()) == NULL)
	{
		members.push_back(member);
	}
}

string* Namespace::ToDOT()
{
	string* dotStr = new string();
	*dotStr = to_string(id) + "[label=\"" + *name + "\"];\n";
	for (auto member : members)
	{
		*dotStr += *member->ToDOT();
		*dotStr += to_string(id) + "--" + to_string(member->GetId()) + ";\n";
	}
	return dotStr;
}

string Class::GetFullName()
{
	string fullName = outerMember->GetFullName();
	fullName += "/" + *GetName();
	return fullName;
}

Class* Class::GetParent()
{
	return parent;
}

DataType* Class::CreateDataType(ClassMember* member)
{
	DataType::Type type = DataType::t_INT;
	Class* classType = NULL;
	bool isArray = false;

	TypeName* typeName = NULL;
	switch (member->returnValue)
	{
	case ClassMember::t_SIMPLE_TYPE:
		type = DataType::GetType(member->simpleType);
		break;
	case ClassMember::t_TYPENAME:
		type = DataType::t_TYPENAME;
		typeName = member->typeName;
		break;
	case ClassMember::t_VOID:
		type = DataType::t_VOID;
		break;
	case ClassMember::t_ARRAY:
		if (member->arrayType->type == ArrayType::t_SIMPLE_TYPE) {
			type = DataType::GetType(member->arrayType->simpleType);
		}
		else {
			type = DataType::t_TYPENAME;
		}
		typeName = member->arrayType->typeName;
		isArray = true;
		break;
	default:
		break;
	}

	if (typeName != NULL)
	{
		classType = FindClass(typeName);
	}

	return new DataType(type, classType, isArray, this);
}

DataType* Class::CreateDataType(VarDeclarator* varDecl)
{
	DataType::Type type = DataType::t_INT;
	Class* classType = NULL;
	bool isArray = false;

	TypeName* typeName = NULL;
	switch (varDecl->type)
	{
	case VarDeclarator::t_SIMPLE_TYPE:
		type = DataType::GetType(varDecl->simpleType);
		break;
	case VarDeclarator::t_TYPE_NAME:
		type = DataType::t_TYPENAME;
		typeName = varDecl->typeName;
		break;
	case VarDeclarator::t_ARRAY_TYPE:
		if (varDecl->arrayType->type == ArrayType::t_SIMPLE_TYPE) {
			type = DataType::GetType(varDecl->arrayType->simpleType);
		}
		else {
			type = DataType::t_TYPENAME;
		}
		typeName = varDecl->arrayType->typeName;
		isArray = true;
		break;
	default:
		break;
	}

	if (typeName != NULL)
	{
		classType = FindClass(typeName);
	}

	return new DataType(type, classType, isArray, this);
}

Class* Class::FindClass(TypeName* typeName)
{
	AbstractNamespaceMember* neededMember = NULL;
	AbstractNamespaceMember* outer = this;
	while (neededMember == NULL && outer != NULL)
	{
		neededMember = outer->GetInnerMember(typeName->identifiers->front());
		outer = outer->GetOuterMember();
	}

	auto currentName = typeName->identifiers->begin(); currentName++;
	while (neededMember != NULL && currentName != typeName->identifiers->end())
	{
		neededMember = neededMember->GetInnerMember(*currentName);
		currentName++;
	}

	if (dynamic_cast<Class*>(neededMember) == NULL)
	{
		string err = "Not such identifier \"" + *typeName->identifiers->back() + "\"";
		throw std::exception(err.c_str());
	}

	return (Class*)neededMember;
}

void Class::AppendField(Field* field)
{
	DataType* dataType = CreateDataType(field);
	if (fields.count(*field->identifier) > 0 || GetInnerMember(field->identifier) != NULL)
	{
		string err = "Identifier \"" + *field->identifier + "\" already exists in class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	FieldTable* newField = new FieldTable(field->identifier, dataType);
	if (field->modifiers == NULL)
	{
		field->modifiers = new ModifielrList(new Modifier(Modifier::t_PRIVATE));
	}

	for (auto modifier : *field->modifiers->modifiers)
	{
		switch (modifier->type)
		{
		case Modifier::t_PRIVATE:   newField->SetAccessModifier(e_PRIVATE);   break;
		case Modifier::t_PROTECTED: newField->SetAccessModifier(e_PROTECTED); break;
		case Modifier::t_PUBLIC:	newField->SetAccessModifier(e_PUBLIC);	  break;
		case Modifier::t_INTERNAL:  newField->SetAccessModifier(e_INTERNAL);  break;
		case Modifier::t_STATIC:	newField->SetStatic(true); break;
		default:
			string err = "Illigal modifier \"" + modifier->GetName() + "\" of class \"" + GetFullName() + "\"";
			throw std::exception(err.c_str());
		}
	}
	if (newField->GetAccessModifier() == e_NONE)
	{
		newField->SetAccessModifier(e_PRIVATE);
	}

	newField->SetInitializer(field->expression);

	AppendUtf8Constant(newField->GetName());
	AppendUtf8Constant(new string(newField->GetType()->ToDescriptor()));

	fields[*newField->GetName()] = newField;
}

void Class::AppendMethod(Method* method)
{
	DataType* dataType = CreateDataType(method);
	if (methods.count(*method->identifier) > 0)
	{
		string err = "Method \"" + *method->identifier + "\" already exists in class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	MethodTable* newMethod = new MethodTable(method->identifier, dataType);
	if (method->modifiers == NULL)
	{
		method->modifiers = new ModifielrList(new Modifier(Modifier::t_PRIVATE));
	}

	for (auto modifier : *method->modifiers->modifiers)
	{
		switch (modifier->type)
		{
		case Modifier::t_PRIVATE:   newMethod->SetAccessModifier(e_PRIVATE);   break;
		case Modifier::t_PROTECTED: newMethod->SetAccessModifier(e_PROTECTED); break;
		case Modifier::t_PUBLIC:	newMethod->SetAccessModifier(e_PUBLIC);	  break;
		case Modifier::t_INTERNAL:  newMethod->SetAccessModifier(e_INTERNAL); break;
		case Modifier::t_STATIC:	newMethod->SetStatic(true);   break;
		case Modifier::t_ABSTRACT:  newMethod->SetAbstract(true); break;
		case Modifier::t_VIRTUAL:   newMethod->SetVirtual(true);  break;
		case Modifier::t_OVERRIDE:  newMethod->SetOverride(true); break;
		default:
			string err = "Illigal modifier \"" + modifier->GetName() + "\" of method \"" +
				*method->identifier + "\" in class \"" + GetFullName() + "\"";
			throw std::exception(err.c_str());
		}
	}
	if (newMethod->GetAccessModifier() == e_NONE)
	{
		newMethod->SetAccessModifier(e_PRIVATE);
	}

	if (method->paramList != NULL)
	{
		for (auto param : *method->paramList->params)
		{
			DataType* paramType = CreateDataType(param);
			newMethod->AddParam(param->identifier, paramType);
		}
	}
	newMethod->SetBody(method->statementList);

	AppendUtf8Constant(newMethod->GetName());
	AppendUtf8Constant(newMethod->GetDescriptor());

	AppendMainMethod(newMethod);

	methods[*newMethod->GetName()] = newMethod;
}

void Class::AppendConstructor(Constructor* constructor)
{
	string* constructorName = new string("<init>");
	if (methods.count(*constructorName) > 0)
	{
		string err = "Unsupported constructor overriding in class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}
	if (*this->name != *constructor->identifier)
	{
		string err = "Constructor name doesn't match with name of class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	DataType* dataType = new DataType(DataType::t_VOID, NULL, false, this);

	MethodTable* newConstructor = new MethodTable(constructorName, dataType);
	if (constructor->modifiers == NULL)
	{
		constructor->modifiers = new ModifielrList(new Modifier(Modifier::t_PRIVATE));
	}

	for (auto modifier : *constructor->modifiers->modifiers)
	{
		switch (modifier->type)
		{
		case Modifier::t_PRIVATE:   newConstructor->SetAccessModifier(e_PRIVATE);   break;
		case Modifier::t_PROTECTED: newConstructor->SetAccessModifier(e_PROTECTED); break;
		case Modifier::t_PUBLIC:	newConstructor->SetAccessModifier(e_PUBLIC);	break;
		default:
			string err = "Illigal constructor modifier \"" + modifier->GetName() + "\" of class \"" + GetFullName() + "\"";
			throw std::exception(err.c_str());
		}
	}
	if (newConstructor->GetAccessModifier() == e_NONE)
	{
		newConstructor->SetAccessModifier(e_PRIVATE);
	}

	if (constructor->paramList != NULL)
	{
		for (auto param : *constructor->paramList->params)
		{
			DataType* paramType = CreateDataType(param);
			newConstructor->AddParam(param->identifier, paramType);
		}
	}
	newConstructor->SetBody(constructor->statementList);
	AppendFieldInitializers(newConstructor, constructor->argumentList);
	constructor->statementList = newConstructor->GetBody();

	AppendUtf8Constant(newConstructor->GetName());
	AppendUtf8Constant(newConstructor->GetDescriptor());

	methods[*constructorName] = newConstructor;
}

void Class::AppdendDefaultConstructor()
{
	string* constructorName = new string("<init>");
	if (methods.count(*constructorName) > 0)
	{
		string err = "Unsupported constructor overriding in class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	DataType* dataType = new DataType(DataType::t_VOID, NULL, false, this);

	MethodTable* newConstructor = new MethodTable(constructorName, dataType);
	newConstructor->SetAccessModifier(e_PUBLIC);
	AppendFieldInitializers(newConstructor, NULL);

	if (decl != NULL)
	{
		if (decl->classMemberList != NULL)
		{
			ClassMemberList::Append(decl->classMemberList,
				new Constructor(NULL, GetName(), NULL, ClassMember::t_NULL, newConstructor->GetBody()));
		}
		else
		{
			decl->classMemberList = new ClassMemberList(
				new Constructor(NULL, GetName(), NULL, ClassMember::t_NULL, newConstructor->GetBody()));
		}
	}

	AppendUtf8Constant(newConstructor->GetName());
	AppendUtf8Constant(newConstructor->GetDescriptor());

	methods[*constructorName] = newConstructor;
}

void Class::AppendParent(TypeName* parentName)
{
	parent = FindClass(parentName);
	AppendClassConstant(parent);
}

void Class::AppendFieldInitializers(MethodTable* constructor, ArgumentList* args)
{
	for (auto field : GetAllFields())
	{
		if (!field->IsStatic() && field->GetInitializer() != NULL)
		{
			Expression* fieldName = new Expression(Expression::t_ID, field->GetName());
			Expression* fieldAccess = new Expression(Expression::t_MEMBER_ACCESS, new Expression(Expression::t_THIS), fieldName);
			Expression* initExpr = new Expression(Expression::t_ASSIGNMENT, fieldAccess, field->GetInitializer());
			Statement* fieldInit = new Statement(Statement::t_EXPRESSION, initExpr);

			if (constructor->GetBody() != NULL)
			{
				StatementList* body = constructor->GetBody();
				body->statements->push_front(fieldInit);
				body->id = body->statements->front()->id;
				constructor->SetBody(body);
			}
			else
			{
				StatementList* body = new StatementList(fieldInit);
				constructor->SetBody(body);
			}
		}
		else if (field->GetInitializer() != NULL)
		{
			AppendStaticInitializer(field);
		}
	}

	TypeName* baseName = new TypeName(new string("<init>"));
	Expression* baseExpr = MemberAccess::FromTypeName(baseName, new Expression(Expression::t_BASE));

	Expression* baseInvoke = new InvocationExpression(baseExpr, args);
	Statement* baseInit = new Statement(Statement::t_EXPRESSION, baseInvoke);

	if (constructor->GetBody() != NULL)
	{
		StatementList* body = constructor->GetBody();
		body->statements->push_front(baseInit);
		body->id = body->statements->front()->id;
		constructor->SetBody(body);
	}
	else
	{
		StatementList* body = new StatementList(baseInit);
		constructor->SetBody(body);
	}
}

void Class::AppendStaticInitializer(FieldTable* fieldInfo)
{
	if (methods.count("<clinit>") == 0)
	{
		AppendMethod(new string("<clinit>"), 
			new DataType(DataType::t_VOID, NULL, false, this), 
			vector<Variable*>());
		methods["<clinit>"]->SetStatic(true);
	}

	MethodTable* clinit = methods["<clinit>"];
	Expression* fieldName = new Expression(Expression::t_ID, fieldInfo->GetName());
	Expression* initExpr = new Expression(Expression::t_ASSIGNMENT, fieldName, fieldInfo->GetInitializer());
	Statement* fieldInit = new Statement(Statement::t_EXPRESSION, initExpr);

	if (clinit->GetBody() != NULL)
	{
		StatementList* body = clinit->GetBody();
		body->statements->push_back(fieldInit);
		clinit->SetBody(body);
	}
	else
	{
		StatementList* body = new StatementList(fieldInit);
		clinit->SetBody(body);
	}
}

void Class::CheckOverridingMethods()
{
	for (auto method : methods)
	{
		if (method.second->IsOverride())
		{
			MethodTable* parentMethod = parent->GetMethod(*method.second->GetName());
			if (parentMethod == NULL)
			{
				string err = "No overriding method \"" + *method.second->GetName()
					+ "\" in class \"" + GetFullName() + "\"";
				throw std::exception(err.c_str());
			}
			if (!parentMethod->IsAbstract() && !parentMethod->IsVirtual() && !parentMethod->IsOverride())
			{
				string err = "Method \"" + *method.second->GetName()
					+ "\" in class \"" + GetFullName() + "\" couldn't be overrided";
				throw std::exception(err.c_str());
			}
			if (method.second->GetAccessModifier() != parentMethod->GetAccessModifier())
			{
				string err = "Access modifier of method \"" + *method.second->GetName()
					+ "\" in class \"" + GetFullName() + "\" couldn't be changed";
				throw std::exception(err.c_str());
			}

			vector<Variable*> currentParams = method.second->GetParams();
			vector<Variable*> parentParams = parentMethod->GetParams();
			if (currentParams.size() != parentParams.size())
			{
				string err = "There is no method  \"" + *method.second->GetName()
					+ "\" with such params set in parents of class \"" + GetFullName() + "\"";
				throw std::exception(err.c_str());
			}

			for (int i = 0; i < currentParams.size(); i++)
			{
				if (!(*currentParams[i]->type == *parentParams[i]->type))
				{
					string err = "There is no method \"" + *method.second->GetName()
						+ "\" with such params set in parents of class \"" + GetFullName() + "\"";
					throw std::exception(err.c_str());
				}
			}
		}
		else if (method.second->IsAbstract())
		{
			if (!IsAbstract())
			{
				string err = "Class \"" + GetFullName()
					+ "\" contains abstract method \"" + *method.second->GetName() + "\" should be abstract";
				throw std::exception(err.c_str());
			}
			if (method.second->GetBody() != NULL)
			{
				string err = "Abstract method \"" + *method.second->GetName()
					+ "\" in class \"" + GetFullName() + "\" could not have realisation";
				throw std::exception(err.c_str());
			}
		}
		else if (*method.second->GetName() != "<init>")
		{
			MethodTable* parentMethod = parent->GetMethod(*method.second->GetName());
			if (parentMethod != NULL)
			{
				string err = "Method \"" + *method.second->GetName()
					+ "\" in class \"" + GetFullName() + "\" should have \"override\" modifier";
				throw std::exception(err.c_str());
			}
		}
	}

	if (!IsAbstract())
	{
		Class* curParent = parent;
		while (curParent != NULL)
		{
			if (curParent->IsAbstract())
			{
				for (auto m : curParent->GetAllMethods())
				{
					if (m->IsAbstract() && !GetMethod(*m->GetName())->IsOverride())
					{
						string err = "Class \"" + GetFullName()
							+ "\" does not have realization of \"" + *m->GetName() + "\" method";
						throw std::exception(err.c_str());
					}
				}
			}
			curParent = curParent->parent;
		}
	}
}

void Class::CheckOverridingFields()
{
	for (auto field : fields)
	{
		FieldTable* parentField = parent->GetField(*field.second->GetName());
		if (parentField != NULL && parentField->GetAccessModifier() != e_PRIVATE)
		{
			string err = "Field \"" + *field.second->GetName()
				+ "\" in class \"" + GetFullName() + "\" hides the field of the parent class";
			throw std::exception(err.c_str());
		}
	}
}

void Class::AppendMethod(string* name, DataType* returnType, vector<Variable*> params)
{
	MethodTable* newMethod = new MethodTable(name, returnType);
	for (auto param : params)
	{
		newMethod->AddParam(param->name, param->type);
	}
	AppendUtf8Constant(newMethod->GetName());
	AppendUtf8Constant(newMethod->GetDescriptor());
	methods[*name] = newMethod;
}

void Class::AppendMainMethod(MethodTable* methodInfo)
{
	if (*methodInfo->GetName() != "Main" || !methodInfo->IsStatic())
	{
		return;
	}

	if (*methodInfo->GetDescriptor() != "([Ljava/lang/String;)V")
	{
		return;
	}

	string* mainName = new string("main");
	DataType* mainReturn = new DataType(DataType::t_VOID, NULL, false, this);
	Variable* mainParam = new Variable();
	mainParam->name = new string("args");
	mainParam->type = new DataType(DataType::t_STRING, NULL, true, this);
	AppendMethod(mainName, mainReturn, vector<Variable*> { mainParam });
	methods["main"]->SetStatic(true);
	methods["main"]->SetAccessModifier(e_PUBLIC);

	Argument* arg = new Argument(
		new Expression(Expression::t_ID, mainParam->name));
	Expression* invokation = new InvocationExpression(
		new Expression(Expression::t_ID, new string("Main")), new ArgumentList(arg));
	Statement* stmt = new Statement(Statement::t_EXPRESSION, invokation);
	methods["main"]->SetBody(new StatementList(stmt));

	isMain = true;
}

Class::Class(string* name, AbstractNamespaceMember* outer, ClassDeclaration* decl)
{
	this->id = ++maxTabelId;
	this->decl = decl;
	this->outerMember = outer;
	this->name = name;

	AppendUtf8Constant(new string("Code"));
	AppendClassConstant(this);
}

int Class::GetId()
{
	return id;
}

string* Class::GetName()
{
	return name;
}

MethodTable* Class::GetMethod(string name)
{
	MethodTable* method = NULL;
	Class* current = this;
	while (method == NULL && current != NULL)
	{
		if (current->methods.count(name) > 0)
		{
			method = current->methods[name];
		}
		current = current->parent;
	}
	return method;
}

FieldTable* Class::GetField(string name)
{
	FieldTable* field = NULL;
	Class* current = this;
	while (field == NULL && current != NULL)
	{
		if (current->fields.count(name) > 0)
		{
			field = current->fields[name];
		}
		current = current->parent;
	}
	return field;
}

vector<FieldTable*> Class::GetAllFields()
{
	vector<FieldTable*> fls;
	for (auto field : fields)
	{
		fls.push_back(field.second);
	}
	return fls;
}

vector<MethodTable*> Class::GetAllMethods()
{
	vector<MethodTable*> mtds;
	for (auto method : methods)
	{
		mtds.push_back(method.second);
	}
	return mtds;
}

void Class::SetStatic(bool value)
{
	if (value && isAbstract)
	{
		string err = "Abstract class \"" + GetFullName() + "\" can not be static";
		throw std::exception(err.c_str());
	}
	if (value)
	{
		string err = "Unsupported modifier \"static\" of class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}
	isStatic = value;
}

bool Class::IsStatic()
{
	return isStatic;
}

void Class::SetAbstract(bool value)
{
	if (value && isStatic)
	{
		string err = "Static class \"" + GetFullName() + "\" can not be abstract";
		throw std::exception(err.c_str());
	}
	isAbstract = value;
}

bool Class::IsAbstract()
{
	return isAbstract;
}

void Class::SetAccesModifier(AccessModifier modifier)
{
	if (accessModifier != e_NONE)
	{
		string err = "Class \"" + GetFullName() + "\" can not have more than one acces modifier";
		throw std::exception(err.c_str());
	}
	if ((modifier == e_PRIVATE || modifier == e_PROTECTED) && dynamic_cast<Namespace*>(outerMember))
	{
		string err = "Class \"" + GetFullName() + "\" can be internal or public";
		throw std::exception(err.c_str());
	}
	if (modifier == e_INTERNAL && dynamic_cast<Class*>(outerMember))
	{
		string err = "Class \"" + GetFullName() + "\" can not be internal";
		throw std::exception(err.c_str());
	}

	if (modifier == e_INTERNAL)
	{
		string err = "Unsupported modifier \"internal\" of class \"" + GetFullName() + "\"";
		throw std::exception(err.c_str());
	}

	accessModifier = modifier;
}

AccessModifier Class::GetAccessModifier()
{
	return accessModifier;
}

bool Class::IsMain()
{
	return isMain;
}

void Class::CreateTables()
{
	if (decl == NULL)
	{
		return;
	}

	if (decl->typeName != NULL)
	{
		AppendParent(decl->typeName);
	}
	else
	{
		TypeName* typeName = new TypeName(new string("System"));
		typeName = TypeName::Append(typeName, new string("Object"));
		AppendParent(typeName);
	}

	if (decl->classMemberList == NULL)
	{
		AppdendDefaultConstructor();
		return;
	}

	for (auto classMember : *decl->classMemberList->members)
	{
		if (classMember->type == ClassMember::t_FIELD)
		{
			AppendField((Field*)classMember);
		}
		else if (classMember->type == ClassMember::t_METHOD)
		{
			AppendMethod((Method*)classMember);
		}
		else if (classMember->type == ClassMember::t_CONSTRUCTOR)
		{
			AppendConstructor((Constructor*)classMember);
		}
	}

	if (methods.count(string("<init>")) == 0)
	{
		AppdendDefaultConstructor();
	}
}

AbstractNamespaceMember* Class::GetOuterMember()
{
	return outerMember;
}

AbstractNamespaceMember* Class::GetInnerMember(string* name)
{
	AbstractNamespaceMember* neededMember = NULL;
	for (auto member : innerMembers)
	{
		if (*member->GetName() == *name)
		{
			neededMember = member;
		}
	}
	return neededMember;
}

vector<AbstractNamespaceMember*> Class::GetAllMembers()
{
	vector<AbstractNamespaceMember*> result;
	for (auto member : innerMembers)
	{
		result.push_back(member);
	}
	return result;
}

void Class::Append(AbstractNamespaceMember* member)
{
	if (GetInnerMember(member->GetName()) == NULL)
	{
		innerMembers.push_back((Class*)member);
	}
}

bool Class::InstanceOf(Class* other)
{
	if (this->GetFullName() == other->GetFullName())
	{
		return true;
	}

	if (parent == NULL)
	{
		return false;
	}

	return parent->InstanceOf(other);
}

string* Class::ToDOT()
{
	string* dotStr = new string();
	*dotStr = to_string(id) + "[label=\"" + GetFullName() + "\"];\n";
	for (auto member : innerMembers)
	{
		*dotStr += *member->ToDOT();
		*dotStr += to_string(id) + "--" + to_string(member->GetId()) + ";\n";
	}
	return dotStr;
}

string Class::ToString()
{
	string str = GetFullName() + "," + GetAccessModifierName(accessModifier)
		+ "," + to_string(IsStatic()) + "," + to_string(IsAbstract());

	if (parent != NULL)
	{
		str += "," + parent->GetFullName();
	}

	return str;
}

void Class::WriteTablesFile()
{
	std::filesystem::create_directories(GetFullName());
	ofstream file;
	file.open(GetFullName() + "/declaration.csv");
	file << "name,access modifier,static,abstract,parent\n";
	file << ToString() << "\n";
	file.close();

	file.open(GetFullName() + "/fields.csv");
	file << "name,descriptor,access modifier,static\n";
	for (auto field : GetAllFields())
	{
		file << field->ToString() << "\n";
	}
	file.close();

	file.open(GetFullName() + "/methods.csv");
	file << "name,descriptor,access modifier,static,abstract,virtual,override" << "\n";
	for (auto method : GetAllMethods())
	{
		file << method->ToString() << "\n";
	}
	file.close();

	file.open(GetFullName() + "/constants.csv");
	file << "num,type,value\n";
	for (int i = 1; i < constantTable.size(); i++)
	{
		file << "#" + to_string(i) + "," + constantTable[i]->ToCSV() << "\n";
	}
	file.close();
}

Constant::Constant(Type type)
{
	this->type = type;
}

Constant::Constant(Type type, string* utf8)
{
	this->type = type;
	this->utf8 = utf8;
}

Constant::Constant(Type type, int integer)
{
	this->type = type;
	this->integer = integer;
}

Constant::Constant(Type type, int firstRef, int secondRef)
{
	this->type = type;
	this->firstRef = firstRef;
	this->secondRef = secondRef;
}

bool Constant::operator==(const Constant& other) const
{
	bool isEqual = this->type == other.type;

	switch (type)
	{
	case Constant::t_UTF8:
		isEqual = isEqual && *this->utf8 == *other.utf8;
		break;
	case Constant::t_INTEGER:
		isEqual = isEqual && this->integer == other.integer;
		break;
	case Constant::t_STRING:
	case Constant::t_CLASS:
		isEqual = isEqual && this->firstRef == other.firstRef;
		break;
	case Constant::t_NAME_AND_TYPE:
	case Constant::t_FIELD_REF:
	case Constant::t_METHOD_REF:
		isEqual = isEqual && this->firstRef == other.firstRef && this->secondRef == other.secondRef;
		break;
	default:
		break;
	}

	return isEqual;
}

string Constant::ToCSV()
{
	switch (type)
	{
	case Constant::t_UTF8:    return "UTF8," + *utf8;
	case Constant::t_INTEGER: return "INT," + to_string(integer);
	case Constant::t_STRING:  return "STRING,#" + to_string(firstRef);
	case Constant::t_CLASS:   return "CLASS,#" + to_string(firstRef);
	case Constant::t_NAME_AND_TYPE: return "N&T,#" + to_string(firstRef) + ",#" + to_string(secondRef);
	case Constant::t_FIELD_REF: return "FIELDREF,#" + to_string(firstRef) + ",#" + to_string(secondRef);
	case Constant::t_METHOD_REF: return "METHODREF,#" + to_string(firstRef) + ",#" + to_string(secondRef);
	default:
		break;
	}
	return "";
}

int Constant::ToByteCode(vector<char>* byteCode)
{
	int size = byteCode->size();
	char* byteArr;
	switch (type)
	{
	case Constant::t_UTF8:
		byteCode->push_back(0x01);
		byteArr = IntToByteCode(utf8->length());
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		for (auto c : *utf8)
		{
			byteCode->push_back(c);
		}
		break;
	case Constant::t_INTEGER:
		byteCode->push_back(0x03);
		byteArr = IntToByteCode(integer);
		for (int i = 0; i < 4; i++)
		{
			byteCode->push_back(byteArr[i]);
		}
		break;
	case Constant::t_STRING:
		byteCode->push_back(0x08);
		byteArr = IntToByteCode(firstRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		break;
	case Constant::t_NAME_AND_TYPE:
		byteCode->push_back(0x0c);
		byteArr = IntToByteCode(firstRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		byteArr = IntToByteCode(secondRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		break;
	case Constant::t_CLASS:
		byteCode->push_back(0x07);
		byteArr = IntToByteCode(firstRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		break;
	case Constant::t_FIELD_REF:
		byteCode->push_back(0x09);
		byteArr = IntToByteCode(firstRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		byteArr = IntToByteCode(secondRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		break;
	case Constant::t_METHOD_REF:
		byteCode->push_back(0x0a);
		byteArr = IntToByteCode(firstRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		byteArr = IntToByteCode(secondRef);
		byteCode->push_back(byteArr[2]);
		byteCode->push_back(byteArr[3]);
		break;
	default:
		break;
	}
	return byteCode->size() - size;
}

char* Constant::IntToByteCode(int integer)
{
	char* bytes = new char[4];
	for (int i = 0; i < 4; i++)
	{
		bytes[3 - i] = (integer >> (i * 8));
	}
	return bytes;
}

DataType::DataType(Type type, Class* classType, bool isArray, Class* namespaceMember)
{
	this->type = type;
	this->classType = classType;
	this->isArray = isArray;

	if (classType == NULL)
	{
		this->classType = GetClassOfType(type, namespaceMember);
	}
}

DataType::Type DataType::GetType(SimpleType* simpleType)
{
	switch (simpleType->type)
	{
	case SimpleType::t_BOOL:   return DataType::t_BOOL;
	case SimpleType::t_CHAR:   return DataType::t_CHAR;
	case SimpleType::t_INT:    return DataType::t_INT;
	case SimpleType::t_STRING: return DataType::t_STRING;
	}
}

string* DataType::ToString()
{
	string* str = new string();
	switch (type)
	{
	case DataType::t_INT:      *str = "INT";    break;
	case DataType::t_STRING:   *str = "STRING"; break;
	case DataType::t_BOOL:     *str = "BOOL";   break;
	case DataType::t_CHAR:	   *str = "CHAR";   break;
	case DataType::t_VOID:	   *str = "VOID";   break;
	case DataType::t_TYPENAME: *str = classType->GetFullName(); break;
	default: break;
	}

	if (isArray) { *str += "[]"; }

	return str;
}

string DataType::ToDescriptor()
{
	string str = "";
	switch (type)
	{
	case DataType::t_INT:
	case DataType::t_BOOL:
	case DataType::t_CHAR:     str = "I"; break;
	case DataType::t_STRING:   str = "Ljava/lang/String;"; break;
	case DataType::t_TYPENAME: str = "L" + classType->GetFullName() + ";"; break;
	case DataType::t_VOID:	   str = "V"; break;
	default: break;
	}

	if (isArray) str = "[" + str;
	return str;
}

bool DataType::operator==(const DataType& other) const
{
	bool isTypeEquals = this->type == other.type || this->type == t_INT && other.type == t_CHAR;
	if (isTypeEquals && this->type == t_TYPENAME)
	{
		isTypeEquals = other.classType->InstanceOf(this->classType);
	}
	return isTypeEquals && this->isArray == other.isArray;
}

Class* DataType::GetClassOfType(Type type, Class* namespaceMember)
{
	Class* classType = NULL;
	TypeName* tName = new TypeName(new string("System"));
	switch (type)
	{
	case DataType::t_INT:
		tName = TypeName::Append(tName, new string("Int"));
		classType = namespaceMember->FindClass(tName);
		break;
	case DataType::t_STRING:
		tName = TypeName::Append(tName, new string("String"));
		classType = namespaceMember->FindClass(tName);
		break;
	case DataType::t_BOOL:
		tName = TypeName::Append(tName, new string("Bool"));
		classType = namespaceMember->FindClass(tName);
		break;
	case DataType::t_CHAR:
		tName = TypeName::Append(tName, new string("Char"));
		classType = namespaceMember->FindClass(tName);
		break;
	case DataType::t_TYPENAME:
	case DataType::t_VOID:
	default:
		break;
	}
	return classType;
}

Expression* FieldTable::GetDefaultInitializer()
{
	Expression* expr = NULL;
	if (!type->isArray)
	{
		switch (type->type)
		{
		case DataType::t_BOOL:
			expr = new Expression(Expression::t_BOOL_LITER, false); break;
		case DataType::t_INT:
			expr = new Expression(Expression::t_INT_LITER, 0); break;
		case DataType::t_CHAR:
			expr = new Expression(Expression::t_CHAR_LITER, 0); break;
		case DataType::t_STRING:
			expr = new Expression(Expression::t_STRING_LITER, new string("")); break;
		default: break;
		}
	}
	return expr;
}

FieldTable::FieldTable(string* name, DataType* type)
{
	this->name = name;
	this->type = type;
}

void FieldTable::SetStatic(bool value)
{
	this->isStatic = value;
}

bool FieldTable::IsStatic()
{
	return isStatic;
}

void FieldTable::SetAccessModifier(AccessModifier modifier)
{
	if (this->accessModifier != e_NONE)
	{
		string err = "Filed \"" + *GetName() + "\" can not have more than one acces modifier";
		throw std::exception(err.c_str());
	}
	if (modifier == e_INTERNAL)
	{
		string err = "Unsupported modifier \"internal\" of field \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}
	this->accessModifier = modifier;
}

AccessModifier FieldTable::GetAccessModifier()
{
	return accessModifier;
}

void FieldTable::SetInitializer(Expression* expr)
{
	if (expr == NULL)
	{
		initializer = GetDefaultInitializer();
	}
	else
	{
		initializer = expr;
	}
}

Expression* FieldTable::GetInitializer()
{
	return initializer;
}

string* FieldTable::GetName()
{
	return name;
}

DataType* FieldTable::GetType()
{
	return type;
}

string FieldTable::ToString()
{
	return *GetName() + "," + GetType()->ToDescriptor()
		+ "," + GetAccessModifierName(GetAccessModifier()) + "," + to_string(IsStatic());
}

void FieldTable::ToByteCode(Class* owner, vector<char>* byteCode)
{
	char accessFlag = 0x00;
	switch (accessModifier)
	{
	case e_PRIVATE:
		accessFlag = 0x02;
		break;
	case e_PROTECTED:
		accessFlag = 0x04;
		break;
	case e_PUBLIC:
		accessFlag = 0x01;
		break;
	default:
		break;
	}

	if (IsStatic())
	{
		accessFlag += 0x08;
	}

	byteCode->push_back(0x00);
	byteCode->push_back(accessFlag);

	char* nameConst = Constant::IntToByteCode(owner->AppendUtf8Constant(name));
	byteCode->push_back(nameConst[2]);
	byteCode->push_back(nameConst[3]);

	char* descConst = Constant::IntToByteCode(owner->AppendUtf8Constant(new string(type->ToDescriptor())));
	byteCode->push_back(descConst[2]);
	byteCode->push_back(descConst[3]);

	byteCode->push_back(0x00);
	byteCode->push_back(0x00);
}

string GetAccessModifierName(AccessModifier modifier)
{
	switch (modifier)
	{
	case e_PRIVATE:   return "private";
	case e_PROTECTED: return "protected";
	case e_INTERNAL:  return "internal";
	case e_PUBLIC:	  return "public";
	default: return "";
	}
}

MethodTable::MethodTable(string* name, DataType* dataType)
{
	this->name = name;
	this->returnValue = dataType;
}

void MethodTable::SetStatic(bool value)
{
	if (IsAbstract() || IsVirtual() || IsOverride())
	{
		string err = "Illegal modifier \"static\" of method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}
	isStatic = value;
}

bool MethodTable::IsStatic()
{
	return isStatic;
}

void MethodTable::SetAbstract(bool value)
{
	if (IsStatic() || IsVirtual() || IsOverride())
	{
		string err = "Illegal modifier \"abstract\" of method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}
	isAbstract = value;
}

bool MethodTable::IsAbstract()
{
	return isAbstract;
}

void MethodTable::SetVirtual(bool value)
{
	if (IsStatic() || IsAbstract() || IsOverride())
	{
		string err = "Illegal modifier \"virtual\" of method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}
	isVirtual = value;
}

bool MethodTable::IsVirtual()
{
	return isVirtual;
}

void MethodTable::SetOverride(bool value)
{
	if (IsAbstract() || IsStatic() || IsVirtual())
	{
		string err = "Illegal modifier \"override\" of method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}
	isOverride = value;
}

bool MethodTable::IsOverride()
{
	return isOverride;
}

void MethodTable::SetAccessModifier(AccessModifier modifier)
{
	if (accessModifier != e_NONE)
	{
		string err = "Method \"" + *GetName() + "\" can not have more than one access modifier";
		throw std::exception(err.c_str());
	}
	if (modifier == e_INTERNAL)
	{
		string err = "Unsupported modifier \"internal\" of method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}
	accessModifier = modifier;
}

AccessModifier MethodTable::GetAccessModifier()
{
	return accessModifier;
}

void MethodTable::AddParam(string* name, DataType* type)
{
	if (GetParam(name) != NULL)
	{
		string err = "Duplicated param name \"" + *name + "\" in method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}

	Variable* newParam = new Variable();
	newParam->name = name;
	newParam->type = type;
	params.push_back(newParam);
}

Variable* MethodTable::GetParam(string* name)
{
	for (auto param : params)
	{
		if (*param->name == *name)
		{
			return param;
		}
	}
	return NULL;
}

void MethodTable::SetBody(StatementList* body)
{
	this->body = body;
}

StatementList* MethodTable::GetBody()
{
	return body;
}

DataType* MethodTable::GetReturnValue()
{
	return returnValue;
}

string* MethodTable::GetName()
{
	return name;
}

vector<Variable*> MethodTable::GetParams()
{
	return params;
}

void MethodTable::AddLocalVariable(string* name, DataType* type)
{
	if (GetLocalVariable(name) != NULL)
	{
		string err = "Duplicated local variable name \"" + *name + "\" in method \"" + *GetName() + "\"";
		throw std::exception(err.c_str());
	}

	Variable* newParam = new Variable();
	newParam->name = name;
	newParam->type = type;
	localVariables.push_back(newParam);
}

Variable* MethodTable::GetLocalVariable(string* varName)
{
	Variable* var = NULL;
	for (auto param : params)
	{
		if (*param->name == *varName)
		{
			var = param;
		}
	}
	if (var == NULL)
	{
		for (auto locVar : localVariables)
		{
			if (*locVar->name == *varName)
			{
				var = locVar;
			}
		}
	}
	return var;
}

void MethodTable::Semantic(Class* owner)
{
	if (body != NULL)
	{
		bool hasReturn = false;
		for (auto stmt : *body->statements)
		{
			hasReturn = hasReturn || stmt->type == Statement::t_RETURN;
			stmt->Semantic(owner, this);
		}

		if (!hasReturn && owner->GetOuterMember()->GetFullName() != "global/System")
		{
			if (this->returnValue->type == DataType::t_VOID)
			{
				body = StatementList::Append(body, new ReturnStatement());
			}
			else
			{
				string err = "There is not return value in " + *this->name + 
					" method of class " + owner->GetFullName();
				throw std::exception(err.c_str());
			}
		}
	}
	else if (!IsAbstract() && owner->GetOuterMember()->GetFullName() != "global/System")
	{
		if (this->returnValue->type == DataType::t_VOID)
		{
			body = new StatementList(new ReturnStatement());
		}
		else
		{
			string err = "There is not return value in " + *this->name +
				" method of class " + owner->GetFullName();
			throw std::exception(err.c_str());
		}
	}
}

vector<Expression*> MethodTable::SortArguments(ArgumentList* args)
{
	vector<Expression*> sortedArgs = vector<Expression*>(params.size(), NULL);
	int lastArgIndex = 0;
	for (auto arg : *args->arguments)
	{
		if (arg->identifier == NULL)
		{
			if (lastArgIndex >= sortedArgs.size())
			{
				string err = "Too many arguments in method \"" + *GetName() + "\"";
				throw std::exception(err.c_str());
			}
			if (sortedArgs[lastArgIndex] != NULL)
			{
				string err = "Argument \"" + *params[lastArgIndex]->name + "\" just exists in method \"" + *GetName() + "\"";
				throw std::exception(err.c_str());
			}
			sortedArgs[lastArgIndex] = arg->expression;
		}
		else
		{
			int argIndex = GetParamIndex(arg->identifier);
			if (argIndex < 0)
			{
				string err = "No param with such name \"" + *arg->identifier + "\" just exists in method \"" + *GetName() + "\"";
				throw std::exception(err.c_str());
			}
			if (sortedArgs[argIndex] != NULL)
			{
				string err = "Argument \"" + *params[argIndex]->name + "\" just exists in method \"" + *GetName() + "\"";
				throw std::exception(err.c_str());
			}
			sortedArgs[argIndex] = arg->expression;
		}
		lastArgIndex++;
	}
	return sortedArgs;
}

bool MethodTable::CompareArgsTypes(ArgumentList* args)
{
	if (args == NULL)
	{
		return params.size() == 0;
	}

	vector<Expression*> sortedArgs = SortArguments(args);
	for (int i = 0; i < params.size(); i++)
	{
		if (sortedArgs[i] == NULL)
		{
			string err = "Argument \"" + *params[i]->name + "\" not exists in method \"" + *GetName() + "\"";
			throw std::exception(err.c_str());
		}
		if (!(*params[i]->type == *sortedArgs[i]->dataType))
		{
			return false;
		}
	}

	return true;
}

int MethodTable::GetParamIndex(string* name)
{
	for (int i = 0; i < params.size(); i++)
	{
		if (*params[i]->name == *name)
			return i;
	}
	return -1;
}

int MethodTable::GetLocalsCount()
{
	int count = IsStatic() ? 0 : 1;
	count += params.size() + localVariables.size();
	return count;
}

int MethodTable::GetLocalIndex(string* name)
{
	int index = GetParamIndex(name);
	for (int i = 0; i < localVariables.size() && index == -1; i++)
	{
		if (*localVariables[i]->name == *name)
		{
			index = i + params.size();
		}
	}

	if (!IsStatic())
	{
		index++;
	}

	return index;
}

string MethodTable::ToString()
{
	string str = *GetName() + "," + *GetDescriptor() + ","
		+ GetAccessModifierName(accessModifier) + "," + to_string(IsStatic()) + "," + to_string(IsAbstract())
		+ "," + to_string(IsVirtual()) + "," + to_string(IsOverride());

	return str;
}

string* MethodTable::GetDescriptor()
{
	if (constDescriptor != NULL) { return constDescriptor; }

	string* descriptor = new string("(");
	for (auto param : params)
	{
		*descriptor += param->type->ToDescriptor();
	}
	*descriptor += ")" + GetReturnValue()->ToDescriptor();
	return descriptor;
}

void MethodTable::SetConstDescriptor(string* str)
{
	constDescriptor = str;
}

void MethodTable::ToByteCode(Class* owner, vector<char>* byteCode)
{
	char accessFlag = 0x00;
	switch (accessModifier)
	{
	case e_PRIVATE:
		accessFlag = 0x02;
		break;
	case e_PROTECTED:
		accessFlag = 0x04;
		break;
	case e_PUBLIC:
		accessFlag = 0x01;
		break;
	default:
		break;
	}

	if (IsStatic())
	{
		accessFlag += 0x08;
	}

	byteCode->push_back(IsAbstract() ? 0x04 : 0x00);
	byteCode->push_back(accessFlag);

	char* nameConst = Constant::IntToByteCode(owner->AppendUtf8Constant(name));
	byteCode->push_back(nameConst[2]);
	byteCode->push_back(nameConst[3]);

	char* descConst = Constant::IntToByteCode(owner->AppendUtf8Constant(GetDescriptor()));
	byteCode->push_back(descConst[2]);
	byteCode->push_back(descConst[3]);

	char* atributesCount = Constant::IntToByteCode(IsAbstract() ? 0 : 1);
	byteCode->push_back(atributesCount[2]);
	byteCode->push_back(atributesCount[3]);

	if (IsAbstract())
	{
		return;
	}

	char* atributeConst = Constant::IntToByteCode(owner->AppendUtf8Constant(new string("Code")));
	byteCode->push_back(atributeConst[2]);
	byteCode->push_back(atributeConst[3]);

	int atributeLenthIndex = byteCode->size();
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	byteCode->push_back(0x04);
	byteCode->push_back(0x00);

	char* localsCount = Constant::IntToByteCode(GetLocalsCount());
	byteCode->push_back(localsCount[2]);
	byteCode->push_back(localsCount[3]);

	int codeLenthIndex = byteCode->size();
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	int codeLenth = 0;
	if (body != NULL)
	{
		codeLenth = body->ToByteCode(owner, this, byteCode);
	}

	char* atributeLenth = Constant::IntToByteCode(codeLenth + 12);
	byteCode->operator[](atributeLenthIndex) = atributeLenth[0];
	byteCode->operator[](atributeLenthIndex + 1) = atributeLenth[1];
	byteCode->operator[](atributeLenthIndex + 2) = atributeLenth[2];
	byteCode->operator[](atributeLenthIndex + 3) = atributeLenth[3];

	char* codeLenthBytes = Constant::IntToByteCode(codeLenth);
	byteCode->operator[](codeLenthIndex) = codeLenthBytes[0];
	byteCode->operator[](codeLenthIndex + 1) = codeLenthBytes[1];
	byteCode->operator[](codeLenthIndex + 2) = codeLenthBytes[2];
	byteCode->operator[](codeLenthIndex + 3) = codeLenthBytes[3];

	byteCode->push_back(0x00);
	byteCode->push_back(0x00);

	byteCode->push_back(0x00);
	byteCode->push_back(0x00);
}

Class* Class::CreateObjectClass(AbstractNamespaceMember* outer)
{
	Class* objectClass = new Class(new string("Object"), outer, NULL);
	objectClass->SetAccesModifier(e_PUBLIC);

	return objectClass;
}

Class* Class::CreateStringClass(AbstractNamespaceMember* outer)
{
	Class* stringClass = new Class(new string("String"), outer, NULL);
	stringClass->SetAccesModifier(e_PUBLIC);
	stringClass->AppendParent(new TypeName(new string("Object")));

	return stringClass;
}

Class* Class::CreateIntClass(AbstractNamespaceMember* outer)
{
	Class* intClass = new Class(new string("Int"), outer, NULL);
	intClass->SetAccesModifier(e_PUBLIC);
	intClass->AppendParent(new TypeName(new string("Object")));

	return intClass;
}

Class* Class::CreateCharClass(AbstractNamespaceMember* outer)
{
	Class* charClass = new Class(new string("Char"), outer, NULL);
	charClass->SetAccesModifier(e_PUBLIC);
	charClass->AppendParent(new TypeName(new string("Object")));

	return charClass;
}

Class* Class::CreateBoolClass(AbstractNamespaceMember* outer)
{
	Class* boolClass = new Class(new string("Bool"), outer, NULL);
	boolClass->SetAccesModifier(e_PUBLIC);
	boolClass->AppendParent(new TypeName(new string("Object")));

	return boolClass;
}

Class* Class::CreateConsoleClass(AbstractNamespaceMember* outer)
{
	Class* consoleClass = new Class(new string("Console"), outer, NULL);
	consoleClass->SetAccesModifier(e_PUBLIC);
	consoleClass->AppendParent(new TypeName(new string("Object")));

	return consoleClass;
}

void Class::FillObjectClass(AbstractNamespaceMember* outer)
{
	Class* objectClass = (Class*)outer->GetInnerMember(new string("Object"));

	DataType* equalsReturn = new DataType(DataType::t_BOOL, NULL, false, objectClass);
	DataType* equalsParamType = new DataType(DataType::t_TYPENAME, objectClass, false, objectClass);
	Variable* equalsParam = new Variable();
	equalsParam->name = new string("obj");
	equalsParam->type = equalsParamType;
	vector<Variable*> equalsParamSet = vector<Variable*>{ equalsParam };

	objectClass->AppendMethod(new string("Equals"), equalsReturn, equalsParamSet);
	MethodTable* equalsMethod = objectClass->methods["Equals"];
	equalsMethod->SetAccessModifier(e_PUBLIC);
	equalsMethod->SetVirtual(true);

	DataType* toStringReturn = new DataType(DataType::t_STRING, NULL, false, objectClass);
	vector<Variable*> toStringParamSet = vector<Variable*>();

	objectClass->AppendMethod(new string("ToString"), toStringReturn, toStringParamSet);
	MethodTable* toStringMethod = objectClass->methods["ToString"];
	toStringMethod->SetAccessModifier(e_PUBLIC);
	toStringMethod->SetVirtual(true);

	objectClass->AppdendDefaultConstructor();
	objectClass->GetMethod("<init>")->SetBody(NULL);
}

void Class::FillStringClass(AbstractNamespaceMember* outer)
{
	Class* stringClass = (Class*)outer->GetInnerMember(new string("String"));
	stringClass->AppdendDefaultConstructor();

	DataType* toStringReturn = new DataType(DataType::t_STRING, NULL, false, stringClass);
	vector<Variable*> toStringParamSet = vector<Variable*>();

	stringClass->AppendMethod(new string("ToString"), toStringReturn, toStringParamSet);
	MethodTable* toStringMethod = stringClass->methods["ToString"];
	toStringMethod->SetAccessModifier(e_PUBLIC);
	toStringMethod->SetOverride(true);
	toStringMethod->SetConstDescriptor(new string("(Ljava/lang/String;)Ljava/lang/String;"));
}

void Class::FillIntClass(AbstractNamespaceMember* outer)
{
	Class* intClass = (Class*)outer->GetInnerMember(new string("Int"));
	intClass->AppdendDefaultConstructor();

	DataType* parseReturn = new DataType(DataType::t_INT, NULL, false, intClass);
	DataType* parseParamType = new DataType(DataType::t_STRING, NULL, false, intClass);
	Variable* parseParam = new Variable();
	parseParam->name = new string("str");
	parseParam->type = parseParamType;
	vector<Variable*> parseParamSet = vector<Variable*>{ parseParam };

	intClass->AppendMethod(new string("Parse"), parseReturn, parseParamSet);
	MethodTable* equalsMethod = intClass->methods["Parse"];
	equalsMethod->SetAccessModifier(e_PUBLIC);
	equalsMethod->SetStatic(true);

	DataType* toStringReturn = new DataType(DataType::t_STRING, NULL, false, intClass);
	vector<Variable*> toStringParamSet = vector<Variable*>();

	intClass->AppendMethod(new string("ToString"), toStringReturn, toStringParamSet);
	MethodTable* toStringMethod = intClass->methods["ToString"];
	toStringMethod->SetAccessModifier(e_PUBLIC);
	toStringMethod->SetOverride(true);
	toStringMethod->SetConstDescriptor(new string("(I)Ljava/lang/String;"));
}

void Class::FillCharClass(AbstractNamespaceMember* outer)
{
	Class* charClass = (Class*)outer->GetInnerMember(new string("Char"));
	charClass->AppdendDefaultConstructor();

	DataType* toStringReturn = new DataType(DataType::t_STRING, NULL, false, charClass);
	vector<Variable*> toStringParamSet = vector<Variable*>();

	charClass->AppendMethod(new string("ToString"), toStringReturn, toStringParamSet);
	MethodTable* toStringMethod = charClass->methods["ToString"];
	toStringMethod->SetAccessModifier(e_PUBLIC);
	toStringMethod->SetOverride(true);
	toStringMethod->SetConstDescriptor(new string("(I)Ljava/lang/String;"));
}

void Class::FillBoolClass(AbstractNamespaceMember* outer)
{
	Class* boolClass = (Class*)outer->GetInnerMember(new string("Bool"));
	boolClass->AppdendDefaultConstructor();

	DataType* toStringReturn = new DataType(DataType::t_STRING, NULL, false, boolClass);
	vector<Variable*> toStringParamSet = vector<Variable*>();

	boolClass->AppendMethod(new string("ToString"), toStringReturn, toStringParamSet);
	MethodTable* toStringMethod = boolClass->methods["ToString"];
	toStringMethod->SetAccessModifier(e_PUBLIC);
	toStringMethod->SetOverride(true);
	toStringMethod->SetConstDescriptor(new string("(I)Ljava/lang/String;"));
}

void Class::FillConsoleClass(AbstractNamespaceMember* outer)
{
	Class* consoleClass = (Class*)outer->GetInnerMember(new string("Console"));
	consoleClass->AppdendDefaultConstructor();

	DataType* writeReturn = new DataType(DataType::t_VOID, NULL, false, consoleClass);
	DataType* writeParamType = new DataType(DataType::t_STRING, NULL, false, consoleClass);
	Variable* writeParam = new Variable();
	writeParam->name = new string("str");
	writeParam->type = writeParamType;
	vector<Variable*> writeParamSet = vector<Variable*>{ writeParam };

	consoleClass->AppendMethod(new string("WriteLine"), writeReturn, writeParamSet);
	MethodTable* writeMethod = consoleClass->methods["WriteLine"];
	writeMethod->SetAccessModifier(e_PUBLIC);
	writeMethod->SetStatic(true);

	DataType* readReturn = new DataType(DataType::t_STRING, NULL, false, consoleClass);
	vector<Variable*> readParamSet = vector<Variable*>{ };

	consoleClass->AppendMethod(new string("ReadLine"), readReturn, readParamSet);
	MethodTable* readMethod = consoleClass->methods["ReadLine"];
	readMethod->SetAccessModifier(e_PUBLIC);
	readMethod->SetStatic(true);
}

int Class::IndexOfConstant(Constant* constant)
{
	int index = 0;
	for (auto c : constantTable)
	{
		if (*c == *constant)
		{
			return index;
		}
		index++;
	}

	constantTable.push_back(constant);
	return index;
}

void Class::AppendConstatntToByteCode()
{
	char* size = Constant::IntToByteCode(constantTable.size());
	byteCode.push_back(size[2]);
	byteCode.push_back(size[3]);

	for (int i = 1; i < constantTable.size(); i++)
	{
		constantTable[i]->ToByteCode(&byteCode);
	}
}

void Class::AppendClassInformationToByteCode()
{
	byteCode.push_back(IsAbstract() ? 0x04 : 0x00);
	byteCode.push_back(0x21);

	char* classConstant = Constant::IntToByteCode(AppendClassConstant(this));
	byteCode.push_back(classConstant[2]);
	byteCode.push_back(classConstant[3]);

	char* parentConstant = Constant::IntToByteCode(AppendClassConstant(parent));
	byteCode.push_back(parentConstant[2]);
	byteCode.push_back(parentConstant[3]);

	byteCode.push_back(0x00);
	byteCode.push_back(0x00);
}

void Class::AppendFieldsTableToByteCode()
{
	char* fieldCount = Constant::IntToByteCode(fields.size());
	byteCode.push_back(fieldCount[2]);
	byteCode.push_back(fieldCount[3]);

	for (auto field : fields)
	{
		field.second->ToByteCode(this, &byteCode);
	}
}

void Class::AppendMethodsTableToByteCode()
{
	char* methodCount = Constant::IntToByteCode(methods.size());
	byteCode.push_back(methodCount[2]);
	byteCode.push_back(methodCount[3]);

	for (auto method : methods)
	{
		method.second->ToByteCode(this, &byteCode);
	}
}

void Class::CreateRTLClasses(AbstractNamespaceMember* outer)
{
	AbstractNamespaceMember* system = new Namespace(new string("System"), outer);
	outer->Append(system);

	system->Append(CreateObjectClass(system));
	system->Append(CreateStringClass(system));
	system->Append(CreateIntClass(system));
	system->Append(CreateCharClass(system));
	system->Append(CreateBoolClass(system));
	system->Append(CreateConsoleClass(system));

	FillObjectClass(system);
	FillIntClass(system);
	FillCharClass(system);
	FillStringClass(system);
	FillBoolClass(system);
	FillConsoleClass(system);
}

int Class::AppendUtf8Constant(string* utf8)
{
	Constant* constant = new Constant(Constant::t_UTF8, utf8);
	int index = IndexOfConstant(constant);
	return index;
}

int Class::AppendIntegerConstant(int integer)
{
	Constant* constant = new Constant(Constant::t_INTEGER, integer);
	int index = IndexOfConstant(constant);
	return index;
}

int Class::AppendStringConstant(string* utf8)
{
	int utf8Const = AppendUtf8Constant(utf8);
	Constant* constant = new Constant(Constant::t_STRING, utf8Const, -1);
	int index = IndexOfConstant(constant);
	return index;
}

int Class::AppendClassConstant(Class* classInfo)
{
	int utf8Const = AppendUtf8Constant(new string(classInfo->GetFullName()));
	Constant* constant = new Constant(Constant::t_CLASS, utf8Const, -1);
	int index = IndexOfConstant(constant);
	return index;
}

int Class::AppendNameAndTypeConstant(string* name, string* descriptor)
{
	int utf8Name = AppendUtf8Constant(name);
	int utf8Type = AppendUtf8Constant(descriptor);
	Constant* constant = new Constant(Constant::t_NAME_AND_TYPE, utf8Name, utf8Type);
	int index = IndexOfConstant(constant);
	return index;
}

int Class::AppendFieldRefConstant(Class* owner, FieldTable* fieldInfo)
{
	int classConst = AppendClassConstant(owner);
	int nameAndTypeConst = AppendNameAndTypeConstant(fieldInfo->GetName(), new string(fieldInfo->GetType()->ToDescriptor()));
	Constant* constant = new Constant(Constant::t_FIELD_REF, classConst, nameAndTypeConst);
	int index = IndexOfConstant(constant);
	return index;
}

int Class::AppendMethofRefConstant(Class* owner, MethodTable* methodTable)
{
	int classConst = AppendClassConstant(owner);
	int nameAndTypeConst = AppendNameAndTypeConstant(methodTable->GetName(), methodTable->GetDescriptor());
	Constant* constant = new Constant(Constant::t_METHOD_REF, classConst, nameAndTypeConst);
	int index = IndexOfConstant(constant);
	return index;
}

void Class::WriteClassFile()
{
	byteCode.push_back(0xca);
	byteCode.push_back(0xfe);
	byteCode.push_back(0xba);
	byteCode.push_back(0xbe);

	byteCode.push_back(0x00);
	byteCode.push_back(0x00);
	byteCode.push_back(0x00);
	byteCode.push_back(0x3e);

	AppendConstatntToByteCode();
	AppendClassInformationToByteCode();
	AppendFieldsTableToByteCode();
	AppendMethodsTableToByteCode();

	byteCode.push_back(0x00);
	byteCode.push_back(0x00);

	ofstream file;
	std::filesystem::create_directories("out/" + GetOuterMember()->GetFullName());
	file.open("out/" + GetOuterMember()->GetFullName() + "/" + *GetName() + ".class", std::ios::binary);
	for (char c : byteCode)
	{
		file << c;
	}
	file.close();
}

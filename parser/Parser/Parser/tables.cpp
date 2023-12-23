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
	DataType* dataType = new DataType();
	TypeName* typeName = NULL;
	switch (member->returnValue)
	{
	case ClassMember::t_SIMPLE_TYPE:
		dataType->type = DataType::GetType(member->simpleType);
		break;
	case ClassMember::t_TYPENAME:
		dataType->type = DataType::t_TYPENAME;
		typeName = member->typeName;
		break;
	case ClassMember::t_VOID:
		dataType->type = DataType::t_VOID;
		break;
	case ClassMember::t_ARRAY:
		if (member->arrayType->type == ArrayType::t_SIMPLE_TYPE) {
			dataType->type = DataType::GetType(member->arrayType->simpleType);
		}
		else {
			dataType->type = DataType::t_TYPENAME;
		}
		typeName = member->arrayType->typeName;
		dataType->isArray = true;
		break;
	default:
		break;
	}

	if (typeName != NULL)
	{
		dataType->classType = FindClass(typeName);
	}

	return dataType;
}

DataType* Class::CreateDataType(VarDeclarator* varDecl)
{
	DataType* dataType = new DataType();
	TypeName* typeName = NULL;
	switch (varDecl->type)
	{
	case VarDeclarator::t_SIMPLE_TYPE:
		dataType->type = DataType::GetType(varDecl->simpleType);
		break;
	case VarDeclarator::t_TYPE_NAME:
		dataType->type = DataType::t_TYPENAME;
		typeName = varDecl->typeName;
		break;
	case VarDeclarator::t_ARRAY_TYPE:
		if (varDecl->arrayType->type == ArrayType::t_SIMPLE_TYPE) {
			dataType->type = DataType::GetType(varDecl->arrayType->simpleType);
		}
		else {
			dataType->type = DataType::t_TYPENAME;
		}
		typeName = varDecl->arrayType->typeName;
		dataType->isArray = true;
		break;
	default:
		break;
	}

	if (typeName != NULL)
	{
		dataType->classType = FindClass(typeName);
	}

	return dataType;
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

	DataType* dataType = new DataType();
	dataType->type = DataType::t_VOID;

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

	DataType* dataType = new DataType();
	dataType->type = DataType::t_VOID;

	MethodTable* newConstructor = new MethodTable(constructorName, dataType);
	newConstructor->SetAccessModifier(e_PUBLIC);
	AppendFieldInitializers(newConstructor, NULL);

	if (decl != NULL)
	{
		ClassMemberList::Append(decl->classMemberList, 
			new Constructor(NULL, GetName(), NULL, ClassMember::t_NULL, newConstructor->GetBody()));
	}

	methods[*constructorName] = newConstructor;
}

void Class::AppendParent(TypeName* parentName)
{
	parent = FindClass(parentName);;
}

void Class::AppendFieldInitializers(MethodTable* constructor, ArgumentList* args)
{
	for (auto field : GetAllFields())
	{
		if (!field->IsStatic() && field->GetInitializer() != NULL)
		{
			Expression* fieldName = new Expression(Expression::t_ID, field->GetName());
			Expression* initExpr = new Expression(Expression::t_ASSIGNMENT, fieldName, field->GetInitializer());
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
		if (method.second->IsAbstract())
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
	}
}

void Class::AppendMethod(string* name, DataType* returnType, vector<Variable*> params)
{
	MethodTable* newMethod = new MethodTable(name, returnType);
	for (auto param : params)
	{
		newMethod->AddParam(param->name, param->type);
	}
	methods[*name] = newMethod;
}

Class::Class(string* name, AbstractNamespaceMember* outer, ClassDeclaration* decl)
{
	this->id = ++maxTabelId;
	this->decl = decl;
	this->outerMember = outer;
	this->name = name;
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
	if (fields.count(name) > 0)
	{
		field = fields[name];
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
	accessModifier = modifier;
}

AccessModifier Class::GetAccessModifier()
{
	return accessModifier;
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
		TypeName* typeName = new TypeName(new string("Object"));
		AppendParent(typeName);
	}

	if (decl->classMemberList == NULL)
	{
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
}

Constant::Constant(Type type)
{
	this->type = type;
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
	case DataType::t_STRING:   str = "L.global/String;"; break;
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

Expression* FieldTable::GetDefaultInitializer()
{
	Expression* expr = NULL;
	switch (type->type)
	{
	case DataType::t_BOOL:
		expr = new Expression(Expression::t_BOOL_LITER, false); break;
	case DataType::t_INT:
		expr = new Expression(Expression::t_INT_LITER, 0); break;
	case DataType::t_CHAR:
		expr = new Expression(Expression::t_CHAR_LITER, 0); break;
	default: break;
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
		for (auto stmt : *body->statements)
		{
			stmt->Semantic(owner, this);
		}
	}
}

bool MethodTable::CompareArgsTypes(ArgumentList* args)
{
	if (args == NULL)
	{
		return params.size() == 0;
	}

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

string MethodTable::ToString()
{
	string descriptor = "(";
	for (auto param : params)
	{
		descriptor += param->type->ToDescriptor();
	}
	descriptor += ")" + GetReturnValue()->ToDescriptor();

	string str = *GetName() + "," + descriptor + ","
		+ GetAccessModifierName(accessModifier) + "," + to_string(IsStatic()) + "," + to_string(IsAbstract())
		+ "," + to_string(IsVirtual()) + "," + to_string(IsOverride());

	return str;
}

Class* Class::CreateObjectClass(AbstractNamespaceMember* outer)
{
	Class* objectClass = new Class(new string("Object"), outer,	NULL);
	objectClass->SetAccesModifier(e_PUBLIC);

	DataType* equalsReturn = new DataType(); 
	equalsReturn->type = DataType::t_BOOL;
	DataType* equalsParamType = new DataType(); 
	equalsParamType->type = DataType::t_TYPENAME;
	equalsParamType->classType = objectClass;
	Variable* equalsParam = new Variable();
	equalsParam->name = new string("obj");
	equalsParam->type = equalsParamType;
	vector<Variable*> equalsParamSet = vector<Variable*>{ equalsParam };

	objectClass->AppendMethod(new string("Equals"), equalsReturn, equalsParamSet);
	MethodTable* equalsMethod = objectClass->methods["Equals"];
	equalsMethod->SetAccessModifier(e_PUBLIC);
	equalsMethod->SetVirtual(true);

	DataType* toStringReturn = new DataType();
	toStringReturn->type = DataType::t_STRING;
	vector<Variable*> toStringParamSet = vector<Variable*>();

	objectClass->AppendMethod(new string("ToString"), toStringReturn, toStringParamSet);
	MethodTable* toStringMethod = objectClass->methods["ToString"];
	toStringMethod->SetAccessModifier(e_PUBLIC);
	toStringMethod->SetVirtual(true);

	objectClass->AppdendDefaultConstructor();
	objectClass->GetMethod("<init>")->SetBody(NULL);
	return objectClass;
}

Class* Class::CreateStringClass(AbstractNamespaceMember* outer)
{
	Class* stringClass = new Class(new string("String"), outer, NULL);
	stringClass->SetAccesModifier(e_PUBLIC);
	stringClass->parent = (Class*)outer->GetInnerMember(new string("Object"));

	stringClass->AppdendDefaultConstructor();
	return stringClass;
}

void Class::CreateRTLClasses(AbstractNamespaceMember* outer)
{
	outer->Append(CreateObjectClass(outer));
	outer->Append(CreateStringClass(outer));
}

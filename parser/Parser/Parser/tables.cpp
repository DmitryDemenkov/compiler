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

DataType* Class::CreateDataType(ClassMember* member)
{
	DataType* dataType = new DataType();
	TypeName* typeName = NULL;
	switch (member->returnValue)
	{
	case Field::t_SIMPLE_TYPE:
		dataType->type = DataType::GetType(member->simpleType);
		break;
	case Field::t_TYPENAME:
		dataType->type = DataType::t_TYPENAME;
		typeName = member->typeName;
		break;
	case Field::t_ARRAY:
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
			throw("Not such identifier");
		}

		dataType->classType = (Class*)neededMember;
	}

	return dataType;
}

void Class::AppendField(Field* field)
{
	DataType* dataType = CreateDataType(field);
	if (fields.count(*field->identifier) > 0 || GetInnerMember(field->identifier) != NULL)
	{
		throw("Identifier already exists");
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
		default: throw("Illigal modifier");
		}
	}

	fields[*newField->GetName()] = newField;
}

Class::Class(string* name, AbstractNamespaceMember* outer, ClassDeclaration* decl)
{
	this->id = ++maxTabelId;
	this->decl = decl;
	this->outerMember = outer;
	this->name = name;

	this->constantTable = vector<Constant*>();
	Constant* code = new Constant(Constant::t_UTF8);        code->utf8 = new string("Code");
	Constant* className = new Constant(Constant::t_UTF8);   className->utf8 = new string(GetFullName());
	Constant* classConst = new Constant(Constant::t_CLASS); classConst->firstRef = 1;

	constantTable.push_back(code);
	constantTable.push_back(className);
	constantTable.push_back(classConst);
}

int Class::GetId()
{
	return id;
}

string* Class::GetName()
{
	return name;
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

void Class::SetStatic(bool value)
{
	if (value && isAbstract)
	{
		throw("Abstract class can not be static");
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
		throw("Static class can not be abstract");
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
		throw("Class can not have nore than 1 acces modifier");
	}
	if ((modifier == e_PRIVATE || modifier == e_PROTECTED) && dynamic_cast<Namespace*>(outerMember))
	{
		throw("Class in namespace can be internal or public");
	}
	if (modifier == e_INTERNAL && dynamic_cast<Class*>(outerMember))
	{
		throw("Class in class can not be internal");
	}
	accessModifier = modifier;
}

AccessModifier Class::GetAccessModifier()
{
	return accessModifier;
}

void Class::CreateFields()
{
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
	return GetFullName() + ";" + GetAccessModifierName(accessModifier) + ";abstract:" +
		to_string(IsAbstract()) + ";static:" + to_string(IsStatic());
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
	case DataType::t_TYPENAME: *str = classType->GetFullName(); break;
	default: break;
	}

	if (isArray) { *str += "[]"; }

	return str;
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
		throw("Filed can not have nore than 1 acces modifier");
	}
	this->accessModifier = modifier;
}

AccessModifier FieldTable::GetAccessModifier()
{
	return accessModifier;
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
	return *GetName() + ";" + *GetType()->ToString() 
		+ ";" + GetAccessModifierName(GetAccessModifier()) + ";static:" + to_string(IsStatic());
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

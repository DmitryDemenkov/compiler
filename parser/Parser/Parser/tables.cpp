#include "tables.h"

Namespace::Namespace(string* name, AbstractNamespaceMember* outer)
{
	this->name = name;
	this->outerMember = outer;
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
	*dotStr = *name + ";\n";
	for (auto member : members)
	{
		*dotStr += *member->ToDOT();
		*dotStr += *name + "--" + *member->GetName() + ";\n";
	}
	return dotStr;
}

Class::Class(string* name, AbstractNamespaceMember* outer)
{
	this->constantTable = vector<Constant*>();
	Constant* code = new Constant(Constant::t_UTF8);        code->utf8 = new string("Code");
	Constant* className = new Constant(Constant::t_UTF8);   className->utf8 = name;
	Constant* classConst = new Constant(Constant::t_CLASS); classConst->firstRef = 1;
	nameIndex = 1;

	constantTable.push_back(code);
	constantTable.push_back(className);
	constantTable.push_back(classConst);

	this->outerMember = outer;
}

string* Class::GetName()
{
	return constantTable[nameIndex]->utf8;
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
	accessModifier = modifier;
}

AccessModifier Class::GetAccessModifier()
{
	return accessModifier;
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
	*dotStr = *GetName() + ";\n";
	for (auto member : innerMembers)
	{
		*dotStr += *member->ToDOT();
		*dotStr += *GetName() + "--" + *member->GetName() + ";\n";
	}
	return dotStr;
}

Constant::Constant(Type type)
{
	this->type = type;
}

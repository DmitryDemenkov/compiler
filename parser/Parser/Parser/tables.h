#pragma once

#include <iostream>
#include <vector>
#include <map>
//#include "../../bison/classes.h"
using namespace std;

class FieldTable;
class MethodTable;

class Constant
{
public:
	enum Type
	{
		t_UTF8,
		t_INTEGER,
		t_STRING,
		t_NAME_AND_TYPE,
		t_CLASS,
		t_FIELD_REF,
		t_METHOD_REF
	};

	Type type;
	string* utf8 = NULL;
	int integer = 0;
	int firstRef = -1;
	int secondRef = -1;

	Constant(Type type);
};

enum AccessModifier
{
	e_NONE,
	e_PRIVATE,
	e_PROTECTED,
	e_INTERNAL,
	e_PUBLIC
};

class AbstractNamespaceMember 
{
public:
	virtual string* GetName() = 0;
	AbstractNamespaceMember* GetOuterMember();
	virtual AbstractNamespaceMember* GetInnerMember(string* name) = 0;
	virtual void Append(AbstractNamespaceMember* member) = 0;
	virtual string* ToDOT() = 0;
};

class Namespace : public AbstractNamespaceMember
{
private:
	string* name;
	vector<AbstractNamespaceMember*> members;
	AbstractNamespaceMember* outerMember;

public:
	Namespace(string* name, AbstractNamespaceMember* outer);
	string* GetName() override;
	AbstractNamespaceMember* GetOuterMember();
	AbstractNamespaceMember* GetInnerMember(string* name) override;
	void Append(AbstractNamespaceMember* member) override;
	string* ToDOT() override;
};

class Class : public AbstractNamespaceMember
{
private:
	vector<Constant*> constantTable;
	int nameIndex;
	bool isStatic = false;
	bool isAbstract = false;
	AccessModifier accessModifier = e_NONE;

	Class* parent = NULL;

	map<string, FieldTable*> fields;
	map<string, MethodTable*> methods;

	AbstractNamespaceMember* outerMember = NULL;
	vector<Class*> innerMembers;

public:
	Class(string* name, AbstractNamespaceMember* outer);
	string* GetName() override;
	Class* GetParent();

	MethodTable* GetMethod(string* name);
	FieldTable* GetField(string* name);

	void SetStatic(bool value);
	bool IsStatic();
	void SetAbstract(bool value);
	bool IsAbstract();
	void SetAccesModifier(AccessModifier modifier);
	AccessModifier GetAccessModifier();

	AbstractNamespaceMember* GetInnerMember(string* name) override;
	void Append(AbstractNamespaceMember* member) override;
	string* ToDOT() override;
};

class DataType
{
public:
	enum Type
	{
		t_INT,
		t_STRING,
		t_BOOL,
		t_CHAR,
		t_TYPENAME
	};
	Type type;
	Class* classType = NULL;
	bool isArray;

	bool operator== (const DataType& other) const;
};

class FieldTable
{
private:
	string* name;
	DataType* type;
	bool isStatic;
	AccessModifier accessModifier;

public:
	string* GetName();
	DataType* GetType();
};

class Variable
{
public:
	DataType* type;
	string* name;
};

class MethodTable
{
private:
	string* name;
	bool isStatic;
	bool isAbstract;
	bool isVirtual;
	AccessModifier accessModifier;

	DataType* returnValue;
	vector<Variable*> params;
	vector<Variable*> localVariables;
	//StatementList* body;

public:
	DataType* GetReturnValue();
	bool CompareParamsSet(map<DataType, int>* args);
	int GetParamIndex(string* name);
};

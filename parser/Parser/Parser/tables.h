#pragma once

#include <iostream>
#include <vector>
#include <map>
using namespace std;

class FieldTable;
class MethodTable;
class ClassDeclaration;
class Field;
class SimpleType;
class DataType;
class ClassMember;
class TypeName;

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

string GetAccessModifierName(AccessModifier modifier);

class AbstractNamespaceMember 
{
public:
	virtual int GetId() = 0;
	virtual string* GetName() = 0;
	virtual string GetFullName() = 0;
	virtual AbstractNamespaceMember* GetOuterMember() = 0;
	virtual AbstractNamespaceMember* GetInnerMember(string* name) = 0;
	virtual vector<AbstractNamespaceMember*> GetAllMembers() = 0;
	virtual void Append(AbstractNamespaceMember* member) = 0;
	virtual string* ToDOT() = 0;
};

class Namespace : public AbstractNamespaceMember
{
private:
	int id;
	string* name;
	vector<AbstractNamespaceMember*> members;
	AbstractNamespaceMember* outerMember;

public:
	Namespace(string* name, AbstractNamespaceMember* outer);
	int GetId() override;
	string* GetName() override;
	string GetFullName() override;
	AbstractNamespaceMember* GetOuterMember() override;
	AbstractNamespaceMember* GetInnerMember(string* name) override;
	vector<AbstractNamespaceMember*> GetAllMembers() override;
	void Append(AbstractNamespaceMember* member) override;
	string* ToDOT() override;
};

class Class : public AbstractNamespaceMember
{
private:
	int id;
	vector<Constant*> constantTable;
	bool isStatic = false;
	bool isAbstract = false;
	AccessModifier accessModifier = e_NONE;

	string* name;
	Class* parent = NULL;
	ClassDeclaration* decl = NULL;

	map<string, FieldTable*> fields;
	map<string, MethodTable*> methods;

	AbstractNamespaceMember* outerMember = NULL;
	vector<Class*> innerMembers;

	DataType* CreateDataType(ClassMember* member);
	Class* FindClass(TypeName* typeName);
	void AppendField(Field* field);
	void AppendParent(TypeName* parentName);

public:
	Class(string* name, AbstractNamespaceMember* outer, ClassDeclaration* decl);
	int GetId() override;
	string* GetName() override;
	string GetFullName() override;
	Class* GetParent();

	MethodTable* GetMethod(string* name);
	FieldTable* GetField(string* name);
	vector<FieldTable*> GetAllFields();

	void SetStatic(bool value);
	bool IsStatic();
	void SetAbstract(bool value);
	bool IsAbstract();
	void SetAccesModifier(AccessModifier modifier);
	AccessModifier GetAccessModifier();

	void CreateTables();

	AbstractNamespaceMember* GetOuterMember() override;
	AbstractNamespaceMember* GetInnerMember(string* name) override;
	vector<AbstractNamespaceMember*> GetAllMembers() override;
	void Append(AbstractNamespaceMember* member) override;
	string* ToDOT() override;
	string ToString();
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
	Type type = t_INT;
	Class* classType = NULL;
	bool isArray = false;

	static Type GetType(SimpleType* simpleType);
	string* ToString();
	bool operator== (const DataType& other) const;
};

class FieldTable
{
private:
	string* name;
	DataType* type;
	bool isStatic = false;
	AccessModifier accessModifier = e_NONE;

public:
	FieldTable(string* name, DataType* type);
	void SetStatic(bool value);
	bool IsStatic();
	void SetAccessModifier(AccessModifier modifier);
	AccessModifier GetAccessModifier();

	string* GetName();
	DataType* GetType();
	string ToString();
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

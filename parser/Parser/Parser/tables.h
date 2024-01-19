#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <bitset>
using namespace std;

class FieldTable;
class MethodTable;
class ClassDeclaration;
class Field;
class Method;
class Constructor;
class SimpleType;
class DataType;
class ClassMember;
class TypeName;
class VarDeclarator;
class Variable;
class Expression;
class StatementList;
class Statement;
class MemberAccess;
class InvocationExpression;
class ArgumentList;

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
	Constant(Type type, string* utf8);
	Constant(Type type, int integer);
	Constant(Type type, int firstRef, int secondRef);

	bool operator== (const Constant& other) const;
	string ToCSV();
	int ToByteCode(vector<char>* byteCode);

	static char* IntToByteCode(int integer);
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
	vector<Constant*> constantTable = vector<Constant*>{new Constant(Constant::t_INTEGER, 0)};
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

	vector<char> byteCode = vector<char>();

	void AppendField(Field* field);
	void AppendMethod(Method* method);
	void AppendMethod(string* name, DataType* returnType, vector<Variable*> params);
	void AppendConstructor(Constructor* constructor);
	void AppdendDefaultConstructor();
	void AppendParent(TypeName* parentName);
	void AppendFieldInitializers(MethodTable* constructor, ArgumentList* args);

	static Class* CreateObjectClass(AbstractNamespaceMember* outer);
	static Class* CreateStringClass(AbstractNamespaceMember* outer);
	static Class* CreateIntClass(AbstractNamespaceMember* outer);
	static Class* CreateCharClass(AbstractNamespaceMember* outer);
	static Class* CreateBoolClass(AbstractNamespaceMember* outer);
	static Class* CreateConsoleClass(AbstractNamespaceMember* outer);

	static void FillObjectClass(AbstractNamespaceMember* outer);
	static void FillStringClass(AbstractNamespaceMember* outer);
	static void FillIntClass(AbstractNamespaceMember* outer);
	static void FillCharClass(AbstractNamespaceMember* outer);
	static void FillBoolClass(AbstractNamespaceMember* outer);
	static void FillConsoleClass(AbstractNamespaceMember* outer);

	int IndexOfConstant(Constant* constant);

	void AppendConstatntToByteCode();
	void AppendClassInformationToByteCode();
	void AppendFieldsTableToByteCode();
	void AppendMethodsTableToByteCode();

public:
	Class(string* name, AbstractNamespaceMember* outer, ClassDeclaration* decl);
	int GetId() override;
	string* GetName() override;
	string GetFullName() override;
	Class* GetParent();

	MethodTable* GetMethod(string name);
	FieldTable* GetField(string name);
	vector<FieldTable*> GetAllFields();
	vector<MethodTable*> GetAllMethods();

	void SetStatic(bool value);
	bool IsStatic();
	void SetAbstract(bool value);
	bool IsAbstract();
	void SetAccesModifier(AccessModifier modifier);
	AccessModifier GetAccessModifier();

	void CreateTables();
	void CheckOverridingMethods();
	void CheckOverridingFields();

	DataType* CreateDataType(ClassMember* member);
	DataType* CreateDataType(VarDeclarator* varDecl);

	AbstractNamespaceMember* GetOuterMember() override;
	AbstractNamespaceMember* GetInnerMember(string* name) override;
	vector<AbstractNamespaceMember*> GetAllMembers() override;
	void Append(AbstractNamespaceMember* member) override;
	
	bool InstanceOf(Class* other);

	string* ToDOT() override;
	string ToString();
	void WriteTablesFile();

	Class* FindClass(TypeName* typeName);

	static void CreateRTLClasses(AbstractNamespaceMember* outer);

	int AppendUtf8Constant(string* utf8);
	int AppendIntegerConstant(int integer);
	int AppendStringConstant(string* utf8);
	int AppendClassConstant(Class* classInfo);
	int AppendNameAndTypeConstant(string* name, string* descriptor);
	int AppendFieldRefConstant(Class* owner, FieldTable* fieldInfo);
	int AppendMethofRefConstant(Class* owner, MethodTable* methodTable);

	void WriteClassFile();
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
		t_TYPENAME,
		t_VOID
	};
	Type type = t_INT;
	Class* classType = NULL;
	bool isArray = false;

	DataType(Type type, Class* classType, bool isArray, Class* namespaceMember);
	static Type GetType(SimpleType* simpleType);
	string* ToString();
	string ToDescriptor();
	bool operator== (const DataType& other) const;

	static Class* GetClassOfType(Type type, Class* namespaceMember);
};

class FieldTable
{
private:
	string* name;
	DataType* type;
	bool isStatic = false;
	AccessModifier accessModifier = e_NONE;
	Expression* initializer = NULL;

	Expression* GetDefaultInitializer();

public:
	FieldTable(string* name, DataType* type);
	void SetStatic(bool value);
	bool IsStatic();
	void SetAccessModifier(AccessModifier modifier);
	AccessModifier GetAccessModifier();

	void SetInitializer(Expression* expr);
	Expression* GetInitializer();

	string* GetName();
	DataType* GetType();
	string ToString();

	void ToByteCode(Class* owner, vector<char>* byteCode);
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
	bool isStatic = false;
	bool isAbstract = false;
	bool isVirtual = false;
	bool isOverride = false;
	AccessModifier accessModifier = e_NONE;

	DataType* returnValue;
	vector<Variable*> params;
	vector<Variable*> localVariables;
	StatementList* body = NULL;

public:
	MethodTable(string* name, DataType* dataType);
	void SetStatic(bool value);
	bool IsStatic();
	void SetAbstract(bool value);
	bool IsAbstract();
	void SetVirtual(bool value);
	bool IsVirtual();
	void SetOverride(bool value);
	bool IsOverride();
	void SetAccessModifier(AccessModifier modifier);
	AccessModifier GetAccessModifier();
	void AddParam(string* name, DataType* type);
	Variable* GetParam(string* name);

	void SetBody(StatementList* body);
	StatementList* GetBody();

	DataType* GetReturnValue();
	string* GetName();
	vector<Variable*> GetParams();
	void AddLocalVariable(string* name, DataType* type);
	Variable* GetLocalVariable(string* varName);

	void Semantic(Class* owner);

	bool CompareArgsTypes(ArgumentList* args);
	int GetParamIndex(string* name);
	int GetLocalsCount();
	int GetLocalIndex(string* name);

	string ToString();
	string* GetDescriptor();

	void ToByteCode(Class* owner, vector<char>* byteCode);
};

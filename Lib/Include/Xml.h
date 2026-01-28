#pragma once

#include <stdio.h>
#include <iostream>
#include "Str.h"
#define UINT32 unsigned int
using namespace Dynamic;

class XmlObj;
class XmlObjOper;
class XmlAttrOper;

class XmlVal{
	//Creative Code
	DynamicStr StrVal = DynamicStr(128);	//OnlyValue
	XmlObj* ChildVal;						//
	bool isObj;								//Obj(t), Value(f)
public:
	XmlVal();
	~XmlVal();

	//user Code

};

class XmlAttrObj{
	//Creative Code
	DynamicStr Name = DynamicStr(128);		//속성자신의 이름
	DynamicStr Value = DynamicStr(128);		//속성의 값
	XmlAttrObj* next;						//다음 속성
public:
	XmlAttrObj();
	~XmlAttrObj();

	//User Code
};

class XmlObj{
	friend class XmlObjOper;
	friend class XmlAttrOper;
	//Creative Code
	DynamicStr Name = DynamicStr(128);
	XmlAttrObj Attr;					//Linked List
	XmlVal Val;							//Xml Obj or Value
	XmlObj* next;						//NextXml
	UINT32 ObjIdx;						//Obj Count Index
public:
	XmlObj();
	~XmlObj();
	
	//Assignment Operator Overloading
	XmlObjOper operator()(int = -1);	//Obj
	XmlAttrOper operator[](char*);		//AttrObj

	//Conversion Operator Overloading
	//operator char*();			//문자열로 반환
	//operator DynamicStr*();		
	//operator XmlObj*();			//객체로 반환	

	//User Code


};

class XmlObjOper{
	XmlObj* ObjRoot;
public:
	XmlObjOper();
	~XmlObjOper();
	//Assignment Operator Overloading
	void operator<<(char*);
	void operator<<(XmlObj*);	//Obj Shallow Copy(Just Address Copy But My Obj Clear)
	void operator<<(XmlObj);	//Obj DeepCopy

	//Conversion Operator Overloading
	explicit operator char*();
	explicit operator DynamicStr*();
	explicit operator XmlObj*();
	explicit operator XmlObj();

	//UserCode
	void SetObjRoot(XmlObj*);
};

class XmlAttrOper{
	XmlAttrObj* AttrObjRoot;
public:
	XmlAttrOper();
	~XmlAttrOper();
	//Assignment Operator Overloading
	void operator=(char*);
	void operator=(DynamicStr*);

	//Conversion Operator Overloading
	operator char*();
	operator DynamicStr*();

};






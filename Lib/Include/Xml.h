#pragma once

#include <stdio.h>
#include <iostream>
#include "Str.h"
//#define UINT32 unsigned int
using namespace Dynamic;
void XmlTestFunc();
class XmlObj;
class XmlObjOper;
class XmlAttrOper;
class XmlVal;
class XmlText;
class XmlElementRef;

typedef union{
	DynamicStr* StrVal;
	XmlObj* ChildVal;
}pXmlVal;

template<typename T>
class Node{
	T* Data;
	Node* next;
	Node* prev;
public:
	Node() : Data(nullptr), next(nullptr), prev(nullptr){}
	~Node(){
		if (Data != nullptr) delete Data;
		if (next != nullptr) delete next;
	}
	template <typename U>friend class LinkedList;

	T* getData(){ return Data; }
	Node* getNextNode(){ return next; }
	Node* getPrevNode(){ return prev; }
};

template <typename T>
class LinkedList{
	Node<T>* Head;
	Node<T>* Tail;
public:
	int Index;
	LinkedList();		
	LinkedList(T* Root);
	~LinkedList();
	//user code
	void AddList(T* AddNode);			//마지막에만 추가
	void DelList();						//마지막에만 제거
	void AddList(T* AddNode, int idx);
	void DelList(int idx);
	Node<T>* getHead();
	Node<T>* getTail();
};

class XmlVal{
	friend class XmlObj;
	//Creative Code
	//Dev Code
public:		
	XmlVal();
	XmlVal();
	virtual ~XmlVal();

	//user Code
	virtual char* getName() = 0;	//이름 반환할것(Value면 그냥 값, Obj면 Obj이름)
	virtual void SetName(DynamicStr*, DynamicStr*);
	virtual void SetName(DynamicStr*, char*);
};

class XmlText : public XmlVal{
	DynamicStr Data = (32);	
public:
	XmlText(char* );
	XmlText(DynamicStr* );

	void UpdateValue(char* );
	void UpdateValue(DynamicStr* );
	char* getName();
};

class XmlElementRef : public XmlVal{
	XmlObj* Obj;
public:
	XmlElementRef(char*);
	XmlElementRef(DynamicStr*);
	~XmlElementRef();
	void UpdateObjName(char*);
	void UpdateObjName(DynamicStr*);
	char* getName();
	XmlObj* getObj();
	void setObj();		//Object를 새롭게 재구성하는것
};


class XmlAttrObj{
	//Creative Code
	DynamicStr Name = DynamicStr(128);		//속성자신의 이름
	DynamicStr Value = DynamicStr(128);		//속성의 값	
public:
	XmlAttrObj();
	~XmlAttrObj();

	//User Code
	
};

class XmlObj{
	friend class XmlElementRef;
	friend class XmlObjOper;
	friend class XmlAttrOper;
	//Creative Code
	DynamicStr Name = DynamicStr(128);
	LinkedList<XmlAttrObj> Attrs;
	LinkedList<XmlVal> Vals;
public:
	XmlObj();
	XmlObj(DynamicStr* Name);
	XmlObj(char* Name);
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






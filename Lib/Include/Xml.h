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
	//Creative Code
	pXmlVal* Val;
	bool isObj;								//Obj(t), Value(f)
public:
	XmlVal();
	~XmlVal();

	//user Code
	//추가해주는거 삭제해주는거 필요함
	void SetAddObj();
	
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
	friend class XmlObjOper;
	friend class XmlAttrOper;
	//Creative Code
	DynamicStr Name = DynamicStr(128);
	LinkedList<XmlAttrObj> Attrs;
	LinkedList<XmlVal> Vals;
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






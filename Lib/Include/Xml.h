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

/* =================================================================================
* Linked List Lib
===================================================================================*/

template<typename T>
class Node{
	T* Data;
	Node* next;
	Node* prev;
public:
	Node() : Data(nullptr), next(nullptr), prev(nullptr){}
	~Node() = default;
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
	void setNode(T* SetNode);
	void AllDelList();					//전체를 비워야하는 경우(객체에서 값으로 바뀐다거나 하는 경우)
	void AddList(T* AddNode);			//마지막에만 추가
	void DelList();						//마지막에만 제거
	void AddList(T* AddNode, int idx);
	void DelList(int idx);
	Node<T>* getHead();
	Node<T>* getTail();
	Node<T>* get(int);
};







/* =================================================================================
* Xml Structer
===================================================================================*/
/* =============================
* 내부 기능 클래스들
================================*/
class XmlStr{
public:
	DynamicStr Name;		//크기는 완전 고정으로 하면 안될듯
	XmlStr(char* newName, int StrSize = 32) : Name(StrSize){
		Name.Append_Str(newName);
	}
	XmlStr(DynamicStr* newName, int StrSize = 32) : Name(StrSize){
		Name.Set_Str(newName->Get_Str());
	}

	//완전히 책임을 넘길지 말지는 애매한 부분(참조 혹은 복사 TBD)
	char* getName(){
		return Name.Get_Str();
	}

	//Update 해줄것
	void SetName(char* setName){
		Name.Str_Clear();
		Name.Append_Str(setName);
	}
	void SetName(DynamicStr* setName){
		Name.Str_Clear();
		Name.Set_Str(setName->Get_Str());
	}
};

class XmlVal{
	friend class XmlObj;
	friend class XmlStr;
	virtual char* c_toString() = 0;
	virtual DynamicStr* d_toString() = 0;
	//Creative Code
	//Dev Code
protected:
	XmlVal() = default;
public:
	virtual ~XmlVal() = default;

	//user Code
};

//<Obj>값</Obj> 객체안에 값을 의미
class XmlValue : public XmlVal{
protected:
public:
	virtual char* c_toString() override;
	virtual DynamicStr* d_toString() override;
	XmlStr Val;
	XmlValue(char*);
	XmlValue(DynamicStr*);
	~XmlValue() = default;

};


/* =============================
* 외부 클래스
================================*/
//Xml객체들을 관리해주는 주체(소멸에 대한 책임 전부 짊어짐)
class XmlAttrObj{
	//Creative Code
	DynamicStr Name = DynamicStr(128);		//속성자신의 이름
	DynamicStr Value = DynamicStr(128);		//속성의 값	
public:
	XmlAttrObj();
	~XmlAttrObj();

	//User Code
	
};


class XmlObj : public XmlVal{
	friend class XmlElementRef;
	friend class XmlObjOper;
	friend class XmlAttrOper;
	//Creative Code
	//DynamicStr Name = DynamicStr(128);
	XmlStr Name;
	LinkedList<XmlAttrObj> Attrs;
	LinkedList<XmlVal> Vals;		//객체이거나 값이거나

	//Develop Code
	void DelAttrs();
	void DelVals();

	bool isVal();
	void InitValSet();

public:
	virtual char* c_toString() override;
	virtual DynamicStr* d_toString() override;
	//XmlObj();
	XmlObj(DynamicStr* Name);
	XmlObj(char* Name);
	~XmlObj() = default;
	

	//User Code
	void setVal(DynamicStr*);
	void setVal(char*);
	void addObj(DynamicStr*);
	void addObj(char*);


	XmlValue* getVal();		
	XmlObj* getObj(int idx = -1);	//매개변수 값을 안넣을시 가장 마지막



	//Assignment Operator Overloading
	XmlObjOper operator()(int = -1);	//Obj or Value
	XmlAttrOper operator[](char*);		//AttrObj
};











/* =============================
* 연산자 오버로딩 클래스들
================================*/

//Xml객체들을 생성해주는 주체들
class XmlObjOper{
	XmlObj* ObjRoot;
public:
	XmlObjOper();
	~XmlObjOper();
	//Assignment Operator Overloading
	void operator<<(char*);
	void operator<<(DynamicStr*);
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






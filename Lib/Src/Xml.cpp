#include <stdlib.h>
//#include "Str.h"
#include "../Include/Xml.h"

using namespace std;
using namespace Dynamic;
/* ================================================
* Linked List Lib
==================================================*/
template<typename T>
LinkedList<T>::LinkedList():Head(nullptr), Tail(nullptr), Index(-1){}
template<typename T>
LinkedList<T>::LinkedList(T* Root) : Index(0), Head(nullptr), Tail(nullptr){
	//Node가 하나 만들어지고 시작하는 거임
	Head = new Node<T>;
	Head->Data = Root;
	Head->next = nullptr;
	Head->prev = nullptr;
	Tail = Head;
}
template<typename T>
LinkedList<T>::~LinkedList(){
	AllDelList();
}

template<typename T>
void LinkedList<T>::AllDelList(){
	Node<T>* cur = Head;
	while (cur != nullptr){
		Node<T>* next = cur->next;
		cur->next = nullptr;
		delete cur;
		cur = next;
	}
	Head = Tail = nullptr;
	Index = -1;
}

template<typename T>
void LinkedList<T>::setNode(T* SetNode){
	if (SetNode == nullptr) return;

	AllDelList();

	AddList(SetNode);
}

template<typename T>
void LinkedList<T>::AddList(T* AddNode){
	if (AddNode == nullptr) return;

	Node<T>* newNode = new Node<T>();
	newNode->Data = AddNode;
	newNode->next = nullptr;
	if (Tail != nullptr){
		newNode->prev = Tail;
		Tail->next = newNode;
		Tail = newNode;
	}
	else{
		newNode->prev = nullptr;
		Head = Tail = newNode;
	}
	Index++;
}
template<typename T>
void LinkedList<T>::AddList(T* AddNode, int idx){ //사용자편의 1부터 시작 0없음
	if (AddNode == nullptr || idx <= 0 || idx > Index+1) return;

	//맨뒤에 추가하는 경우 재활용하기
	if (idx == Index+1 || Index == 0){
		AddList(AddNode);
		return;
	}
	Node<T>* newNode = new Node<T>();
	newNode->Data = AddNode;

	//맨 앞에 삽입하는 경우
	if (idx == 1){
		newNode->next = Head;
		newNode->prev = nullptr;
		if (Head) Head->prev = newNode;
		Head = newNode;
	}
	//중간에 삽입하는 경우
	else{
		Node<T>* target = Head;
		for (int i = 1; i < idx; i++){
			target = target->next;
		}
		newNode->prev = target->prev;
		newNode->next = target;
		if (target->prev) target->prev->next = newNode;
		target->prev = newNode;
	}
	Index++;
}

template<typename T>
void LinkedList<T>::DelList(){
	if (Tail == nullptr) return;//리스트가 비워져있음
	Node<T>* target = Tail;
	if (Tail->prev != nullptr){
		if (Tail->prev) Tail->prev->next = nullptr;
		Tail = Tail->prev;
		target->prev = nullptr;
		delete target;
		target = nullptr;
	}
	else{	//노드가 1개임
		delete Tail;
		Head = Tail = nullptr;
	}
	Index--;
}

template<typename T>
void LinkedList<T>::DelList(int idx){
	if (Tail == nullptr || idx <= 0 || idx > Index+1) return;
	
	//맨뒤에 제거하는 경우
	if (idx == Index+1 || Index == 0){
		DelList();
		return;
	}
	Node<T>* target = Head;

	//맨 앞에 삭제하는 경우
	if (idx == 1){
		Head = Head->next;
		Head->prev = nullptr;
		target->next = nullptr;
		delete target;
		target = nullptr;
	}
	//중간에 삭제하는 경우
	else{
		for (int i = 1; i < idx; i++){
			target = target->next;
		}
		if (target->prev) target->prev->next = target->next;
		if (target->next) target->next->prev = target->prev;
		delete target;
		target = nullptr;
	}
	Index--;
}
template<typename T>
Node<T>* LinkedList<T>::getHead(){
	return Head;
}
template<typename T>
Node<T>* LinkedList<T>::getTail(){
	return Tail;
}
template<typename T>
Node<T>* LinkedList<T>::get(int idx){
	if (idx > Index || idx < 0){
		return nullptr;
	}
	if (Index == idx){
		return getTail();
	}

	Node<T>* target = Head;
	for (int i = 0; i < idx; i++){
		target = target->next;
	}
	return target;
}

void DelTest(){
	XmlObj root("Root");

	root.addObj("Child");

	XmlObj* Child = root.getObj();

	Child->setVal("TestValue");

	printf("%s\n", Child->c_toString());
}

void XmlTestFunc(){
	DelTest();
	

	
	int debug = 10;	
}

/* ================================================
* Xml Value(XmlObj or Value)
==================================================*/



// XmlText
XmlValue::XmlValue(char* newValue) : Val(newValue){}
XmlValue::XmlValue(DynamicStr* newValue) : Val(newValue){}
char* XmlValue::c_toString() {
	return Val.Name.Get_Str();
}
DynamicStr* XmlValue::d_toString()  {
	return &Val.Name;
}


/* ================================================
* Xml Attribute Class
==================================================*/
XmlAttrObj::XmlAttrObj(){}
XmlAttrObj::~XmlAttrObj(){}




/* ================================================
* Xml Class
==================================================*/
//XmlObj::XmlObj(){}
XmlObj::XmlObj(DynamicStr *Name) : Name(Name, 128){
	InitValSet();
}
XmlObj::XmlObj(char *Name) : Name(Name, 128){
	InitValSet();
}
char* XmlObj::c_toString() {
	DynamicStr* toStr = new DynamicStr(128);
	toStr->Append_Char("<");
	toStr->Append_Str(Name.getName());	
	toStr->Append_Char(">");

	Node<XmlVal>* Cur = Vals.getHead();
	
	while (Cur != nullptr){
		toStr->Append_Str(Cur->getData()->c_toString());
		Cur = Cur->getNextNode();
	}
	toStr->Append_Str("</");
	toStr->Append_Str(Name.getName());
	toStr->Append_Char(">");

	char* ptoStr = toStr->Get_Str();

	toStr->str_free();

	delete toStr;

	return ptoStr;
}
DynamicStr* XmlObj::d_toString()  {
	DynamicStr* toStr = new DynamicStr(128);
	toStr->Append_Char("<");
	toStr->Append_Str(Name.getName());
	toStr->Append_Char(">");

	Node<XmlVal>* Cur = Vals.getHead();

	while (Cur != nullptr){
		toStr->Append_Str(Cur->getData()->c_toString());
		Cur = Cur->getNextNode();
	}
	toStr->Append_Str("</");
	toStr->Append_Str(Name.getName());
	toStr->Append_Char(">");

	return toStr;
}

void XmlObj::InitValSet(){
	XmlVal* InitVal = new XmlValue("");
	Vals.AddList(InitVal);
}
bool XmlObj::isVal(){
	XmlVal* v = Vals.getHead()->getData();

	if (dynamic_cast<XmlValue*>(v)){
		return true;
	}
	return false;
}

void XmlObj::DelAttrs(){Attrs.AllDelList();}
void XmlObj::DelVals(){}

void XmlObj::setVal(DynamicStr* newVal){
	XmlVal* pnewVal = new XmlValue(newVal);
	Vals.setNode(pnewVal);
}
void XmlObj::setVal(char* newVal){
	XmlVal* pnewVal = new XmlValue(newVal);
	Vals.setNode(pnewVal);
}
void XmlObj::addObj(DynamicStr* newObj){
	XmlVal* pnewObj = new XmlObj(newObj);
	if (isVal()){
		Vals.setNode(pnewObj);
	}
	else{
		Vals.AddList(pnewObj);
	}
}
void XmlObj::addObj(char* newObj){
	XmlVal* pnewObj = new XmlObj(newObj);
	if (isVal()){
		Vals.setNode(pnewObj);
	}
	else{
		Vals.AddList(pnewObj);
	}
}

XmlValue* XmlObj::getVal(){
	if (isVal() == false){
		//Obj가 들어있는데 Val을 꺼내려고 해서 실패하는 경우
		return nullptr;
	}

	return dynamic_cast<XmlValue*>(Vals.getHead()->getData());
}
XmlObj* XmlObj::getObj(int idx){
	if (isVal()){
		//Value면 Obj가 아니기 때문에 실패
		return nullptr;
	}
	Node<XmlVal>* pNode = nullptr;
	if (idx == -1)
		pNode = Vals.getTail();
	else
		pNode = Vals.get(idx);

	if (pNode == nullptr){
		return nullptr;
	}	
	XmlVal* data = pNode->getData();
	return dynamic_cast<XmlObj*>(data);
}

//객체에 객체를 넣는 경우(순서 중요)
XmlObjOper XmlObj::operator()(int idx){
	XmlObjOper test;
	test.SetObjRoot(this);
	return test;
}
//객체의 속성을 부여하는 경우
XmlAttrOper XmlObj::operator[](char* AttrName){
	XmlAttrOper test;
	return test;
}



/* ================================================
* Xml Obj Operator
==================================================*/
XmlObjOper::XmlObjOper(){}
XmlObjOper::~XmlObjOper(){}
void XmlObjOper::operator<<(char*){
	
}
void XmlObjOper::operator<<(XmlObj*){

}
void XmlObjOper::operator<<(XmlObj){

}
XmlObjOper::operator char*(){
	return "T";
}
XmlObjOper::operator DynamicStr*(){
	return nullptr;
}
XmlObjOper::operator XmlObj*(){
	return nullptr;
}
XmlObjOper::operator XmlObj(){
	XmlObj T("T");
	return T;
}
//UserCode
void XmlObjOper::SetObjRoot(XmlObj* xmlobj){

}

/* ================================================
* Xml Attr Operator
==================================================*/
XmlAttrOper::XmlAttrOper(){}
XmlAttrOper::~XmlAttrOper(){}
void XmlAttrOper::operator=(char* rStrVal){

}
void XmlAttrOper::operator=(DynamicStr* rStrVal){

}
XmlAttrOper::operator char*(){
	return "T";
}
XmlAttrOper::operator DynamicStr*(){
	return nullptr;
}





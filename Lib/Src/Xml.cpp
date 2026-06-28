#define _CRT_SECURE_NO_WARNINGS
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
		delete cur->Data;
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


void XmlAttrPrint(XmlObj* Obj){
	int AttrsIdx = Obj->getAttrsIdx();
	AttrsIdx != -1 ? printf(" ") : 0;
	for (int i = 0; i <= AttrsIdx; i++){
		if (i == AttrsIdx) printf("%s = \"%s\"", Obj->getAttr(i)->getName(), Obj->getAttr(i)->getValue());
		else printf("%s = \"%s\" ", Obj->getAttr(i)->getName(), Obj->getAttr(i)->getValue());
	}
}

void XmlValuePrint(XmlObj* Obj, int floor){
	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("<%s", Obj->getName());
	XmlAttrPrint(Obj);
	printf(">\n");
	for (int i = 0; i <= floor; i++){
		printf("\t");
	}
	printf("%s\n", Obj->getVal()->Val.getName());
	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("</%s>\n", Obj->getName());
}

void XmlObjPrint(XmlObj* Obj, int floor){
	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("<%s", Obj->getName());
	XmlAttrPrint(Obj);
	printf(">\n");

	XmlObj* pObj = nullptr;


	for (int i = 0; i <= Obj->getObjIdx(); i++){
		pObj = Obj->getObj(i);

		if (pObj->isVal()){
			XmlValuePrint(pObj, floor+1);
		}
		else{
			XmlObjPrint(pObj, floor+1);
		}
	}


	for (int i = 0; i < floor; i++){
		printf("\t");
	}
	printf("</%s>\n", Obj->getName());
}

void XmlPrint(XmlObj* Root){
	XmlVal* cursor = Root;
	XmlObj* Obj = dynamic_cast<XmlObj*>(cursor);	
	XmlObj* pObj = nullptr;
	
	//Object는 무조건 여러개인데?
	if (Obj->isVal()){
		XmlValuePrint(Obj, 0);
	}
	else{
		XmlObjPrint(Obj, 0);
	}
}



void DelTest(){
	XmlObj root("Root");

	root.setObj("Child1");
	root.addObj("Child2");
	root.addObj("Child3");
	XmlObj* ctrlObj = root.getObj(0);
	char str[32];
	for (int i = 0; ; i++){
		ctrlObj = root.getObj(i);
		if (ctrlObj == nullptr)
			break;
		sprintf(str, "I'm Child%d", i);
		ctrlObj->setVal(str);
	}
		
	root.setAttr("Test", "Test");
	root.setAttr("Attr", "Value");
	root.addAttr("Attr2", "Val");


	XmlAttrObj* attrObj = root.getAttr(0);
	for (int i = 0; i; i++){
		attrObj = root.getAttr(i);
		if (attrObj == nullptr)
			break;
		printf("속성 : %s=\"%s\"\n", attrObj->getName(), attrObj->getValue());
	}

	printf("Value : %s\n", root.c_toString());

	//객체를 처음 만들고 나면 무조건 ""값이 있는데 Value상태로 "" 이런 상태여야함
	XmlPrint(&root);
}

void OperTest(){
	XmlObj root("Root");

	root.setObj("Child1");
	root.setAttr("Attr1", "Value");
	
	root.getObj(0);
	root.getObj("Child1");
	root.getAttr("Attr1");

	//오퍼레이터 반환
	root(0);		//객체용(객체인덱스로 찾기)
	root("Child1");	//객체용(객체명으로 찾기)
	root[0];		//속성용(속성인덱스로 찾기)
	root["Attr"];	//속성용(속성명으로 찾기)

	//<대입>

	//<반환>

	root.c_toString();

}

void XmlTestFunc(){
	DelTest();
	//OperTest();

	
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
XmlAttrObj::XmlAttrObj(DynamicStr* Name, DynamicStr* Value){
	this->Name.Set_Str(Name->Get_Str());
	this->Value.Set_Str(Value->Get_Str());
}
XmlAttrObj::XmlAttrObj(char* Name, char* Value){
	this->Name.Set_Str(Name);
	this->Value.Set_Str(Value);
}
XmlAttrObj::~XmlAttrObj(){}
char* XmlAttrObj::getName(){
	return Name.Get_Str();
}
char* XmlAttrObj::getValue(){
	return Value.Get_Str();
}



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
	auto* toStr = new DynamicStr(128);
	//toStr->Append_Char("<");
	//toStr->Append_Str(Name.getName());	
	//toStr->Append_Char(">");

	Node<XmlVal>* Cur = Vals.getHead();
	
	while (Cur != nullptr){
		toStr->Append_Str(Cur->getData()->c_toString());
		Cur = Cur->getNextNode();
	}
	//toStr->Append_Str("</");
	//toStr->Append_Str(Name.getName());
	//toStr->Append_Char(">");

	char* ptoStr = toStr->Get_Str();

	toStr->str_free();

	delete toStr;

	return ptoStr;
}
DynamicStr* XmlObj::d_toString()  {	//호출자가 delete 해줘야함(이거는 안쓰는게 좋은듯)
	auto* toStr = new DynamicStr(128);
	//toStr->Append_Char("<");
	//toStr->Append_Str(Name.getName());
	//toStr->Append_Char(">");

	Node<XmlVal>* Cur = Vals.getHead();

	while (Cur != nullptr){
		toStr->Append_Str(Cur->getData()->c_toString());
		Cur = Cur->getNextNode();
	}
	//toStr->Append_Str("</");
	//toStr->Append_Str(Name.getName());
	//toStr->Append_Char(">");

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

char* XmlObj::getName(){
	return Name.getName();
}
int XmlObj::getObjIdx(){
	if (isVal()) return -1;
	return Vals.Index;
}
int XmlObj::getAttrsIdx(){
	return Attrs.Index;
}

void XmlObj::setVal(DynamicStr* newVal){
	XmlVal* pnewVal = new XmlValue(newVal);
	Vals.setNode(pnewVal);
}
void XmlObj::setVal(char* newVal){
	XmlVal* pnewVal = new XmlValue(newVal);
	Vals.setNode(pnewVal);
}
void XmlObj::setObj(DynamicStr* newObj){
	XmlVal* pnewObj = new XmlObj(newObj);
	Vals.setNode(pnewObj);
}
void XmlObj::setObj(char* newObj){
	XmlVal* pnewObj = new XmlObj(newObj);
	Vals.setNode(pnewObj);
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

void XmlObj::setAttr(DynamicStr* AttrsName, DynamicStr* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.setNode(newAttr);
}
void XmlObj::setAttr(char* AttrsName, char* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.setNode(newAttr);
}
void XmlObj::addAttr(DynamicStr* AttrsName, DynamicStr* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.AddList(newAttr);
}
void XmlObj::addAttr(char* AttrsName, char* AttrsValue){
	auto* newAttr = new XmlAttrObj(AttrsName, AttrsValue);
	Attrs.AddList(newAttr);
}

XmlValue* XmlObj::getVal(){
	if (isVal() == false){
		//Obj가 들어있는데 Val을 꺼내려고 해서 실패하는 경우
		return nullptr;
	}

	auto *pVal = Vals.getHead()->getData();

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
XmlObj* XmlObj::getObj(char* name){
	if (isVal()){
		return nullptr;
	}
	Node<XmlVal>* pNode = nullptr;
	XmlObj* pObj = nullptr;
	pNode = Vals.getHead();
	for (int i = 0; i <= Vals.Index; i++){
		pObj = dynamic_cast<XmlObj*>(pNode->getData());
		if (DynamicStr::StrCmp_Org(pObj->getName(), name)){
			return pObj;
		}
		pNode = pNode->getNextNode();
	}
	return nullptr;	//Search Failed
}

XmlAttrObj* XmlObj::getAttr(int idx){
	if (Attrs.Index <= -1) return nullptr;	//속성이 없는 경우임
	Node<XmlAttrObj>* pNode = nullptr;

	if (idx == -1)
		pNode = Attrs.getTail();	//의미가 있나? 속성은?
	else
		pNode = Attrs.get(idx);

	if (pNode == nullptr)
		return nullptr;

	XmlAttrObj* data = pNode->getData();
	return data;
}
XmlAttrObj* XmlObj::getAttr(char* name){
	if (Attrs.Index <= -1) return nullptr;	//속성이 없는 경우임
	Node<XmlAttrObj>* pNode = nullptr;
	XmlAttrObj* pAttr = nullptr;
	pNode = Attrs.getHead();

	for (int i = 0; i <= Attrs.Index; i++){
		pAttr = dynamic_cast<XmlAttrObj*>(pNode->getData());
		if (DynamicStr::StrCmp_Org(pAttr->getName(), name)){
			return pAttr;
		}
		pNode = pNode->getNextNode();
	}
	return nullptr; //Search Failed

}


//객체 찾는 함수
template<typename T>
T XmlObj::SearchObjAttr(int idx, TYPE type){	//XmlObj* or XmlAttrObj* 만 반환
	if (type == TYPE::OBJ){
		T Obj = nullptr;
		void* raw = this->getObj(idx);
		Obj = static_cast<T>(raw);

		return Obj;
	}
	else{
		T Attr = nullptr;
		void* raw = this->getAttr(idx);
		Attr = static_cast<T>(raw);

		return Attr;
	}
	return nullptr;
}
template<typename T>
T XmlObj::SearchObjAttr(char* Name, TYPE type){	//XmlObj* or XmlAttrObj* 만 반환
	if (type == TYPE::OBJ){
		T Obj = nullptr;
		void* raw = this->getObj(Name);
		Obj = static_cast<T>(raw);

		return Obj;
	}
	else{
		T Attr = nullptr;
		void* raw = this->getAttr(Name);
		Attr = static_cast<T>(raw);

		return Attr;
	}
	return nullptr;
}

//객체에 객체를 넣는 경우(순서 중요)
XmlObjOper XmlObj::operator()(int idx){
	//객체를 찾기
	XmlObj* obj = this->SearchObjAttr<XmlObj*>(idx, TYPE::OBJ);

	XmlObjOper ObjOper(obj);
	return ObjOper;
}

XmlObjOper XmlObj::operator()(char* ObjName){
	XmlObj* obj = this->SearchObjAttr<XmlObj*>(ObjName, TYPE::OBJ);

	XmlObjOper ObjOper(obj);
	return ObjOper;
}

//객체의 속성을 부여하는 경우
XmlAttrOper XmlObj::operator[](int idx){
	XmlAttrObj* attr = this->SearchObjAttr<XmlAttrObj*>(idx, TYPE::ATTR);

	XmlAttrOper AttrOper(attr);
	return AttrOper;
}

XmlAttrOper XmlObj::operator[](char* AttrName){
	XmlAttrObj* attr = this->SearchObjAttr<XmlAttrObj*>(AttrName, TYPE::ATTR);
	
	XmlAttrOper AttrOper(attr);
	return AttrOper;
}



/* ================================================
* Xml Obj Operator
==================================================*/
XmlObjOper::XmlObjOper(XmlObj* ObjRoot) : ObjRoot(ObjRoot){

}
//XmlObjOper::~XmlObjOper(){}
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
XmlAttrOper::XmlAttrOper(XmlAttrObj* AttrRoot) :AttrRoot(AttrRoot){

}
//XmlAttrOper::~XmlAttrOper(){}
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





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
	if (Head != nullptr){
		delete Head;
	}
}

template<typename T>
void LinkedList<T>::AllDelList(){
	if (Head != nullptr){
		delete Head;
	}
	Index = -1;
	Head = nullptr;
	Tail = nullptr;
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

void LinkedList_Prt(LinkedList<XmlVal>* RootNode) {
	if (RootNode == nullptr || RootNode->Index == -1) {
		printf("[ Empty List ]\n");
		return;
	}

	printf("\n=== Double Linked List Structure (Total: %d) ===\n", RootNode->Index);

	// 1. Head 정보 출력
	printf(" HEAD [%p]\n", RootNode->getHead());
	printf("  ▼\n");

	Node<XmlVal>* CurObj = RootNode->getHead();

	// 2. 리스트 순회 및 노드 출력
	for (int i = 0; i <= RootNode->Index; i++) {
		if (CurObj == nullptr) break;

		// 노드 상태 출력 (이전 주소 <== 내 주소 ==> 다음 주소)
		printf(" [%02d]  (Prev: %p) <== [ Data: %p ] ==> (Next: %p)\n",
			i,
			CurObj->getPrevNode(),  // 이전 노드 주소
			CurObj->getData(),      // 실제 데이터 주소
			CurObj->getNextNode()); // 다음 노드 주소

		// 마지막 노드가 아니면 연결선 출력
		if (i < RootNode->Index - 1) {
			printf("              ↕\n");
		}

		CurObj = CurObj->getNextNode(); // 다음으로 이동
	}

	// 3. Tail 정보 출력
	printf("  ▲\n");
	printf(" TAIL [%p]\n", RootNode->getTail());
	printf("================================================\n\n");
}
void XmlTestFunc(){
	XmlObj Obj("Obj");

	XmlVal* objVal = new XmlElementRef("Test");
	XmlVal* Value = new XmlText("Test");
	//((XmlText*)Value)->UpdateValue("TT");
	int debug = 10;	
}

/* ================================================
* Xml Value(XmlObj or Value)
==================================================*/
XmlVal::XmlVal(){}

// XmlText
XmlText::XmlText(char* newName) : XmlName(newName){}
XmlText::XmlText(DynamicStr* newName) : XmlName(newName){}

// XmlElements
XmlElementRef::XmlElementRef(char* ObjName){Obj = new XmlObj(ObjName);}
XmlElementRef::XmlElementRef(DynamicStr* ObjName){Obj = new XmlObj(ObjName);}
void XmlElementRef::UpdateObjName(char* newObjName){
	if (Obj != nullptr){
		Obj->SetName(newObjName);
	}
}
void XmlElementRef::UpdateObjName(DynamicStr* newObjName){
	if (Obj != nullptr){
		Obj->SetName(newObjName);
	}
}
char* XmlElementRef::getObjName(){
	return Obj->getName();
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
XmlObj::XmlObj(DynamicStr *Name) : XmlName(Name, 128){}
XmlObj::XmlObj(char *Name) : XmlName(Name, 128){}

void XmlObj::DelAttrs(){Attrs.AllDelList();}
void XmlObj::DelVals(){Vals.AllDelList();}


//객체에 객체를 넣는 경우(순서 중요)
XmlObjOper XmlObj::operator()(int idx){
	XmlObjOper test;
	test.SetObjRoot(this);
	return test;
}
//객체의 이름을 부여하는 경우
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





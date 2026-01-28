#include <stdlib.h>
//#include "Str.h"
#include "../Include/Xml.h"

using namespace std;
using namespace Dynamic;

/* ================================================
* Xml Value(XmlObj or Value)
==================================================*/
XmlVal::XmlVal(){}
XmlVal::~XmlVal(){}



/* ================================================
* Xml Attribute Class
==================================================*/
XmlAttrObj::XmlAttrObj(){

}
XmlAttrObj::~XmlAttrObj(){}




/* ================================================
* Xml Class
==================================================*/
XmlObj::XmlObj():ObjIdx(0),next(nullptr){}
XmlObj::~XmlObj(){
	if (next != nullptr){
		delete next;
		next = nullptr;
	}
}
XmlObjOper XmlObj::operator()(int idx){
	XmlObjOper test;
	test.SetObjRoot(this);
	return test;
}
XmlAttrOper XmlObj::operator[](char* AttrName){
	XmlAttrOper test;
	return test;
}
/*
XmlObj::operator char*(){
	return "T";
}
XmlObj::operator DynamicStr*(){
	return nullptr;	//값인 경우 반환해주는거임
}
XmlObj::operator XmlObj*(){
	return nullptr;	//자신의 객체를 반환하고 자신은 빈깡통이 되어야함
}
*/

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
	XmlObj T;
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





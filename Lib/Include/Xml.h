#pragma once

#include <stdio.h>
#include <iostream>
#include "Str.h"

class XmlObj{
	//Creative Code
	DynamicStr Name = DynamicStr(128);
	XmlAttrObj Attr;
	//Xml 객체
public:
	XmlObj();
	~XmlObj();

	//User Code


};

class XmlVal{
	//Creative Code
	DynamicStr StrVal = DynamicStr(128);	//값만 있는경우
	XmlObj* ChildVal;						//
public:
	XmlVal();
	~XmlVal();

	//user Code

};


class XmlAttrObj{
	//Creative Code
	DynamicStr Name = DynamicStr(128);
	XmlAttrObj* next;
	DynamicStr Value = DynamicStr(128);
public:
	XmlAttrObj();
	~XmlAttrObj();

	//User Code
};
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include "../../Lib/Include/Xml.h"

/* ================================================
* Xml 라이브러리 테스트/데모 코드
* 라이브러리 본체(Xml.cpp)와 분리해서 관리한다.
==================================================*/

/* ================================================
* Xml API 검증용 테스트 모음
* 필요한 Test 함수만 XmlTestFunc()에서 주석 풀고 호출해보면 됨
==================================================*/

void XmlTest(){
	XmlObj root("Root");
	root.addObj("RtChild1");
	root.addObj("RtChild2");
	XmlObj* prtObj = root.getObj("RtChild2");
	prtObj->setAttr("Attr1", "Value");

	
	XmlObj rVal("rVal");
	rVal.addObj("rValChild1");

	XmlPrint(&root);

	//root(-1) = rVal;
	
	root.Insert(0) << rVal;	//체이닝을 사용하면 빈객체 <></> 여기 하위에 추가를 하기 때문에
							//<<연산자 오버로딩에서 지워야할듯 그래서 지우기 API가 필요함

	XmlPrint(&root);
}


void XmlTestFunc(){
	XmlTest();
}
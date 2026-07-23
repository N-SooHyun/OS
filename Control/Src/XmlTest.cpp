#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include "../../Lib/Include/Xml.h"
#include "../../Lib/Include/File.h"

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
	root("RtChild3") = "Value";
	root("RtChild4") = "Value";
	root("RtChild5") = "Value";
	root("RtChild6") << "Value";
	XmlPrint(&root);

	char* lVal = (char*)root("RtChild3");
	XmlObj* plVal = (XmlObj*)root("RtChild5");

	lVal = (char*)root("RtChild2")["Attr1"];

	printf("%s\n", lVal);

	root = "Test";

	XmlPrint(&root);

	//char* RtChild3 = root("RtChild3");
}


/* ================================================
* 변환 연산자(반환 연산자) 검증용 테스트
* - Value(char*) 꺼내기
* - 자식 Obj를 XmlObj*로 꺼내서 체이닝하기
* - 속성(Attr) 값 꺼내기
* - 범위 밖 인덱스 접근 시 크래시 없이 nullptr 나오는지
* - Value가 아닌 컨테이너 노드를 char*로 캐스팅했을 때 nullptr 나오는지
==================================================*/
void XmlConvertOperTest(){
	XmlObj root("Root");

	// [Test1] operator()(char*)로 자식 자동생성 + operator=(char*)로 Value 세팅
	//         -> explicit (char*) 변환 연산자로 다시 읽어오기
	root("Config") = "1.0";
	char* val1 = (char*)root("Config");
	printf("[Test1] Value 추출              : %-8s (기대값: 1.0)\n", val1 ? val1 : "(null)");

	// [Test2] "Server" 자식을 만들고, XmlObjOper -> XmlObj* 로 뽑아낸 뒤
	//         그 포인터 위에서 operator()로 체이닝 (XmlObjOper 자체엔 operator()가 없기 때문)
	root("Server");
	XmlObj* serverPtr = (XmlObj*)root("Server");
	if (serverPtr){
		(*serverPtr)("Port") = "8080";
	}
	printf("[Test2] Obj* 추출               : %-8s (기대값: OK)\n", serverPtr ? "OK" : "FAIL(null)");
	if (serverPtr){
		printf("        serverPtr->getName()    : %-8s (기대값: Server)\n", serverPtr->getName());
	}

	// [Test3] 뽑아낸 포인터로 손자(Port) Value까지 읽어오기
	if (serverPtr){
		char* portVal = (char*)(*serverPtr)("Port");
		printf("[Test3] 손자 Value 추출          : %-8s (기대값: 8080)\n", portVal ? portVal : "(null)");
	}

	// [Test4] 속성(Attr) Value 추출 - root가 AssignAttrOper를 상속받아 바로 []로 접근 가능
	root["version"] = "2.0";
	char* attrVal = (char*)root["version"];
	printf("[Test4] Attr Value 추출          : %-8s (기대값: 2.0)\n", attrVal ? attrVal : "(null)");

	// [Test5] 범위 밖 Obj 인덱스 -> nullptr이어야 하고, 크래시가 나면 안 됨
	char* crash1 = (char*)root(999);
	printf("[Test5] 범위밖 Obj 인덱스 안전성  : %-8s (기대값: (null), 크래시 없어야 정상)\n", crash1 ? crash1 : "(null)");

	// [Test6] 범위 밖 Attr 인덱스 -> nullptr이어야 하고, 크래시가 나면 안 됨
	char* crash2 = (char*)root[999];
	printf("[Test6] 범위밖 Attr 인덱스 안전성 : %-8s (기대값: (null), 크래시 없어야 정상)\n", crash2 ? crash2 : "(null)");

	// [Test7] Test2에서 Server는 Port를 담은 컨테이너로 바뀌었으므로(더이상 Value 노드 아님)
	//         char*로 캐스팅하면 isVal() 체크에 걸려서 nullptr이 나와야 정상
	char* objAsVal = (char*)root("Server");
	printf("[Test7] 컨테이너를 Value로 캐스팅 : %-8s (기대값: (null), Server는 이제 컨테이너)\n", objAsVal ? objAsVal : "(null)");

	printf("=== 트리 전체 구조 출력 ===\n");
	XmlPrint(&root);
}

void XmlParserTest(){
	FILE* pFile = FileFunc::RdFile("./Test/XmlTest.xml");
	DynamicStr RdStr(64);
	FileFunc::FileToStr(pFile, RdStr);

	printf("%s\n", RdStr.Get_uStr());

	printf("파서 시작\n");
	XmlObj root(&RdStr);
}


void XmlTestFunc(){
	XmlTest();
	//XmlConvertOperTest();
	//XmlParserTest();
}
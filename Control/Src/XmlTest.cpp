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

// 01. 기본 트리 구성 + 출력
// 기대: Root 아래 Child1, Child2가 자식으로 출력됨
void Test01_BasicPrint(){
	XmlObj root("Root");
	root.setObj("Child1");
	root.addObj("Child2");

	XmlPrint(&root);
}

// 02. 속성 기본 - setAttr/addAttr
// 기대: Root 태그에 Attr1="A", Attr2="B" 두 속성이 붙어서 출력됨
void Test02_AttrBasic(){
	XmlObj root("Root");
	root.setAttr("Attr1", "A");
	root.addAttr("Attr2", "B");

	XmlPrint(&root);
}

// 03. operator() 인덱스로 자식 접근 + << 체이닝
// 기대: Child1 안에 rVal이 자식으로 deep copy 되어 들어감
void Test03_ObjIndexAndChaining(){
	XmlObj root("Root");
	root.setObj("Child1");
	root.addObj("Child2");

	XmlObj rVal("rVal");
	rVal.addObj("rValChild");

	root(0) << rVal;   // 0번 자식(Child1)의 자식으로 rVal deep copy 추가

	XmlPrint(&root);
}

// 04. operator() 이름으로 자식 접근
// 기대: 03번과 동일한 결과, "Child1"이라는 이름으로 찾아서 접근
void Test04_ObjIndexByName(){
	XmlObj root("Root");
	root.setObj("Child1");

	XmlObj rVal("rVal");
	rVal.addObj("rValChild");

	root("Child1") << rVal;

	XmlPrint(&root);
}

// 05. operator[] 속성 자동 생성
// 기대: 원래 없던 "NewAttr" 속성이 자동으로 만들어지면서 값이 세팅됨
void Test05_AttrAutoCreate(){
	XmlObj root("Root");
	root.setAttr("Attr1", "A");

	root["NewAttr"] = "created!";

	XmlPrint(&root);
}

// 06. 완전 대입(=) - 이름까지 통째로 교체
// 기대: root의 태그명이 "Root"에서 "rVal"로 바뀌고, 기존 자식/속성은 사라짐
void Test06_FullReplace(){
	XmlObj root("Root");
	root.setAttr("OldAttr", "old");
	root.addObj("OldChild");

	XmlObj rVal("rVal");
	rVal.setAttr("NewAttr", "new");
	rVal.addObj("NewChild");

	printf("=== 대입 전 ===\n");
	XmlPrint(&root);

	root = rVal;

	printf("=== 대입 후 ===\n");
	XmlPrint(&root);
}

// 07. << 로 자식 추가 (Root 자체는 유지)
// 기대: Root 태그는 그대로, 그 안에 rVal이 새 자식으로 deep copy 추가됨
void Test07_AppendChild(){
	XmlObj root("Root");
	root.addObj("ExistingChild");

	XmlObj rVal("rVal");
	rVal.addObj("rValChild");

	root << rVal;

	XmlPrint(&root);
}

// 08. 대리 객체를 거친 속성 접근 - root(0)["Attr"]
// 예전엔 SearchObjAttr 버그로 크래시 나던 경로, 지금은 정상 동작해야 함
// 기대: Child1의 속성으로 "Status=ready"가 붙음
void Test08_ProxyAttrAccess(){
	XmlObj root("Root");
	root.setObj("Child1");
	root.addObj("Child2");

	root(0)["Status"] = "ready";

	XmlPrint(&root);
}

// 09. 대리 객체를 거친 대입 - root(0) = rVal
// 기대: Child1이 rVal의 내용(이름+속성+자식)으로 통째로 교체됨
void Test09_ProxyAssign(){
	XmlObj root("Root");
	root.setObj("Child1");
	root.addObj("Child2");

	XmlObj rVal("rVal");
	rVal.setAttr("From", "rVal");
	rVal.addObj("rValChild");

	root(0) = rVal;

	XmlPrint(&root);
}

// 10. Deep copy 독립성 확인
// 기대: rVal을 나중에 바꿔도 이미 복사되어 들어간 root 쪽엔 영향 없어야 함
void Test10_DeepCopyIndependence(){
	XmlObj root("Root");
	XmlObj rVal("rVal");
	rVal.addObj("Original");

	root << rVal;   // 이 시점 상태로 deep copy

	rVal.addObj("AddedAfterCopy");   // 복사 이후에 rVal을 변경

	printf("=== root (복사본, AddedAfterCopy가 없어야 정상) ===\n");
	XmlPrint(&root);

	printf("=== rVal (원본, AddedAfterCopy가 있어야 정상) ===\n");
	XmlPrint(&rVal);
}

// 11. Value 노드 vs Object 노드 구분
// 기대: 처음엔 Value(빈 문자열), setVal 후에도 Value, addObj 후엔 Object로 전환됨
void Test11_ValueVsObject(){
	XmlObj node("Node");
	printf("생성 직후 isVal(): %s\n", node.isVal() ? "true" : "false");

	node.setVal("hello");
	printf("setVal 후 isVal(): %s\n", node.isVal() ? "true" : "false");

	node.addObj("Child");
	printf("addObj 후 isVal(): %s\n", node.isVal() ? "true" : "false");
}

// 12. XmlObjOper끼리 대입 - root(0) = root(1)
// 기대: Child1이 Child2의 내용으로 교체됨 (지난번 고친 rhs.GetTarget() 경로 확인용)
void Test12_OperToOperAssign(){
	XmlObj root("Root");
	root.setObj("Child1");
	root.addObj("Child2");
	root(1)["Marker"] = "fromChild2";

	printf("=== 대입 전 ===\n");
	XmlPrint(&root);

	root(0) = root(1);

	printf("=== root(0) = root(1) 이후 ===\n");
	XmlPrint(&root);
}

// 13. 없는 인덱스/이름 접근 - 크래시 안 나고 안전하게 처리되는지
void Test13_NotFound(){
	XmlObj root("Root");
	root.setObj("Child1");

	root(99);            // 범위 밖 인덱스
	root("NoSuchChild");  // 없는 이름
	root["NoSuchAttr"];   // 없는 속성 (읽기만) — 자동생성 되는지 확인

	XmlPrint(&root);      // 여기서 이상한 게 생겼는지 확인
}

// 14. 빈 Root에 이것저것
void Test14_EmptyRoot(){
	XmlObj root("Root");   // 자식도 속성도 없음
	root(0);               // 자식 없는데 0번 접근
	root["Attr"] = "val";  // 빈 상태에서 속성 대입

	XmlPrint(&root);
}

// 15. 같은 이름 자식이 여러 개일 때
void Test15_DuplicateNames(){
	XmlObj root("Root");
	root.setObj("Same");
	root.addObj("Same");
	root.addObj("Same");

	root("Same")["Which"] = "me";  // 이름으로 찾으면 몇 번째가 걸리는지

	XmlPrint(&root);
}

// 16. 자기 자신 대입 - root = root
void Test16_SelfAssign(){
	XmlObj root("Root");
	root.setObj("Child1");
	root.setAttr("A", "1");

	root = root;   // DelVals/DelAttrs로 먼저 지우는데 자기 자신이면? (위험 케이스)

	XmlPrint(&root);
}

// 17. 깊은 중첩 트리
void Test17_DeepNesting(){
	XmlObj root("L0");
	root.setObj("L1");
	//root(0).operator()(-1);  // 필요시 파고들기

	XmlObj deep("L0");
	XmlObj* cur = &deep;
	for (int i = 1; i <= 5; i++){
		char name[16];
		sprintf(name, "L%d", i);
		cur->setObj(name);
		cur = cur->getObj(0);
	}
	XmlPrint(&deep);
}

// 18. 속성 값 덮어쓰기 - 같은 속성에 두 번 대입
void Test18_AttrOverwrite(){
	XmlObj root("Root");
	root["Color"] = "red";
	root["Color"] = "blue";   // 새로 만드는 게 아니라 덮어써야 정상

	XmlPrint(&root);          // Color가 하나만, blue로 나와야 함
}

// 19. 연쇄 체이닝
void Test19_MultiChain(){
	XmlObj root("Root");
	root.setObj("Child1");

	XmlObj a("A"), b("B");
	root(0) << a;   // Child1에 A 추가
	root(0) << b;   // Child1에 B도 추가

	XmlPrint(&root);
}


void XmlTestFunc(){
	printf("[시작] Test13_NotFound\n");
	Test13_NotFound();
	printf("\n\n\n\n");

	printf("[시작] Test14_EmptyRoot\n");
	Test14_EmptyRoot();
	printf("\n\n\n\n");

	printf("[시작] Test15_DuplicateNames\n");
	Test15_DuplicateNames();
	printf("\n\n\n\n");

	printf("[시작] Test16_SelfAssign\n");
	Test16_SelfAssign();
	printf("\n\n\n\n");

	printf("[시작] Test17_DeepNesting\n");
	Test17_DeepNesting();
	printf("\n\n\n\n");

	printf("[시작] Test18_AttrOverwrite\n");
	Test18_AttrOverwrite();
	printf("\n\n\n\n");

	printf("[시작] Test19_MultiChain\n");
	Test19_MultiChain();
	printf("\n\n\n\n");
}
#include "../Include/Lib.h"

void main(){

	//Test::TestMain t;
	//t.TestFuncMain();
	XmlObj Root;
	Root() << "Test";		//값을 넣는 경우
	XmlObj Root1;

	//삽입의 경우
	Root() << Root1;		//Chainning
	Root() << &Root1;		//Deep Copy

	Root(1) << Root1;		//특정 인덱스에 넣기
	Root(2) << Root1;		//만약에 인덱스가 없는경우 그냥 맨뒤에 넣기?
	Root() << Root1;		//맨뒤에 넣기
	
	Root["Test"] = "Test";	//속성값 넣기
	
	//반환의 경우
	char* str = Root["Test"];	//속성 반환
	//str = Root;					//값을 반환?
	XmlObj* pObj;
	//pObj = Root;				//객체를 반환?
	pObj = (XmlObj*)Root();
	pObj = (XmlObj*)Root(2);				

	while (1){
		fflush(stdin);
		printf("아무키나 누르면 종료 : ");
		getchar();
		break;

		fflush(stdin);
	}
}
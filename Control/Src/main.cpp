#include "../Include/Lib.h"

void main(){

	Test::TestMain t;
	t.TestFuncMain();

	while (1){
		fflush(stdin);
		printf("아무키나 누르면 종료 : ");
		getchar();
		break;

		fflush(stdin);
	}
}
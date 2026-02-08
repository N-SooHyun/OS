#include "../Include/GUI.h"


//실제 동작하는 GUI 메인 스레드
int Gui_Main(){
	//실제로 계속 동작을 하는지를 확인하는 부분
	Gui_Root gui;

	while (1){
		//gui.Test();
		//gui.Loading_Graph();
		gui.Loading_Graph_Sleep_Blocked();
		fflush(stdin);
		//getchar();
		//break;
		fflush(stdin);
	}

	return 0;
}
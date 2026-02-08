#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include "../../Lib/Include/Timer.h"
#include "../../Lib/Include/Str.h"

int Gui_Main();


class Gui_Root{
	int StartFocus;
	bool PrintTick;
	long long CurTime;
	TimerFunc::Timer Gui_Time = TimerFunc::Timer();
	std::condition_variable cv;
	void Init_Gui(){
		if (Gui_Time.Start() == false){
			printf("Timer Thread Failed");
		}
	}
public:
	Gui_Root() : cv(){ 
		StartFocus = 0; 
		PrintTick = false;
		CurTime = 0;
		Init_Gui();
	}
	~Gui_Root(){}

	//그래픽이 계속 동작하는지 확인하는 부분
	//바쁜 대기로 스레드 주체자인 자신이 타이머를 통해서 결정하기에
	//계속 실행됨 (Timer, Gui 두개의 스레드가 계속 수행되서 부담스러움)
	void Loading_Graph_OnlyRunning(){
		static long long lastDrawTime = 0;
		CurTime = Gui_Time.getTime();
		if (CurTime - lastDrawTime >= 1000){
			lastDrawTime = CurTime;

			system("cls");

			if (StartFocus >= 20)
				StartFocus = 0;

			for (int i = 0; i < 20; i++) {
				if (i == StartFocus)
					printf("*");
				else
					printf("-");
			}

			//printf("\n");
			StartFocus++;
		}
	}


	//Os에게 그릴때 아니면 Block하여 수시로 대기상태로 변환함
	//Sleep기반이라 시간 조건으로 대기 -> 준비 상태가 됨
	//이벤트기반도 있는데 이건 조건기반으로 대기 -> 준비로 변환하기 떄문에 좀더 유리하나
	//현재 구조인 시간조건으로 ReDraw를 하는 구조라면 Sleep이나 이벤트나 똑같은 시간조건이라 의미없음
	void Loading_Graph_Sleep_Blocked(){
		system("cls");

		if (StartFocus >= 20)
			StartFocus = 0;

		for (int i = 0; i < 20; i++) {
			if (i == StartFocus)
				printf("*");
			else
				printf("-");
		}

		StartFocus++;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
};
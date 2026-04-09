#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <functional>
#include "../../Lib/Include/Timer.h"
#include "../../Lib/Include/Str.h"

int Gui_Main();

//콜백함수 개념에 대해서 알아볼것
//사용자 입장에서의 콜백에 불과함
class Call_Back{
public:
	Call_Back(){}
	~Call_Back(){}
	//1. 코드 재사용성과 확장성 증가
	void Fst_Func(void(*callback)(int)){
		printf("[Fst_Func] 시작\n");
		for (int i = 1; i <= 3; i++){
			callback(i);
		}
		printf("[Fst_Func] 끝\n\n");
	}

	//2. 제어의 역전 (Inversion of Control)
	void Scd_Func(void(*callback)()){
		printf("[Scd_Func] 이벤트 발생 시 콜백 호출\n");
		//이벤트 발생하면 콜백 호출
		if (1){
			callback;
		}
		printf("[Scd_Func] 끝\n\n");
	}
	//3️. 동기/비동기 처리 유연성
	void Trd_Func(void(*callback)(int), bool async = false){
		printf("[Trd_Func] 시작\n");

		//비동기일때
		if (async){
			std::thread([callback](){
				std::this_thread::sleep_for(std::chrono::seconds(1));
				callback(42);
			}).detach();
			printf("[Trd_Func] 비동기 작업 시작\n");
		}
		else{ //동기일때
			callback(42);
		}

		printf("[Trd_Func] 끝\n\n");
	}

	//4. 라이브러리/프레임워크와 호환
	void Fr_Func(int(*callback)(int,int)){
		printf("[Fr_Func] 콜백으로 연산 수행\n");
		int result = callback(10, 5);
		printf("[Fr_Func] 결과 : %d\n\n", result);
	}

	//5. 결과 처리 책임 분리
	void Fsth_Func(void(*callback)(char*)){
		Dynamic::DynamicStr data(32);
		data.Set_Str("Hello Callback!");

		printf("[Fsth_Func] 데이터 준비 완료\n");
		callback(data.Get_Str());
		printf("[Fsth_Func] 끝\n\n");
	}

	// 콜백 함수의 기능? 
	void CallBack_Func(){
		// 1. 람다식으로 호출할 수 있음(람다 -> 이름이 없는 함수)
		Fst_Func([](int x){printf("[CallBack1 값 2배 : %d\n", x * 2); });
		Fst_Func([](int x){printf("[CallBack1 값 제곱 : %d\n", x*x); });


		// 2. 제어의 역전
		Scd_Func([]() {printf("[CallBack2] 이벤트처리\n"); });

		// 3. 동기/비동기 처리 유연성
		Trd_Func([](int x){printf("[Callback3] 동기 값 : %d\n", x); }, false);
		Trd_Func([](int x){printf("[Callback3] 비동기 값 : %d\n", x); }, true);

		// 4. 라이브러리/프레임워크와 호환
		Fr_Func([](int a, int b) -> int { return a + b; });
		Fr_Func([](int a, int b) -> int { return a * b; });

		// 5. 결과 처리 책임 분리
		Fsth_Func([](char* msg) { printf("[Callback5] 처리 : %s\n", msg); });
	}

	//이벤트 기반에서의 콜백 함수
	//주의점은 현재 예제에서는 B스레드가 콜백함수를 실행하나 A스레드는 해당 콜백이 실행되었는지 안되었는지 알방법이 없음
	//하지만 B스레드가 자신의 작업이 끝날때 콜백함수로 자신이 처리를 해버리니 즉각처리긴 함
	void performHeavyTask(int data, void(*callback)(int)){
		//별도의 스레드에서 동작한다는 가정
		std::thread t([data, callback](){
			printf("[B 스레드] 무거운 작업 시작(3초 소요..)\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));

			int result = data * 2;

			printf("[B 스레드] 작업 완료 콜백 호출 종료\n");
			callback(result);
		});

		t.detach();
	}

	void CallBack_Event(){
		printf("[A 스레드] 프로그램 시작\n");

		//콜백 함수 정의
		void(*call)(int) = [](int result){
			printf("[CallBack] 작업 결과 : %d\n", result);
			printf("[CallBack] 이벤트 갱신\n");
		};
		
		performHeavyTask(100, *call);

		printf("[A 스레드] B에게 일을 시켰고, 나는 한던 일 진행\n");

		for (int i = 0; i < 5; i++){
			printf("[A 스레드] 메인 루프 실행 중..\n");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	


	void main(){
		//CallBack_Func();
		CallBack_Event();
	}
};


class Gui_Form{
private:
	void tm_Callback(int tick){

	}

	// 👉 1. 콜백 (WndProc)
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// 커널이 내 스레드를 가로채서 여기로 점프시켜야만 찍히는 로그
		//printf(">>> [WndProc] 호출됨! (메시지: %u)\n", msg);

		switch (msg)
		{
		case WM_LBUTTONDOWN:
			printf("3초동안 멈출건데 가능?\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			std::cout << "[WndProc] Mouse Click!\n";
			break;

		case WM_KEYDOWN:
			std::cout << "[WndProc] Key Down: " << (int)wParam << "\n";
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

public:
	int run(){
		// 👉 2. 윈도우 클래스 등록
		WNDCLASS wc = {};
		wc.lpfnWndProc = WndProc; // 🔥 콜백 등록
		wc.lpszClassName = "MyWindowClass";

		RegisterClass(&wc);

		// 👉 3. 창 생성 (HWND 생성)
		HWND hwnd = CreateWindowEx(
			0,
			"MyWindowClass",
			"My Window",
			WS_OVERLAPPEDWINDOW,
			100, 100, 500, 400,
			NULL, NULL, NULL, NULL
			);

		ShowWindow(hwnd, SW_SHOW);

		// 👉 4. 메시지 루프
		MSG msg;		
		while (true)
		{

			//while (1){
			//	//5초 동안 커널 진입을 안하게 되면?
			//	printf("--- 실험 시작: 5초간 커널 진입 안 하고 무한 루프 돌기 ---\n");
			//	printf("--- 이 동안 창을 클릭하거나 움직여보세요. 로그가 찍히나 확인! ---\n");
			//	std::this_thread::sleep_for(std::chrono::seconds(2));
			//	break;
			//}

			// 🔥 여기서 block됨 (메시지 없으면 잠듦)
			if (GetMessage(&msg, NULL, 0, 0) <= 0)
				break;

			//std::cout << "[Loop] Message received: " << msg.message << "\n";

			// 👉 메시지를 WndProc으로 전달
			DispatchMessage(&msg);
		}
		return 0;
	}

};



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
		while (1){
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
	}


	//컴포넌트 기반 실제 Gui 베이스 개발
	void Main_Component(){

	}

};
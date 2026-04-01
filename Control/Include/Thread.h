#pragma once
#include <thread>
#include <iostream>
#include <stdio.h>
#include <Windows.h>

int Thread_Main();

//시스템콜을 활용한 방법 스레드 라이브러리와는 다르게 순수 함수주소가 필요 static으로 처리
class NoSync_Low_Thread{
	static int counter;
	DWORD static WINAPI ThreadFunc(LPVOID lpParam){
		for (int i = 0; i < 100000; i++){
			counter++;
		}
		return 0;
	}
public:
	int Main(){
		HANDLE t1 = CreateThread(NULL, 0, &NoSync_Low_Thread::ThreadFunc, NULL, 0, NULL);
		HANDLE t2 = CreateThread(NULL, 0, &NoSync_Low_Thread::ThreadFunc, NULL, 0, NULL);

		WaitForSingleObject(t1, INFINITE);
		WaitForSingleObject(t2, INFINITE);

		std::cout << counter << std::endl;

		CloseHandle(t1); 
		CloseHandle(t2);

		return 0;
	}

};


//스레드 라이브러리를 활용한 방법 스레드 라이브러리 내부적으로 함수를 1개 만들어서 그 함수가 멤버메소드같은 처리를 해줌
//그리고 만들어진 함수1개를 CreateThread에 넘겨줘서 처리를 해줌
class NoSync_Thread{
	int counter = 0;

public:
	void work(){
		for (int i = 0; i < 100000; i++){
			counter++;
		}
	}

	int Main(){
		//this->work는 해당 인스턴스의 work를 부르겠다임 주소아님
		std::thread t1(&NoSync_Thread::work, this);		//실제 함수포인터주소로 스레드에게 넘겨야함
		std::thread t2(&NoSync_Thread::work, this);		//또한 함수포인터만으로는 해당 함수를 누가 호출하는지를 모름 그래서 this 인스턴스를 넣어줘야함

		t1.join();
		t2.join();

		std::cout << counter << std::endl;

		return 0;
	}
};

#pragma once
#include <iostream>
#include <windows.h>

namespace TimerFunc{
	class Timer{
		__int32 ElapsedTime;		//msec
		bool isTimeStp = true;		//정지유무
		HANDLE hTimer = NULL;		//신호용 핸들러
		HANDLE hThread = NULL;		//스레드용 핸들러

		//스레드가 사용하는 함수 프로세스
		static DWORD WINAPI ThreadProc(LPVOID pArg){
			Timer* pThis = (Timer*)pArg;
			while (1){
				if (pThis->isTimeStp)
					break;

				DWORD dw = WaitForSingleObject(pThis->hTimer, INFINITE);
				if (dw == WAIT_OBJECT_0){
					if (!pThis->isTimeStp){
						pThis->ElapsedTime += 100;
					}
				}
				else{
					break;
				}
			}
			return 0;
		}
	public:
		Timer(){
			hTimer = CreateWaitableTimer(NULL, FALSE, NULL);			//신호용 핸들러 생성
			hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);		//스레드 생성
		}
		~Timer(){
			Stop();
			if (hTimer) CloseHandle(hTimer);
			if (hThread) TerminateThread(hThread, 0);
		}

		bool Start(){
			if (!hTimer) return false;
			LARGE_INTEGER liDueTime;
			liDueTime.QuadPart = 0;    // 절대시간이 아니라 0은 '바로 실행'
			//바로 시작하셈
			BOOL b = SetWaitableTimer(
				hTimer,        // 타이머 오브젝트
				&liDueTime,			   // 언제 처음 신호를 줄지 (시간)
				100,          // 주기 (ms) : 0.1초에 한 번 반복
				NULL,          // APC 콜백 없음
				NULL,          // APC 호출 인자 없음
				FALSE          // system resume 알람 아님
				);

			isTimeStp = false;
			return true;
		}
		bool Stop(){
			CancelWaitableTimer(hTimer);
			isTimeStp = true;
			return true;
		}

		bool Clear(){
			if (isTimeStp){
				ElapsedTime = 0;
				return true;
			}
			return false;
		}

		unsigned int getTime(){
			return (unsigned int)ElapsedTime;
		}

	};


}
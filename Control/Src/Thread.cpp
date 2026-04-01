#include "../Include/Thread.h"
int NoSync_Low_Thread::counter = 0;
int Thread_Main(){
	NoSync_Low_Thread ns_l_t;
	NoSync_Thread ns_t;

	ns_l_t.Main();
	ns_t.Main();

	return 0;
}
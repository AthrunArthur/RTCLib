//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24
#include "DemoUse.h"
#include <iostream>

#pragma comment(lib, "Winmm.lib")

void consum_sh_mem_data(const char *pData, size_t s, LPVOID lpParam);
/*
|---|--------|---|-------...
| s1|    s2  | s1|   s2  ...
|   S        E   S       ...
*/
class EventSimulator{
public:
	//! s1---s2--s1--s2--....
	EventSimulator(DWORD s1, DWORD s2)
		: m_s1(s1), m_s2(s2)
	{
		QueryPerformanceFrequency(&Frequency);
		m_bIsS1 = true;
		m_LastTime = 0;
		timeGetDevCaps(&m_TimeCaps, sizeof(TIMECAPS));
		
		m_wTimerRes = min(max(m_TimeCaps.wPeriodMin, 1), m_TimeCaps.wPeriodMax);
		timeBeginPeriod(m_wTimerRes);
	}
	~EventSimulator()
	{
		timeKillEvent(m_wTimerID);
		timeEndPeriod(m_wTimerRes);
	}
	void setParam(IPCSignal_t * s, IPCSignal_t * e, IPCSignal_t * r, IPCShMem_t * m)
	{
		S = s;
		E = e;
		R = r;
		M = m;
	}
	void startSim(){
		m_wTimerID = timeSetEvent(10, 1, onTimeProc, (DWORD)(this), TIME_PERIODIC);
	}

	//! Start信号的回调函数
	void  onStart()
	{
		S->send_signal();
	}
	//! End信号的回调函数
	void onEnd()
	{
		E->send_signal();
	}
protected:
	void time_proc(UINT uID, UINT uMsg, DWORD dwUsers, DWORD dw1, DWORD dw2)
	{
		if(m_LastTime == 0){
			m_LastTime = current_milliseconds();
			return ;
		}
		LONGLONG ctime = current_milliseconds();
		LONGLONG elapse = ctime - m_LastTime;
		if(m_bIsS1)
		{
			if(elapse >= m_s1){
				m_LastTime = ctime;
				m_bIsS1 = false;
				onStart();
			}
		}
		else
		{
			if(elapse >= m_s2){
				m_LastTime = ctime;
				m_bIsS1 = true;
				onEnd();
			}
		}
	}
	static void CALLBACK onTimeProc(UINT uID, UINT uMsg, DWORD dwUsers, DWORD dw1, DWORD dw2)
	{
		EventSimulator * p= (EventSimulator *)(dwUsers);
		p->time_proc(uID, uMsg, dwUsers, dw1, dw2);
	}

	LONGLONG current_milliseconds(){
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		t.QuadPart= t.QuadPart*1000/Frequency.QuadPart;
		return t.QuadPart;
	}
protected:
	DWORD m_s1;
	DWORD m_s2;
	TIMECAPS m_TimeCaps;
	UINT m_wTimerRes;
	UINT m_wTimerID;
	IPCSignal_t * S;
	IPCSignal_t * E;
	IPCSignal_t * R;
	IPCShMem_t * M;
	LARGE_INTEGER Frequency;
	LONGLONG m_LastTime;
	bool m_bIsS1;
};
void consum_sh_mem_data(const char * pData, size_t s, LPVOID lpParam)
{
	//!数据可读时的回调函数
	std::cout<<"get data!"<<std::endl;
	//!Consume the data here!
	std::cout<<pData<<std::endl;
}

int main(int argc, char *argv[])
{
	//! Intializing code.
	ff::EDefaultErrorHandler eh;
	IPCSignal_t startSig(&eh); //Start信号
	IPCSignal_t endSig(&eh);  //End 信号
	IPCSignal_t readSig(&eh); //数据可读信号
	IPCShMem_t  shmem(&eh); //共享内存
	startSig.initialize(START_SEM_NAME, 0);
	endSig.initialize(END_SEM_NAME, 0);
	readSig.initialize(READ_SEM_NAME, 0);
	shmem.initialize(SH_MEM_NAME, SH_MEM_SIZE);
	DemoServerDataHandler dh(&eh);

	dh.initialize_with_thread(&readSig, &shmem, consum_sh_mem_data, NULL); //指定数据可读时的回调函数
	//! End initialization.

	//以下代码用于模拟时间信号。
	EventSimulator es(10, 100);
	es.setParam(&startSig, &endSig, &readSig, &shmem);
	es.startSim();

	std::cout<<"press any key to quit..."<<std::endl;
	getchar();
	return 0;
}
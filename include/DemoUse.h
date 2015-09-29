//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24

//! We define all real-world types and macros here.

#pragma once
#include "IPCSignal.h"
#include "SignalHandler.h"
#include "IPCShMem.h"
#include <vector>
#include <Windows.h>
#include <map>
#include <queue>

//! NOTE: you may want to use your own error handler instead of EDefaultErrorHandler;

typedef ff::IPCSignal<1, ff::EDefaultErrorHandler> IPCSignal_t;
typedef ff::SingleSignalHandler<IPCSignal_t, ff::EDefaultErrorHandler> SignalHandler_t;
typedef ff::MultiSignalHandler<IPCSignal_t, ff::EDefaultErrorHandler> MSignalHandler_t;
typedef ff::IPCShMem<ff::EDefaultErrorHandler> IPCShMem_t;


#define START_SEM_NAME TEXT("my_app_start_semaphore")
#define END_SEM_NAME TEXT("my_app_end_semaphore")
#define READ_SEM_NAME TEXT("my_app_read_semaphore")
#define SH_MEM_NAME  TEXT("my_app_sh_mem")

#define SH_MEM_SIZE 1024*1024*20

#define MAX_BLOCK_TIME 10

//typedef long long index_t;
typedef long index_t;
class DemoServerDataBuffer{
public:
	typedef void (*DataHandler) (const char * pData, size_t s, LPVOID);

	DemoServerDataBuffer(size_t s, size_t buf_size);
	virtual ~DemoServerDataBuffer();

	void write_to_head(const char * buf, size_t len);

	void handle_tail(DataHandler pHandler, LPVOID param, DWORD time_out);
protected:
	void lock();

	void unlock();
protected:
	typedef std::pair<char *, size_t> buf_t;
	typedef std::vector<buf_t> mbuf_t;
	std::queue<buf_t> mo_filled_buf;
	mbuf_t mo_recycle_buf;
	HANDLE m_mutex;
	size_t mi_buf_size;
	HANDLE mh_sem;
};

//! This is a a data handler for the server side. 
//! We define this to wrap the details of handling shared memory. 
class DemoServerDataHandler: public SignalHandler_t
{
public:
	typedef void (*DataHandler)(const char * pData, size_t s, LPVOID);
	typedef SignalHandler_t::EH_t  EH_t;
	typedef SignalHandler_t::IPCSignal_ptr IPCSignal_ptr;

	DemoServerDataHandler(EH_t * pErrorHandler);
	~DemoServerDataHandler();

	//!Initialize the data handler with signal, shared memory and callback function.
	/* @pSignal -- the signal to wait for;
	 * @pMem -- the shared memory;
	 * @pHandler -- the callback function;
	 * @param -- the additional parameter for the callback function.
	*/
	void initialize_with_thread(IPCSignal_ptr pSignal, IPCShMem_t *pMem,  DataHandler pHandler, LPVOID param);

	void initialize(IPCSignal_ptr pSignal, IPCShMem_t *pMem);

	DemoServerDataBuffer & get_data_buffer() {return mo_buf;}

protected:
	static DWORD WINAPI thread_call_back_func(LPVOID lpParam)
	{
		DemoServerDataHandler * p = (DemoServerDataHandler *)(lpParam);
		while(!p->m_bDemoThreadStopped)
		{
			p->mo_buf.handle_tail(p->m_pHandler, p->m_pParam, 100);
		}
		CloseHandle(p->m_hThread);
		ReleaseSemaphore(p->mh_clear_to_quit, 1, NULL);
		p->mh_thread=NULL;
		return 0;
	}

	static DWORD recv_sh_mem(LPVOID lpParam);

	DataHandler m_pHandler;
	LPVOID m_pParam;
	IPCShMem_t * m_pShMem;
	HANDLE mh_thread;
	HANDLE mh_clear_to_quit;
	DemoServerDataBuffer mo_buf;
	bool m_bDemoThreadStopped;
};


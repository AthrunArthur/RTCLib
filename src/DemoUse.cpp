//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24
#include "DemoUse.h"

#include <iostream>

DemoServerDataBuffer::DemoServerDataBuffer(size_t s, size_t buf_size)
{
	for(size_t i = 0; i < s; i++){
		char * buf = new char[buf_size];
		if(buf == NULL){
			assert(false && "allocation failed in DemoServerDataBuffer::DemoServerDataBuffer");
			exit(-5);
		}
		std::pair<char *, size_t> b;
		b.first = buf;
		b.second = buf_size;
		mo_recycle_buf.push_back(b);
	}
	m_mutex = CreateMutex(NULL, false, NULL);
	if(m_mutex == NULL){
		assert(false && "CreateMutex failed!");
		exit(-4);
	}
	mh_sem = ::CreateSemaphore(NULL, 0, 1, NULL);
	mi_buf_size = buf_size;
}

DemoServerDataBuffer::~DemoServerDataBuffer(){
	while(!mo_filled_buf.empty()){
		buf_t b = mo_filled_buf.front();
		delete[] b.first;
		mo_filled_buf.pop();
	}
	for(size_t i = 0; i < mo_recycle_buf.size(); ++i){
		delete[] mo_recycle_buf[i].first;
	}
	CloseHandle(mh_sem);
}

void DemoServerDataBuffer::write_to_head(const char * buf, size_t len){
	lock();
	buf_t tbuf;
	if(mo_recycle_buf.size() == 0)
	{
		tbuf = mo_filled_buf.front();
		mo_filled_buf.pop();
	}
	else{
		tbuf = mo_recycle_buf.back();
		mo_recycle_buf.pop_back();
	}
	unlock();
	memset(tbuf.first, 0, tbuf.second);
	memcpy(tbuf.first, buf, len);
	lock();
	mo_filled_buf.push(tbuf);
	unlock();
	ReleaseSemaphore(mh_sem, 1, NULL);
}

void DemoServerDataBuffer::handle_tail(DataHandler pHandler, LPVOID param, DWORD time_out){
	DWORD ret = WaitForSingleObject(mh_sem, time_out);
	if(ret == WAIT_TIMEOUT || ret == WAIT_FAILED || ret == WAIT_ABANDONED){
		return ;
	}
	lock();
	buf_t tbuf = mo_filled_buf.front();
	mo_filled_buf.pop();
	unlock();
	(*pHandler)(tbuf.first, tbuf.second, param);
	memset(tbuf.first, 0, tbuf.second);
	lock();
	mo_recycle_buf.push_back(tbuf);
	unlock();
}

void DemoServerDataBuffer::lock(){
	WaitForSingleObject(m_mutex, INFINITE);
}

void DemoServerDataBuffer::unlock(){
	ReleaseMutex(m_mutex);
}

DemoServerDataHandler::DemoServerDataHandler(EH_t * pErrorHandler)
	: SignalHandler_t(pErrorHandler)
	, m_pHandler(NULL)
	, mo_buf(4, SH_MEM_SIZE){
		mh_clear_to_quit = NULL;
	}

DemoServerDataHandler::~DemoServerDataHandler()
{
	m_bDemoThreadStopped = true;
	if(mh_clear_to_quit){
		WaitForSingleObject(mh_clear_to_quit, INFINITE);
		CloseHandle(mh_clear_to_quit);
	}
}
void DemoServerDataHandler::initialize_with_thread(IPCSignal_ptr pSignal, IPCShMem_t *pMem, DataHandler pHandler, LPVOID param)
{
	m_pParam = param;
	m_pHandler = pHandler;
	m_pShMem = pMem;

	SignalHandler_t::handle_signale(pSignal, MAX_BLOCK_TIME, recv_sh_mem, (LPVOID)(this));
	m_bDemoThreadStopped = false;
	mh_clear_to_quit = ::CreateSemaphore(NULL, 0, 1, NULL);
	mh_thread = CreateThread(0, 0, thread_call_back_func, (LPVOID) this, 0, 0);
	if(m_hThread == NULL){
		m_pErrorHandler->handle_create_thread_failed();
	}
}

void DemoServerDataHandler::initialize(IPCSignal_ptr pSignal, IPCShMem_t *pMem ){
	m_pShMem = pMem;
	SignalHandler_t::handle_signale(pSignal, MAX_BLOCK_TIME, recv_sh_mem, (LPVOID)(this));
}

DWORD DemoServerDataHandler::recv_sh_mem(LPVOID lpParam)
{
	DemoServerDataHandler * p = (DemoServerDataHandler *)(lpParam);
	IPCShMem_t *psh = p->m_pShMem;
	DataHandler ph = p->m_pHandler;
	p->mo_buf.write_to_head(psh->addr(), psh->size());
	return 0;
}




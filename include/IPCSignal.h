//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24
#pragma once
#include <Windows.h>
#include "ErrorHandler.h"

namespace ff{

//! A wrapper for inter-process communication (IPC) signal.
/*
* Basically, we use named semaphore as the low-level implementation. 
* We use @MValue to specify the max value of the semaphore.
*/
template<size_t MValue = 1, typename ErrorHandler_t = EDefaultErrorHandler>
class IPCSignal
{
public:
	IPCSignal(ErrorHandler_t *pErrorHandler)
		: m_pErrorHandler(pErrorHandler)
		, m_hSemaphore(NULL){}

	~IPCSignal(){
		if(m_hSemaphore){
			CloseHandle(m_hSemaphore);
			m_hSemaphore = NULL;
		}
	}

	//! Initailize the IPCSignal with @name and initial value, @init_value.
	void initialize(LPTSTR name, int init_value)
	{
		m_hSemaphore = NULL;
		while(m_hSemaphore == NULL){
			m_hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, name);
			if(m_hSemaphore == NULL){
				m_hSemaphore = CreateSemaphore(NULL, init_value, MValue, name);
			}
		}
	}

	//! Send a signal.
	void	send_signal(){
		ReleaseSemaphore(m_hSemaphore, 1, NULL);
	}

	//! Wait the signal with block.
	//! @return - true if get the signal successfully, otherwise false.
	/*
	* Note: this could be blocking if set @wait_timeout as INFINITE.
	* Usually, you should use a limited timeout value, like 10ms.
	* Also, there could be errors, and thus call 
	* ErrorHandler_t::handle_wait_failed or ErrorHandler_t::handle_wait_abandoned.
	*/
	bool	wait_signal(DWORD wait_timeout=INFINITE)
	{
		DWORD ret = WaitForSingleObject(m_hSemaphore, wait_timeout);
		if(ret == WAIT_TIMEOUT){
			return false;
		}
		if(ret == WAIT_FAILED){
			m_pErrorHandler->handle_wait_failed(m_hSemaphore);
			return false;
		}
		if(ret == WAIT_ABANDONED){
			m_pErrorHandler->handle_wait_abandoned(m_hSemaphore);
			return false;
		}
		return true;
	}

	HANDLE	get_native_handle() const{return m_hSemaphore;}
protected:
	ErrorHandler_t *		m_pErrorHandler;
	HANDLE		m_hSemaphore;
};//end class IPCSignal

}//end namespace ff
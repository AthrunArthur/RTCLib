//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24

/* We define a SignalHandler to wait for a signal, 
 * to triger a callback function when the signal is signaled. 
 * The basic idea is to create a thread, and put all the things into the thread,
 * including waiting for the signal (WaitForSingleObject or WaitForMultipleObjects) 
 * and trigering the callback function.
 *
 * For the implementation, we use "template specialization" to differentiate the handlers 
 * for single and multiple signals.
 */
#pragma once
#include "IPCSignal.h"
#include "ErrorHandler.h"
#include <cassert>
#include <vector>

namespace ff{


//!A wrapper for windows thread.
/* Note: this thread will repeatly call @thread_repeat_func.
*/
template<typename ErrorHandler_t = EDefaultSignalHandler>
class SignalHandlerBase
{
public:
	SignalHandlerBase(ErrorHandler_t * pErrorHandler)
		: m_hThread(NULL)
		, m_pErrorHandler(pErrorHandler)
		, m_bIsStopped(0){}

	//! Create and start the thread.
	//! Will call ErrorHandler_t::handle_create_thread_failed if
	//! fail to create thread.
	void		start(){
		if(m_hThread) return ;

		m_hClearToQuitSemphore = ::CreateSemaphore(NULL, 0, 1, NULL);
		if(m_hClearToQuitSemphore == NULL){
			m_pErrorHandler->handle_create_thread_failed();
			return ;
		}

		m_hThread = ::CreateThread(0,0,thread_call_back_func,
                    (LPVOID)this,0,0);
		if(m_hThread == NULL){
			m_pErrorHandler->handle_create_thread_failed();
			return ;
		}
		
	}

	//! Set the stop flag to make sure the thread exit.
	void stop(){
		InterlockedExchange(&m_bIsStopped, 1);
		WaitForSingleObject(m_hClearToQuitSemphore, INFINITE);
	}

	virtual	void	thread_repeat_func() = 0;
protected:
	static DWORD WINAPI thread_call_back_func(LPVOID lpParam)
	{
		SignalHandlerBase<ErrorHandler_t> * p=(SignalHandlerBase<ErrorHandler_t> *)(lpParam);
		while(p->m_bIsStopped == 0)
		{
			p->thread_repeat_func();
		}
		CloseHandle(p->m_hThread);
		ReleaseSemaphore(p->m_hClearToQuitSemphore, 1, NULL);
		p->m_hThread=NULL;
		return 0;
	}
	HANDLE	m_hThread;
	ErrorHandler_t * m_pErrorHandler;
	LONG	m_bIsStopped;
	HANDLE m_hClearToQuitSemphore;
};


//!Delcare SingleSignalHandler.
//! @Signal_t -- the type of signal to handle;
//! @ErrorHandler_t -- the type of the error handle.
template<typename Signal_t, typename ErrorHandler_t>
class SingleSignalHandler : public SignalHandlerBase<ErrorHandler_t>
{
public:
	typedef ErrorHandler_t		EH_t;
	typedef Signal_t			IPCSignal_t;
	typedef IPCSignal_t *		IPCSignal_ptr;
	typedef DWORD (*Handler_t) (LPVOID);

	SingleSignalHandler(ErrorHandler_t * pErrorHandler)
		: SignalHandlerBase<ErrorHandler_t>(pErrorHandler)
		, m_pErrorHandler(pErrorHandler)
		, m_pSignal(NULL){}

	//! Set the signal and start the thread.
	/* @pSignal -- the pointer of the signal.
	 * @timeout_milliseconds -- the blocking time;
	 * @pHandler -- the callback function when the signal is set;
	 * @param -- additional parameter for the callback function.
	*/
	void handle_signale(IPCSignal_ptr pSignal, DWORD timeout_milliseconds, Handler_t pHandler, LPVOID param)
	{
		assert(m_pSignal == NULL && "This is only for one signal!");
		m_pSignal = pSignal;
		m_dwTimeout = timeout_milliseconds;
		m_pHandler = pHandler;
		m_pParam = param;
		SignalHandlerBase<ErrorHandler_t>::start();
	}
protected:
	virtual void thread_repeat_func()
	{
		if(m_pSignal->wait_signal(m_dwTimeout)){
			if(m_pHandler != NULL){
				(*m_pHandler)(m_pParam);
			}
		}
	}

protected:
	IPCSignal_ptr m_pSignal;
	DWORD	m_dwTimeout;
	ErrorHandler_t * m_pErrorHandler;
	Handler_t m_pHandler;
	LPVOID m_pParam;
};//end class SignalHandler

//!This is another specialization for multiple signals.
template<typename Signal_t, typename ErrorHandler_t>
class MultiSignalHandler : public SignalHandlerBase<ErrorHandler_t>
{
public:
	typedef ErrorHandler_t		EH_t;
	typedef Signal_t			IPCSignal_t;
	typedef IPCSignal_t *		IPCSignal_ptr;
	typedef DWORD (*Handler_t) (LPVOID);

	MultiSignalHandler(ErrorHandler_t * pErrorHandler)
		: SignalHandlerBase<ErrorHandler_t>(pErrorHandler)
		, m_pErrorHandler(pErrorHandler)
		, m_dwTimeout(0){}

	//! Set the signal.
	/* @pSignal -- the pointer of the signal.
	 * @timeout_milliseconds -- the blocking time;
	 * @pHandler -- the callback function when the signal is set;
	 * @param -- additional parameter for the callback function.
	 * NOTE: this shall not start the thread. Remember to call @start() manually.
	*/
	void handle_signale(IPCSignal_ptr pSignal, DWORD timeout_milliseconds, Handler_t pHandler, LPVOID param)
	{
		m_oSignals.push_back(pSignal->get_native_handle());
		if(m_dwTimeout == 0) m_dwTimeout = timeout_milliseconds;
		m_dwTimeout = min(m_dwTimeout, timeout_milliseconds);
		m_oHandlers.push_back(pHandler);
		m_oParams.push_back(param);
	}
protected:
	virtual void thread_repeat_func()
	{
		DWORD ret = WaitForMultipleObjects(m_oSignals.size(), &(*m_oSignals.begin()), FALSE, m_dwTimeout);
		if(ret == WAIT_TIMEOUT){
			return ;
		}
		if(ret == WAIT_FAILED){
			m_pErrorHandler->handle_wait_failed(NULL);
			return ;
		}
		if(ret >= WAIT_ABANDONED_0 && ret <= WAIT_ABANDONED_0 + m_oSignals.size() -1)
		{
			m_pErrorHandler->handle_wait_abandoned(m_oSignals[ret - WAIT_ABANDONED_0]);
			return ;
		}
		if(ret >= WAIT_OBJECT_0  && ret <= WAIT_OBJECT_0  + m_oSignals.size() -1)
		{
			size_t i = ret - WAIT_OBJECT_0 ;
			if(m_oHandlers[i]){
				(*m_oHandlers[i])(m_oParams[i]);
			}
		}
	}

protected:
	std::vector<HANDLE> m_oSignals;
	DWORD	m_dwTimeout;
	ErrorHandler_t * m_pErrorHandler;
	std::vector<Handler_t> m_oHandlers;
	std::vector<LPVOID> m_oParams;
};//end class SignalHandler<IPCSignal, SingleSignalTag>


}//end namespace ff
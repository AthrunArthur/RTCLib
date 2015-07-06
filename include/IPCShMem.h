//Author Xuepeng Fan, xuepeng_fan@163.com 2015.05.24
#pragma once

#include <Windows.h>
#include "ErrorHandler.h"
namespace ff
{
	//!This is a wrapper for inter-process communication (IPC) shared memory.
	template<typename ErrorHandler_t=EDefaultErrorHandler>
	class IPCShMem
	{
	public:
		IPCShMem(ErrorHandler_t * pErrorHandler)
			: m_hShMem(NULL)
			, m_pErrorHandler(pErrorHandler)
			, m_pAddr(NULL)
			, m_SHSize(0){}

		~IPCShMem(){
			if(m_hShMem){
				UnmapViewOfFile(m_pAddr);
				CloseHandle(m_hShMem);
				m_hShMem = NULL;
				m_pAddr = NULL;
				m_SHSize = 0;
			}
		}

		//! Initailize a IPC shared memory with @name and @s.
		/* Note: will call ErrorHandler_t::handle_map_view_of_file_failed, 
		 * if fail to create the shared memory.
		*/
		void initialize(LPTSTR name, size_t s)
		{
			HANDLE h = NULL;
			while(h == NULL){
			h = OpenFileMapping(FILE_MAP_WRITE, false, name);
			if (h == NULL){
				h = CreateFileMapping(INVALID_HANDLE_VALUE, 
					NULL, PAGE_READWRITE, 0, s, name);
				}
			}
			m_hShMem = h;
			m_SHSize = s;
			m_pAddr = (char *)MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, s);
			if(m_pAddr == NULL)
			{
				m_pErrorHandler->handle_map_view_of_file_failed(h);
				return ;
			}
		}

		//! Return the address of the shared memory.
		char * addr() const {return m_pAddr;}

		//! Return the size of the shared memory.
		size_t size() const {return m_SHSize;}

	protected:
		HANDLE m_hShMem;
		ErrorHandler_t	* m_pErrorHandler;
		size_t m_SHSize;
		char * m_pAddr;
	}; //end class IPCShMem
}//end namespace ff
//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24

#pragma once
#include <Windows.h>

namespace ff{
	
	//! This is the default error handler for all OS level functions.
	//! We use error handler instead of defining different return values,
	/// so the users can place all their error handling code together.
	// One should define their own error handler with the same methods.
class EDefaultErrorHandler
{
public:
	EDefaultErrorHandler();
	void	handle_wait_timeout();

	//!For WaitForSingleObject and WaitForMultipleObjects with WAIT_FAILED.
	void	handle_wait_failed(HANDLE h);

	//!For WaitForSingleObject and WaitForMultipleObjects with WAIT_ABANDONED.
	void	handle_wait_abandoned(HANDLE h);

	//!For CreateThread failed.
	void	handle_create_thread_failed();

	//!For MapViewOfFile failed
	void	handle_map_view_of_file_failed(HANDLE h);
};//end class EDefaultErrorHandler
}//end namespace ff
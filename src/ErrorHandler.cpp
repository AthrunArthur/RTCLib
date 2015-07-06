//Author: Xuepeng Fan, xuepeng_fan@163.com, 2015.05.24

#include "ErrorHandler.h"
#include <cassert>

namespace ff{
	EDefaultErrorHandler::EDefaultErrorHandler(){}

	void EDefaultErrorHandler::handle_wait_timeout(){}

	void EDefaultErrorHandler::handle_wait_failed(HANDLE h){
		assert(false && "We have a wait failed!");
	}

	void EDefaultErrorHandler::handle_wait_abandoned(HANDLE h){
		assert(false && "We have a wait abandoned!");
	}

	void EDefaultErrorHandler::handle_create_thread_failed(){
		assert(false &&"CreateThread failed!");
	}
	
	void EDefaultErrorHandler::handle_map_view_of_file_failed(HANDLE h){
		assert(false && "MapViewOfFile failed!");
	}

}//end namespace ff
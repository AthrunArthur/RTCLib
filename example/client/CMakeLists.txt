add_definitions(-D_AFXDLL)
 set(CMAKE_MFC_FLAG 2)

 set(src DemoClient.aps
 	DemoClient.cpp
 	DemoClient.h
 	DemoClient.rc
 	DemoClientDlg.cpp
 	DemoClientDlg.h
 	resource.h
 	stdafx.h
 	stdafx.cpp
 	targetver.h)



 add_executable(DemoClient WIN32 ${src})
 target_link_libraries(DemoClient RTCLib)
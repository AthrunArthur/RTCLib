project(RTCLib)
cmake_minimum_required(VERSION 2.8)
message(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
message(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_definitions(-DPROFILE_DEMO_DATA_BUFFER)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(src include/DemoUse.h
  include/ErrorHandler.h
  include/IPCShMem.h
  include/IPCSignal.h
  include/SignalHandler.h
  include/Utility.h
  src/DemoUse.cpp
  src/ErrorHandler.cpp
  src/Utility.cpp)

add_library(RTCLib ${src})

add_subdirectory(example)
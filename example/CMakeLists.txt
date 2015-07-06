
add_executable(DemoServer server.cpp)
target_link_libraries(DemoServer RTCLib)

add_subdirectory(client)
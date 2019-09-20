cmake_minimum_required (VERSION 2.6)

project (Webserver CXX)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -g -std=c++11")
endif()

add_executable(main main.cc)
add_library(server Server.cc Channel.cc Epoll.cc EventLoop.cc EventLoopThread.cc EventLoopThreadPool.cc Socket.cc HttpParse.cc ./Log/Logging.cc ./Log/LogFile.cc ./Log/Fileutil.cc ./Log/AsyncLog.cc ./Log/AsyncStream.cc)
target_link_libraries(main server pthread)
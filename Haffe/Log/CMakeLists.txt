cmake_minimum_required(VERSION 2.7)
SET(CMAKE_C_COMPILER g++)

add_library(Log LogAppender.cpp LogAppender.h LogFormatter.cpp LogFormatter.h Logger.h Logger.cpp )
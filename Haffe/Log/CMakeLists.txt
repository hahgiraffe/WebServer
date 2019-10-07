cmake_minimum_required(VERSION 2.7)
SET(CMAKE_C_COMPILER g++)

add_library(Log LogAppender.cpp LogFormatter.cpp Logger.cpp )
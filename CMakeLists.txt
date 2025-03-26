cmake_minimum_required(VERSION 3.30)
project(CSE_340_Project_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(CSE_340_Project_2 main.cpp
        inputbuf.cpp
        inputbuf.h
        lexer.cpp
        lexer.h)

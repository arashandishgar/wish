cmake_minimum_required(VERSION 3.16)
project(wish C)

set(CMAKE_C_STANDARD 11)

add_executable(wish main.c Handler.c Handler.h StringArray.c StringArray.h)
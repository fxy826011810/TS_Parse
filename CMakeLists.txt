cmake_minimum_required(VERSION 3.19)
project(ts C)

set(CMAKE_C_STANDARD 99)

add_executable(ts main.c pat.c pat.h pmt.c pmt.h platform.h avpid.c avpid.h ts_header.c ts_header.h sdt.c sdt.h)
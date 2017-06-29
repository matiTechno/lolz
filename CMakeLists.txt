cmake_minimum_required(VERSION 2.6)
project(lolz)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++14 -g -O2")

set(SOURCE
    main.cpp
    main.hpp
    glad.c
    glad.h
    stb_truetype.h
    Inconsolata-Regular.h
    nonmvcp.hpp
    globjects.hpp
    stb_image.h
    globjects.cpp
    deleter.hpp
    gnu.h
    stb_image.c
    client.cpp
    client.hpp
    )

add_executable(lolz ${SOURCE})
target_link_libraries(lolz -lglfw -lpthread)

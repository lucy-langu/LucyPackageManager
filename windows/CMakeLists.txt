cmake_minimum_required(VERSION 3.15)
project(lpm VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lpm lpm.cpp)

if(MSVC)
    target_compile_options(lpm PRIVATE /W4 /O2)
else()
    target_compile_options(lpm PRIVATE -Wall -Wextra -O3)
endif()

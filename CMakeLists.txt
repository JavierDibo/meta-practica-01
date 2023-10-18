cmake_minimum_required(VERSION 3.25)
project(meta_practica_01)

set(CMAKE_CXX_STANDARD 17)

add_executable(meta_practica_01 main.cpp)

find_package(OpenMP)
if(OPENMP_FOUND)
    target_link_libraries(meta_practica_01 PUBLIC OpenMP::OpenMP_CXX)
endif()

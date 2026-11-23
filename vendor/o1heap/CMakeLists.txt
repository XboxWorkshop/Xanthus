cmake_minimum_required(VERSION 3.13)
project(o1heap LANGUAGES C)

add_library(o1heap STATIC ${CMAKE_CURRENT_SOURCE_DIR}/o1heap.c)
add_library(o1heap::o1heap ALIAS o1heap)
target_include_directories(o1heap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(o1heap PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE ON)

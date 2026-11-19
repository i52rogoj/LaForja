cmake_minimum_required(VERSION 3.16)
project(SG_CICT CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cict_lib src/GestorCICT.cc)
target_include_directories(cict_lib PUBLIC src)

add_executable(sg_cict src/main.cc)
target_link_libraries(sg_cict PRIVATE cict_lib)
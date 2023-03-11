cmake_minimum_required(VERSION 3.24)
project(log)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)

include_directories(${PROJECT_SOURCE_DIR}/lzma)
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE LZMA_SRC_FILES ./lzma/*.c)
add_library(lzma SHARED ${LZMA_SRC_FILES})



add_executable(log main.cpp service/LzmaUtil.cpp service/LogControl.cpp service/QGLog.cpp)

target_link_libraries(log lzma)

cmake_minimum_required (VERSION 3.8)

project (ThreadPromise)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. DIR_SRCS)

link_libraries(pthread dl)

add_executable(ThreadPromise ${DIR_SRCS})
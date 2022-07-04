cmake_minimum_required (VERSION 2.8)

project (ConDebug)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(. DIR_SRCS)

# INCLUDE         += -I${TARS_PATH}/include -I./ ${INC_DIR}
# LIB             += -L${TARS_PATH}/lib -ltarsservant -ltarsparse -ltarsutil -lpthread ${LIB_DIR}

# tars相关头文件
include_directories(/usr/local/tars/cpp/include)


#tars库文件
link_directories(/usr/local/tars/cpp/lib)
link_libraries(/usr/local/tars/cpp/lib/libtarsparse.a)
link_libraries(/usr/local/tars/cpp/lib/libtarsservant.a)
link_libraries(/usr/local/tars/cpp/lib/libtarsutil.a)

#多线程支持
link_libraries(pthread dl)

# 编译生成可执行文件
add_executable(ConDebug ${DIR_SRCS})
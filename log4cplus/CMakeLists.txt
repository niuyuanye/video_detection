cmake_minimum_required(VERSION 3.0)
project(demo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/./bin)  # 可执行文件的输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
include_directories(
    "/usr/local/include"
    "./libx64/log4cplus/include")
    
    

    link_directories(
        "/usr/local/lib"
        "./libx64/log4cplus/lib")

file(GLOB SRCC "./*.cpp")
ADD_LIBRARY(loglib SHARED ${SRCC} )
# cuda_add_library(myplugins SHARED ${PROJECT_SOURCE_DIR}/yololayer.cu)
target_link_libraries(loglib log4cplus)
# target_link_libraries(loglib log4cplus)


# file(GLOB SRC "src/*.cpp" "src/*.c")
# file(GLOB SRC "src/*.c")

# add_executable(demo ${SRC} )
# target_link_libraries(demo pthread)
# target_link_libraries(demo ftp++)
# target_link_libraries(demo log4cplus)
# target_link_libraries(demo ini)


# set(CMAKE_BUILD_TYPE Debug)
#设置编译模式
# set(CMAKE_BUILD_TYPE "Release" )

#生成文件到指定位置
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#install 安装文件到指定位置
# install(TARGETS hello_shared LIBRARY DESTINATION /usr/local/lib)
# install(FILES ${PROJECT_SOURCE_DIR}/include/libHelloSLAM.h DESTINATION /usr/local/include)


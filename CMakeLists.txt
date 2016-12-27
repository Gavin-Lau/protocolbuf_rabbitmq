cmake_minimum_required(VERSION 2.8)

project(ProtRmq)

# output dir
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# system check
if (WIN32)
    message("current platform is windows!")
    add_definitions(-DDLLEXPORT)
    set(OS_DIR Windows)
    set(Lib_suffix ".lib")
elseif(UNIX)
    message("current platform is UNIX like!")
    add_definitions(-D_LINUX)
    set(OS_DIR  Linux)
    set(Lib_suffix ".a")
else()
    message(FATAL_ERROR "current platform is unknown!")
endif()

#platform check
if (PLATFORM STREQUAL x64)
    message("Will generate X64 platform app/lib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
    set(Platform_DIR x64)
elseif(PLATFORM STREQUAL x32)
    message("Will generate X32 platform app/lib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(Platform_DIR x86)
else()
    message(FATAL_ERROR "Wrong Platform param!")
endif ()

#buil type check
if (BUILD_TYPE STREQUAL release)
    message("Will build Release app/lib")
    set(CMAKE_BUILD_TYPE Release)
    set(Lib_type )
elseif(BUILD_TYPE STREQUAL debug)
    message("Will build Debug app/lib")
    set(CMAKE_BUILD_TYPE Debug)
    set(Lib_type d)
else()
    message(FATAL_ERROR "Wrong build type param!")
endif ()

# previous defines
set(Src_DIR src)
set(Test_DIR test)
set(Third_DIR ${PROJECT_SOURCE_DIR}/../third)
set(Protobuf_Lib_DIR ${Third_DIR}/protobuf/lib/${OS_DIR}/${Platform_DIR})
set(Rabbitmq_Lib_DIR ${Third_DIR}/rabbitmq/lib/${OS_DIR}/${Platform_DIR})
#include dir
include_directories(
    include
    ${Src_DIR}
    ${Third_DIR}/protobuf/include
    ${Third_DIR}/rabbitmq/include
)

link_directories(
    ${Third_DIR}
    ${Third_DIR}/rabbitmq/lib/${OS_DIR}/${Platform_DIR}
)

#complier check
message("libs dir debug:${Third_DIR}/protobuf/lib/${OS_DIR}/${Platform_DIR} ${Third_DIR}/rabbitmq/lib/${OS_DIR}/${Platform_DIR}")
message("libs prarams debug: libprotobuf${Lib_type}${Lib_suffix} librabbitmq${Lib_type}.4${Lib_suffix}")

set(Lib_List
    ${Protobuf_Lib_DIR}/libprotobuf${Lib_type}${Lib_suffix}
    ${Rabbitmq_Lib_DIR}/librabbitmq${Lib_type}.4${Lib_suffix}
)

set(Server_FILES
    ${Src_DIR}/rabbitmq.cpp
    ${Src_DIR}/stulist.pb.cc
    ${Test_DIR}/server.cpp
)

set(Clent_FILES
    ${Src_DIR}/rabbitmq.cpp
    ${Src_DIR}/stulist.pb.cc
    ${Test_DIR}/client.cpp
)

add_executable(Server ${Server_FILES})
add_executable(Client ${Clent_FILES})

target_link_libraries(Server PUBLIC ${Lib_List})
target_link_libraries(Client PUBLIC ${Lib_List})

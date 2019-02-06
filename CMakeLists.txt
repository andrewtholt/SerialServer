cmake_minimum_required(VERSION 2.8.9)

project (SerialServer)
include_directories(.)

if(ARM)
    message("Setup for ARM")
    set(tools /home/andrew_h/Source/SMIB/buildroot/SMEG_nit6/host/usr)
    set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
else()
    message("Bye")
endif()

unset(HELLO CACHE)

find_library(THREADS pthread )
find_package( Threads )
find_library(REDIS redis-dev)

add_executable(serialServer serialComms.cpp serialServer.cpp globals.cpp strsave.cpp)
add_executable(clientTest client.cpp clientTest.cpp serialComms.cpp)
add_executable(creatorTest client.cpp creatorTest.cpp serialComms.cpp)

# target_link_libraries(serialServer ${THREADS})
# target_link_libraries(clientTest ${THREADS})
# target_link_libraries(creatorTest ${THREADS})
target_link_libraries(creatorTest)

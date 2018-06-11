#
# Created by M. Swianiewicz
#

cmake_minimum_required(VERSION 2.9)
set(CMAKE_CXX_STANDARD 11)
project(rosa_config)
set(SOURCES "src/main.cpp")
include_directories("../common/include")
set(DEFAULT_FLAGS "-Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_FLAGS} -O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEFAULT_FLAGS} -O3")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../common/build/rosa_common.so)

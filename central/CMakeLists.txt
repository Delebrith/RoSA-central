#
# Created by T. Nowak
#

cmake_minimum_required(VERSION 2.9)
set(CMAKE_CXX_STANDARD 11)
project(rosa_central)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem thread)
find_package (Threads)

FILE(GLOB_RECURSE SOURCES "src/*")
FILE(GLOB_RECURSE HEADERS "include/*")
include_directories("include" "../common/include")

set(DEFAULT_FLAGS "-Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEFAULT_FLAGS} -O3 -DNDEBUG -Werror")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../common/build/rosa_common.so ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

#
# Created by T. Nowak
#

cmake_minimum_required(VERSION 2.9)
set(CMAKE_CXX_STANDARD 11)
project(rosa_common)
FILE(GLOB_RECURSE SRC_FILES "src/*")
FILE(GLOB_RECURSE INCLUDE_FILES "include/*")
include_directories("include")
set(DEFAULT_FLAGS "-Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEFAULT_FLAGS} -O3 -DNDEBUG -Werror")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${INCLUDE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "") # no default 'lib' before library name
target_link_libraries(${PROJECT_NAME} "-lpthread")

cmake_minimum_required(VERSION 2.9)
set(CMAKE_CXX_STANDARD 11)
project(rosa_central)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system thread unit_test_framework)
set(Casablanca_LIBRARIES "-lboost_system -lboost_filesystem -lcrypto -lssl -lcpprest -lpthread")

enable_testing()

FILE(GLOB_RECURSE SOURCES "src/*")
FILE(GLOB_RECURSE TEST_SOURCES "test/*")
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp) # main.cpp is excluded from sources, so that it will not be linked to tests
FILE(GLOB_RECURSE HEADERS "include/*")
include_directories("include" "../common/include")

set(DEFAULT_FLAGS "-Wall -pedantic -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEFAULT_FLAGS} -O3 -DNDEBUG -Werror")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} src/main.cpp)
add_executable(${PROJECT_NAME}_tests ${SOURCES} ${HEADERS} ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../common/build/rosa_common.so ${Boost_LIBRARIES} ${Casablanca_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_tests ${CMAKE_CURRENT_SOURCE_DIR}/../common/build/rosa_common.so ${Boost_LIBRARIES} ${Casablanca_LIBRARIES})

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

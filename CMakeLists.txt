cmake_minimum_required(VERSION 3.20)
project(grokcpp)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  # add_compile_options(-fno-elide-constructors)
  set(GROKCPP_DEBUG 1)
else(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(GROKCPP_DEBUG 0)
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

configure_file("${PROJECT_SOURCE_DIR}/grokcpp/config.h.in"
               "${PROJECT_BINARY_DIR}/grokcpp/config.h")

file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/grokcpp/*cpp)
set(SRC ${SRC_LIST})

add_library(grokcpp ${SRC})

find_package(
  Boost 1.40
  COMPONENTS regex
  REQUIRED)

target_include_directories(grokcpp PUBLIC ${PROJECT_SOURCE_DIR}/grokcpp)
target_include_directories(grokcpp PUBLIC ${PROJECT_BINARY_DIR}/grokcpp)
target_include_directories(grokcpp PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(grokcpp PRIVATE ${Boost_LIBRARIES})
target_compile_options(grokcpp PRIVATE -Wall)

option(GROKCPP_BUILD_TESTS "Build tests for grokcpp" ON)
if (${GROKCPP_BUILD_TESTS})
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()

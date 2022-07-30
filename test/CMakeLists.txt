include_directories(${PROJECT_SOURCE_DIR}/grokcpp)
link_libraries(grokcpp)

find_package(GTest CONFIG REQUIRED)

file(GLOB test_files ${CMAKE_CURRENT_LIST_DIR}/ test_*cpp)
foreach(one_test_file ${test_files})
  cmake_path(GET one_test_file FILENAME one_file_name)
  cmake_path(REMOVE_EXTENSION one_file_name LAST_ONLY OUTPUT_VARIABLE exec_name)
  add_executable(${exec_name} ${one_test_file})
  target_link_libraries(
    ${exec_name} PRIVATE GTest::gmock GTest::gtest GTest::gmock_main
                         GTest::gtest_main)

  add_test(NAME ${exec_name} COMMAND $<TARGET_FILE:${exec_name}>)
endforeach(one_test_file ${test_files})

cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

file(GLOB_RECURSE STRONK_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*.cpp")

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

# "make check-tests"
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# "make X_test"
foreach(stronk_test_source ${STRONK_TEST_SOURCES})
    get_filename_component(stronk_test_filename ${stronk_test_source} NAME)
    string(REPLACE ".cpp" "" stronk_test_name ${stronk_test_filename})

    add_executable(${stronk_test_name} EXCLUDE_FROM_ALL ${stronk_test_source})
    add_dependencies(build-tests ${stronk_test_name})
    add_dependencies(check-tests ${stronk_test_name})

    gtest_discover_tests(${stronk_test_name}
        EXTRA_ARGS
        --gtest_color=auto
        --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${stronk_test_name}.xml
        --gtest_catch_exceptions=0
        DISCOVERY_TIMEOUT 120
        PROPERTIES
        TIMEOUT 120
        )

    target_link_libraries(${stronk_test_name} stronk gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${stronk_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
        COMMAND ${stronk_test_name}
        )

endforeach()



include(GoogleTest)

include_directories(${CMAKE_SOURCE_DIR}/include/backend)
include_directories(${CMAKE_SOURCE_DIR}/include/frontend)
include_directories(${CMAKE_SOURCE_DIR}/include/common)

add_executable(TestExecutable 
    frontend/parser_tests.cpp 
    frontend/scanner_tests.cpp 
    main.cpp
)
target_link_libraries(TestExecutable PRIVATE stronk_backend stronk_common stronk_frontend gtest gmock_main)

enable_testing()
add_test(NAME TestExecutable COMMAND TestExecutable)
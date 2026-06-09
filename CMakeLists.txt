cmake_minimum_required(VERSION 3.22)

project(ConsoleCalc VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(PostgreSQL REQUIRED)
find_program(VALGRIND valgrind)

add_library(calc STATIC
            src/application.cpp
            src/calculator.cpp
            src/cacher.cpp
            src/libpqwrapper.cpp
)

add_executable(consolecalc 
               src/main.cpp
               src/logger.cpp
)

add_executable(consolecalc_tests
               src/tests_main.cpp
               src/tests.cpp
               src/logger.cpp
)

include(FetchContent)

FetchContent_Declare(
    integermath
    GIT_REPOSITORY https://github.com/AnastasiaVeselovskaya/CPP-IntergerMath.git
    GIT_TAG main
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.3
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(integermath
                           json
                           googletest
                           spdlog
)

target_include_directories(calc PUBLIC
    ${PostgreSQL_INCLUDE_DIR}
    ${integermath_SOURCE_DIR}/include
    ${json_SOURCE_DIR}/include
    ${spdlog_SOURCE_DIR}/include
)

include(GNUInstallDirs)

install(TARGETS consolecalc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(USE_CLANG_FORMAT_CHECK)
    find_program(CLANG_FORMAT_PATH
        NAMES
        clang-format-21
        REQUIRED)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_PATH}")

    file(GLOB_RECURSE CLANG_FORMAT_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
        src/*.hpp
    )

add_custom_target(clang-format-fix
    COMMAND
    ${CLANG_FORMAT_PATH} -style=file -i ${CLANG_FORMAT_SRC}
    WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
    COMMENT
    "Formatting consolecalc with ${CLANG_FORMAT_PATH} ..."
)

add_custom_target(clang-format
    COMMAND
    run-clang-format.py --clang-format-executable=${CLANG_FORMAT_PATH} --style=file ${CLANG_FORMAT_SRC}
    WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
    COMMENT
    "Checking format consolecalc with ${CLANG_FORMAT_PATH} ..."
)
add_dependencies(consolecalc clang-format)
endif()

if(USE_CLANG_FORMAT_FIX)
    find_program(CLANG_FORMAT_PATH
        NAMES
        clang-format-21
        REQUIRED)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_PATH}")

    file(GLOB_RECURSE CLANG_FORMAT_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
        src/*.hpp
    )

add_custom_target(clang-format-fix
    COMMAND
    ${CLANG_FORMAT_PATH} -style=file -i ${CLANG_FORMAT_SRC}
    WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
    COMMENT
    "Formatting consolecalc with ${CLANG_FORMAT_PATH} ..."
)

add_dependencies(consolecalc clang-format-fix)
endif()

if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY_PATH
        NAMES
        clang-tidy-21
        REQUIRED)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_PATH}")
    set(CLANG_TIDY_COMMAND ${CLANG_TIDY_PATH} "-config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")
    set_target_properties(consolecalc PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

target_link_libraries(calc PRIVATE    
    PostgreSQL::PostgreSQL
    integermath
    nlohmann_json::nlohmann_json    
    spdlog::spdlog
)

target_link_libraries(consolecalc PRIVATE
    calc
    $<$<BOOL:${MINGW}>:ws2_32>
)

target_link_libraries(consolecalc_tests PRIVATE
    calc
    GTest::gtest
    GTest::gtest_main
)

if(VALGRIND)
    add_custom_target(test_memcheck
        COMMAND ${VALGRIND}
            --leak-check=full
            --error-exitcode=1
            --show-leak-kinds=all
            --track-origins=yes
            $<TARGET_FILE:consolecalc_tests>
        DEPENDS consolecalc_tests
    )
endif()

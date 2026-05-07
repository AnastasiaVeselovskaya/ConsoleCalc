cmake_minimum_required(VERSION 3.22)

project(ConsoleCalc VERSION 1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(consolecalc src/main.cpp)

include(FetchContent)

FetchContent_Declare(
    integermath
    GIT_REPOSITORY https://github.com/AnastasiaVeselovskaya/CPP-IntergerMath.git
    GIT_TAG main
)

FetchContent_MakeAvailable(integermath)

target_link_libraries(consolecalc PRIVATE integermath)

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

# Check whether the system supports getopt_long
# and define macros GETOPT_LONG_SUPPORTED based on that info
include(CheckSymbolExists)
check_symbol_exists(getopt_long "getopt.h" GETOPT_LONG_SUPPORTED)
target_compile_definitions(consolecalc PRIVATE GETOPT_LONG_SUPPORTED=$<BOOL:${GETOPT_LONG_SUPPORTED}>)

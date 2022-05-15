cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 11)

project(ide VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")     # -Werror

# ================== Library files ==================
# nlohman-json library files
AUX_SOURCE_DIRECTORY(lib/nlohman-json JSON_LIB_FILES)

# path library files
AUX_SOURCE_DIRECTORY(lib/path PATH_LIB_FILES)

# id library files
AUX_SOURCE_DIRECTORY(lib/id ID_LIB_FILES)

# doctest library files
AUX_SOURCE_DIRECTORY(lib/doctest DOCTEST_LIB_FILES)

# ================== Source files ===================
# object source files
AUX_SOURCE_DIRECTORY(src/automata OBJECT_SOURCE_FILES)

# test source files
file(GLOB_RECURSE TEST_SOURCE_FILES tests/**/*.cc)

set(RELEASE_SOURCE_FILES
    src/ide.cc
    src/other/python_to_enfa.cpp
    src/other/python_to_enfa.h
    ${JSON_LIB_FILES}
    ${PATH_LIB_FILES}
    ${ID_LIB_FILES}
    ${OBJECT_SOURCE_FILES}
)

set(TEST_SOURCE_FILES
    src/ideTest.cc
    src/other/python_to_enfa.h
    src/other/python_to_enfa.cpp
    tests/otherTests/python_to_enfaTest.cc
    ${DOCTEST_LIB_FILES}
    ${JSON_LIB_FILES}
    ${PATH_LIB_FILES}
    ${OBJECT_SOURCE_FILES}
    ${TEST_SOURCE_FILES}
)

add_executable(ide ${RELEASE_SOURCE_FILES})
add_executable(ide_test ${TEST_SOURCE_FILES})

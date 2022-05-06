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

# ================== Source files ===================
AUX_SOURCE_DIRECTORY(src/objects OBJECT_SOURCE_FILES)

set(RELEASE_FILES
    src/ide.cc
    ${JSON_LIB_FILES}
    ${PATH_LIB_FILES}
    ${ID_LIB_FILES}
    ${OBJECT_SOURCE_FILES}
)

add_executable(ide ${RELEASE_FILES})

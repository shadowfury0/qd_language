cmake_minimum_required(VERSION 3.0)

project(qd_language)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE "Debug")

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
# aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
file(GLOB_RECURSE SRC_FILE "src/*.cpp" "src/*.c")

# set(LIB_DIR  ${PROJECT_SOURCE_DIR}/lib)
# set(LIB_NAME clc)

# list(REMOVE_ITEM MAIN ${PROJECT_SOURCE_DIR}/src/main.cpp)

# add_library(clc STATIC ${SRC_LIST})
# add_library(clac SHARED ${SRC_LIST})

add_executable(qd ${PROJECT_SOURCE_DIR}/src/main.cpp ${SRC_FILE})

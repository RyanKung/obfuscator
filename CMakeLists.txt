cmake_minimum_required(VERSION 3.17)
project("Obfucator")
set(ARCH "x86_64")

if (NOT PATH_TO_LLVM)
  find_package(LLVM 11.0.0 REQUIRED CONFIG)
else()
  set(
    SEARCH_PATHS
    ${PATH_TO_LLVM}
    ${PATH_TO_LLVM}/lib/cmake
    ${PATH_TO_LLVM}/lib/cmake/llvm
    ${PATH_TO_LLVM}/lib/cmake/clang
    ${PATH_TO_LLVM}/share/clang/cmake/
    ${PATH_TO_LLVM}/share/llvm/cmake/
    )
  find_package(
    LLVM 11.0.0
    REQUIRED
    CONFIG
    NO_DEFAULT_PATH
    PATHS ${SEARCH_PATHS}
    )
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})

add_definitions(-DFUNCPASS=true)
add_definitions(-DSPLITPASS=true)
add_definitions(-DSTRINGPASS=false)
add_definitions(-DBOGUSPASS=false)
add_definitions(-DFLATPASS=false)



add_definitions(-DDEBUG=false)

include_directories(${LLVM_INCLUDE_DIRS})
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

# CMake Modules used for llvm specific macros
include_directories(include/)
add_subdirectory(Obfuscation)

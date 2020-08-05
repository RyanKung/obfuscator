cmake_minimum_required(VERSION 3.17)
set(ARCH "x86_64")
set(CMAKE_CXX_STANDARD 14)
aux_source_directory(./src SRC_LIST)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PATH_TO_LLVM "/Users/ryan/lab/llvm-project/build")
set (search_paths
  ${PATH_TO_LLVM}
  ${PATH_TO_LLVM}/lib/cmake
  ${PATH_TO_LLVM}/lib/cmake/llvm
  ${PATH_TO_LLVM}/lib/cmake/clang
  ${PATH_TO_LLVM}/share/clang/cmake/
  ${PATH_TO_LLVM}/share/llvm/cmake/
)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(include/)
add_subdirectory(Obfuscation)

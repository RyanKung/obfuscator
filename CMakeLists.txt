find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
add_subdirectory(Obfuscation)

add_llvm_library(LLVMObfuscation MODULE
  Substitution.cpp
  BogusControlFlow.cpp
  Utils.cpp
  SplitBasicBlocks.cpp
  Flattening.cpp
  StringObfuscation.cpp
  CryptoUtils.cpp
  )
add_dependencies(LLVMObfuscation intrinsics_gen)

include(LLVM-Config)
include(HandleLLVMOptions)
include(AddLLVM)

add_llvm_library(LLVMObfuscation MODULE
  Substitution.cpp
  BogusControlFlow.cpp
  Utils.cpp
  SplitBasicBlocks.cpp
  Flattening.cpp
  StringObfuscation.cpp
  CryptoUtils.cpp
  FuncNameObf.cpp
  )
add_dependencies(LLVMObfuscation intrinsics_gen)

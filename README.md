LLVM Obfuscator Pass
===========

# TL;DR

LLVM obfuscator pass is depends on ieeespro2015-JunodRWM ( [LLVM-4.0](https://github.com/obfuscator-llvm/obfuscator/tree/llvm-4.0)), and spelle's work [LLVM-9.0.0](https://github.com/spelle/obfuscator/tree/llvm-9.0.0).

The difference between this impl and the origin is, LLVMObfusPass is a autoload plugin, and not require origin LLVM codes for compiling. For now, it support llvm 11.


It contains passes:

* Flattening(Origin): flattening, Call graph flattening (unstable);

* BogusControlFlow(Origin): boguscf, inserting bogus control flow (unstable);

* SplitBasicBlock(Origin): splitbbl, BasicBlock splitting;

* StringObfuscationPass(Origin): GVDiv, Global variable (i.e., const char*) diversification pass(unstable);

* FunctionNameObfPass(New Feature): func_name, Rename Function Name Randomly"


Please have a look at the [wiki](https://github.com/obfuscator-llvm/obfuscator/wiki)!

Current (official) version: [LLVM-4.0](https://github.com/obfuscator-llvm/obfuscator/tree/llvm-4.0)

Current version on spelle's repo is [LLVM-9.0.0](https://github.com/spelle/obfuscator/tree/llvm-9.0.0)

You can cite Obfuscator-LLVM using the following Bibtex entry:



```
@INPROCEEDINGS{ieeespro2015-JunodRWM,
  author={Pascal Junod and Julien Rinaldini and Johan Wehrli and Julie Michielin},
  booktitle={Proceedings of the {IEEE/ACM} 1st International Workshop on Software Protection, {SPRO'15}, Firenze, Italy, May 19th, 2015},
  editor = {Brecht Wyseur},
  publisher = {IEEE},
  title={Obfuscator-{LLVM} -- Software Protection for the Masses},
  year={2015},
  pages={3--9},
  doi={10.1109/SPRO.2015.10},
}
```

# How to Build :

Ref:

http://llvm.org/docs/CMake.html#developing-llvm-passes-out-of-source
https://llvm.org/docs/WritingAnLLVMPass.html#registering-dynamically-loaded-passes

simplify:

```
mkdir build
cd build
cmake .. & make
```

# How to use :

## Automatically apply with `clang`

This version of o-llvm pass support autoload: (impl details: https://github.com/rdadolf/clangtool), so just:

```
clang -Xclang -load -Xclang <path>/LLVMObfuscation.dylib  -fobjc-arc ./<source>.m
```

It will automatically apply all passes.

## With `opt`

Just try
```
opt -load <path>/LLVMObfuscation.dylib --help
```

You will find avaliable passes list there such as `func_name` and `splitbbl`


## Example

```
clang -Xclang -load -Xclang \
	../../build/Obfuscation/LLVMObfuscation.dylib  \
	-fobjc-arc ./main.m
Renaming Function: -[Box init]
Skipping External Function: objc_msgSendSuper2
Skipping External Function: llvm.objc.storeStrong
Skipping External Function: llvm.objc.retain
Renaming Function: -[Box volume]
Renaming Function: -[Box height]
Renaming Function: -[Box setHeight:]
Skipping main
Skipping External Function: llvm.objc.autoreleasePoolPush
Skipping External Function: NSLog
Skipping External Function: objc_alloc_init
Skipping External Function: objc_msgSend
Skipping External Function: llvm.objc.autoreleasePoolPop
Renaming Function: .datadiv_decode6839268405975385304
Renaming Struct: struct._class_t
Renaming Struct: struct._objc_cache
Renaming Struct: struct._class_ro_t
Renaming Struct: struct.__method_list_t
Renaming Struct: struct._objc_method
Renaming Struct: struct._objc_protocol_list
Renaming Struct: struct._protocol_t
Renaming Struct: struct._ivar_list_t
Renaming Struct: struct._ivar_t
Renaming Struct: struct._prop_list_t
Renaming Struct: struct._prop_t
Renaming Struct: struct.__NSConstantString_tag
Renaming Struct: struct._objc_super
```

# How to Debug

1. If you got segmentfalut or other wired errors, it may caused by LLVM Pass sequence.

Check the document: https://llvm.org/doxygen/classllvm_1_1PassManagerBuilder.html#a575d14758794b0997be4f8edcef7dc91

2. Debug with LLVM IR:

Turn on debug flag on `cmake` file, and try command like this

```
	${CC} -Xclang -load -Xclang \
	../../build/Obfuscation/LLVMObfuscation.dylib  \
	./main.m -S -o /dev/stdout
```

You can use toolset like `diff` to find out how the pass modified origin IR

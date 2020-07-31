# Warning: This repo is UNSTABLE


# TL;DR

LLVM obfuscator pass is depends on ieeespro2015-JunodRWM ( [LLVM-4.0](https://github.com/obfuscator-llvm/obfuscator/tree/llvm-4.0)), and spelle's work [LLVM-9.0.0](https://github.com/spelle/obfuscator/tree/llvm-9.0.0).

The difference between this impl and the origin is, LLVMObfusPass is a autoload plugin, and not require origin LLVM codes for compiling. For now, it support llvm 11.


It contains passes:

* Flattening: flattening, Call graph flattening (unstable);

* BogusControlFlow: boguscf, inserting bogus control flow;

* SplitBasicBlock: splitbbl, BasicBlock splitting;

* StringObfuscationPass GVDiv, Global variable (i.e., const char*) diversification pass;


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

This version of o-llvm pass support autoload: (impl details: https://github.com/rdadolf/clangtool), so just:

```
clang -Xclang -load -Xclang <path>/build/Obfuscation/LLVMObfuscation.dylib  -fobjc-arc ./<source>.m
```

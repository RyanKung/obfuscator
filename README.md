# Warning: This repo is UNSTABLE


# TL;DR

This repo is withdrawed from the offical version  [LLVM-4.0](https://github.com/obfuscator-llvm/obfuscator/tree/llvm-4.0), and spelle's [LLVM-9.0.0](https://github.com/spelle/obfuscator/tree/llvm-9.0.0),
but removed all llvm codes for supporting build outside llvm codetree.

Please have a look at the [wiki](https://github.com/obfuscator-llvm/obfuscator/wiki)!

Current (official) version: [LLVM-4.0](https://github.com/obfuscator-llvm/obfuscator/tree/llvm-4.0)

Current version on this repo is [LLVM-9.0.0](https://github.com/spelle/obfuscator/tree/llvm-9.0.0)

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

# How to use :

This version of o-llvm pass support autoload: (impl details: https://github.com/rdadolf/clangtool), so just:

```
 clang -Xclang -load -Xclang <custom-pass>.so ..
```



When you are done developing your pass, you may wish to integrate it into the LLVM source tree. You can achieve it in two easy steps:

*    Copying <pass name> folder into <LLVM root>/lib/Transform directory.
*    Adding add_subdirectory(<pass name>) line into <LLVM root>/lib/Transform/CMakeLists.txt.


### Special case for macOS & Xcode

From : https://afnan.io/2018-10-01/using-the-latest-llvm-release-on-macos/

```
cmake -G Ninja \
  -DLLDB_CODESIGN_IDENTITY='' \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DLLVM_CREATE_XCODE_TOOLCHAIN=On \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  ../obfuscator-llvm-9.0.0.src
ninja -j5
```

I also recommend using `Ninja` rather than  `make` to build LLVM, because it will build significantly faster.

Now that you have the XCode toolchain, you can place it in the Toolchains directory in XCode.

```
sudo ninja install-xcode-toolchain
```

The toolchain is generated and installed in `/usr/local/Toolchains/LLVM9.0.0.xctoolchain`

You need to instruct XCode to actually use the toolchain. You can do so in two ways: from your environment variables, and through the XCode app itself.

To set it through an environment variable:

```
export TOOLCHAINS="LLVM9.0.0"
```

In Xcode.app, you can select `Xcode -> Toolchains -> org.llvm.9.0.0` in the menu.

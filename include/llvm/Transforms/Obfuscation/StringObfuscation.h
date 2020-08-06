#ifndef _STROBF_H_
#define _STROBF_H_
#include <string>
#include <strstream>
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Alignment.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Obfuscation/CryptoUtils.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Config/llvm-config.h"
#if defined(LLVM_VERSION_MAJOR) && LLVM_VERSION_MAJOR > 10
#include "llvm/IR/AbstractCallSite.h"
#else
#include "llvm/IR/CallSite.h"
#endif

namespace llvm {
    Pass* createStringObfuscation(bool flag);
}
#endif

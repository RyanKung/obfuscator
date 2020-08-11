/* Ref: https://github.com/HikariObfuscator/Core/blob/9647935d0958ac2bac3e66e20194434ebc18d9f6/Utils.cpp */

#ifndef __UTILS_OBF__
#define __UTILS_OBF__

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Local.h" // For DemoteRegToStack and DemotePHIToStack
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/NoFolder.h"
#include <stdio.h>

using namespace llvm;

void fixStack(Function *f);
std::string readAnnotate(Function *f);
bool toObfuscate(bool flag, Function *f, std::string attribute);
void FixFunctionConstantExpr(Function *Func);
void FixBasicBlockConstantExpr(BasicBlock *BB);
#endif

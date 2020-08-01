#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/LegacyPassManager.h"


// ref: http://mayuyu.io/2017/06/01/LLVMHacking-0x1/

using namespace llvm;
using namespace std;

static string obfcharacters="qwertyuiopasdfghjklzxcvbnm1234567890";

namespace llvm {
  struct FuncNameObfPass : public ModulePass {
    static char ID;
    FuncNameObfPass() : ModulePass(ID) {}

    string randomString(int length){
      string name;
      name.resize(length);
      for(int i=0;i<length;i++){
        name[i]=obfcharacters[rand()%(obfcharacters.length()+1)];
      }
      return name;
    }

    bool runOnModule(Module &M) override {
      for(Module::iterator Fun=M.begin();Fun!=M.end();Fun++){
	Function &F=*Fun;
	if (F.getName().str().compare("main")==0){
	  errs()<<"Skipping main\n";
	}
	else if(F.empty()==false){
	  //Rename
	  errs()<<"Renaming Function: "<<F.getName()<<"\n";
	  F.setName(randomString(16));
	}
	else{
	  errs()<<"Skipping External Function: "<<F.getName()<<"\n";
	}
      }
      return true;
    }
  };
}

char FuncNameObfPass::ID = 0;
// ref: https://github.com/rdadolf/clangtool/blob/master/clangtool.cpp

static void loadPass(const PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) {
  PM.add(new FuncNameObfPass());
}

static RegisterPass<FuncNameObfPass> A("func_name", "Rename Function Name Randomly");
static RegisterStandardPasses B(PassManagerBuilder::EP_EarlyAsPossible, loadPass);
static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_OptimizerLast, loadPass);
static RegisterStandardPasses D(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0, loadPass);

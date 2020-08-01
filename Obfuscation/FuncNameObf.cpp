#include "llvm/Transforms/Obfuscation/FuncNameObf.h"



// ref: http://mayuyu.io/2017/06/01/LLVMHacking-0x1/
// ref: https://llvm.org/doxygen/MetaRenamer_8cpp_source.html

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
      for (auto AI = M.alias_begin(), AE = M.alias_end(); AI != AE; ++AI) {
	errs()<<"Skipping Alias:"<<AI->getName()<<"\n";
      }

      for(auto &F: M){
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
      // Rename all struct types
      TypeFinder StructTypes;
      StructTypes.run(M, true);

      for (StructType *STy : StructTypes) {
	if (STy->isLiteral() || STy->getName().empty()) {
	  errs()<<"Skipping External Struct: "<<STy->getName()<<"\n";
	} else {
	  errs()<<"Renaming Struct: "<<STy->getName()<<"\n";
	  STy->setName(randomString(16));
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

static RegisterPass<FuncNameObfPass> A("func_name", "Rename Function&Struct Name Randomly", false, false);
static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_OptimizerLast, loadPass);
static RegisterStandardPasses D(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0, loadPass);

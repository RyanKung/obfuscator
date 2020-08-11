#include "llvm/Transforms/Obfuscation/FuncNameObf.h"
#include "llvm/Transforms/Obfuscation/macros.h"

#define DEBUG_TYPE "funcobf"

// ref: http://mayuyu.io/2017/06/01/LLVMHacking-0x1/
// ref: https://llvm.org/doxygen/MetaRenamer_8cpp_source.html
// ref: http://mayuyu.io/2017/06/02/LLVMHacking-0x2/

using namespace llvm;
using namespace std;

static string obfcharacters="qwertyuiopasdfghjklzxcvbnm1234567890";

namespace llvm {
  struct FuncNameObfPass : public ModulePass {
    static char ID;
    bool is_flag = true;
    FuncNameObfPass() : ModulePass(ID) {}
    FuncNameObfPass(bool flag):
      ModulePass(ID) {
      is_flag = flag;
    }

    string randomString(int length){
      string name;
      name.resize(length);
      for(int i=0;i<length;i++){
        name[i]=obfcharacters[rand()%(obfcharacters.length()+1)];
      }
      return name;
    }

    bool runOnModule(Module &M) override {
      if (!is_flag)
	return false;
      string _name;

      for (auto AI = M.alias_begin(), AE = M.alias_end(); AI != AE; ++AI) {
	DEBUG_OUT("Skipping Alias:"<<AI->getName());
      }

      for(auto &F: M){
	StringRef Name = F.getName();
	LibFunc Tmp;
	if ((!Name.empty() && Name[0] == 1) ||
	    getAnalysis<TargetLibraryInfoWrapperPass>().getTLI(F).getLibFunc(F, Tmp)) {
	  DEBUG_OUT("Skipping External Function: "<<F.getName());
	  continue;
	}

	if (Name != "main"){
	  _name = randomString(16);
	  DEBUG_OUT("Renaming Function: "<<F.getName()<<" to: "<<_name);
	  //	  F.setName(_name);
	} else {
	  DEBUG_OUT("Skipping main");
	  continue;
	}
      }
      // Rename all struct types
      TypeFinder StructTypes;
      StructTypes.run(M, true);

      // for(auto G=M.global_begin(); G!=M.global_end(); G++) {
      //   GlobalVariable &GV=*G;
      // 	DEBUG_OUT(GV.getName());
      //   if (GV.getName().str().find("OBJC_CLASSLIST_REFERENCES") == 0) {
      // 	  if(GV.hasInitializer()) {
      // 	    _name = randomString(16);
      // 	    GV.getInitializer()->setName(randomString(16));
      // 	    DEBUG_OUT("Renaming Struct: "<<GV.getInitializer()->getName()<<" to: "<<_name);
      // 	  }
      //   }
      // }
      return true;
    }
  };
}

char FuncNameObfPass::ID = 0;
static void loadPass(const PassManagerBuilder &Builder, llvm::legacy::PassManagerBase &PM) {
#ifdef FUNCPASS
  PM.add(new FuncNameObfPass(FUNCPASS));
#else
   PM.add(new FuncNameObfPass(false));
#endif
}
 ModulePass *llvm::createFuncNameObfPass() {
   return new FuncNameObfPass();
 }
static RegisterPass<FuncNameObfPass> A("func_name", "Rename Function&Struct Name Randomly", false, false);
//static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_OptimizerLast, loadPass);
static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_ModuleOptimizerEarly, loadPass);
static RegisterStandardPasses D(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0, loadPass);

#include "llvm/Transforms/Obfuscation/FuncNameObf.h"
#include "llvm/Transforms/Obfuscation/macros.h"
#include "llvm/support/typename.h"


#define DEBUG_TYPE "funcobf"

// ref: http://mayuyu.io/2017/06/01/LLVMHacking-0x1/
// ref: https://llvm.org/doxygen/MetaRenamer_8cpp_source.html
// ref: http://mayuyu.io/2017/06/02/LLVMHacking-0x2/
// ref: http://scottcarr.github.io/2016/01/24/my-week-in-llvm.html
// ref: https://stackoverflow.com/questions/14811587/how-to-get-functiontype-from-callinst-when-call-is-indirect-in-llvm
// ref: https://stackoverflow.com/questions/40789200/llvm-rename-function-inside-a-module
// ref: https://stackoverflow.com/questions/13104085/what-to-pass-for-the-vmap-argument-of-clonefunction-in-llvm

using namespace llvm;
using namespace std;

static string obfcharacters="qwertyuiopasdfghjklzxcvbnm1234567890";
static string prefix = "obf_";

namespace llvm {
  struct FuncNameObfPass : public ModulePass {
    static char ID;
    bool is_flag = true;
    FuncNameObfPass() : ModulePass(ID) {}
    FuncNameObfPass(bool flag):
      ModulePass(ID) {
      is_flag = flag;
    }

    string randomString(int length) {
      string name;
      name.resize(length);
      for(int i=0;i<length;i++){
        name[i]=obfcharacters[rand()%(obfcharacters.length()+1)];
      }
      return name;
    }

    string getObfName(int length) {
      return prefix + randomString(length);
    }

    bool isObfFun(StringRef &s) {
      return s.str().rfind(prefix, 0) == 0;
    }

    bool runOnModule(Module &M) override {
      if (!is_flag)
	return false;
      string _name;

      for (auto AI = M.alias_begin(), AE = M.alias_end(); AI != AE; ++AI) {
	DEBUG_OUT("Skipping Alias:"<<AI->getName());
      }


      for(auto &F: M) {
	StringRef Name = F.getName();
	if (F.isDeclaration()) {
	  DEBUG_OUT("Skipping External Function: "<< F.getName());
	  continue;
	}
	if ((Name != "main") && !isObfFun(Name)){
	  StringRef fnName = getObfName(16);
	  auto originName = F.getName();
	  F.setName(fnName);
	  DEBUG_OUT("Renaming Function: `"<< originName.str() << "` to: "<<fnName);
	} else {
	  continue;
	}
      }
      // Rename all struct types
      TypeFinder StructTypes;
      StructTypes.run(M, true);


      for(auto G=M.global_begin(); G!=M.global_end(); G++) {
        GlobalVariable &GV=*G;
	if (GV.isDeclaration()) {
	  DEBUG_OUT("Skipping External Varable: "<< GV.getName());
	  continue;
	}
	if(GV.getName().str().find("OBJC_METH_VAR_NAME_") == 0) {
	  DEBUG_OUT("Link: " << GV.getName());
	  // SHT_LLVM_LINKER_OPTIONS
	  DEBUG_OUT("Link: " << GV.getValueName());

	}
	_name = getObfName(16);
	GV.getInitializer()->setName(_name);
	DEBUG_OUT("Renaming Varable: "<<GV.getName()<<" to: "<<_name);
	GV.setName(_name);

	if(GV.hasInitializer()) {
	  _name = getObfName(16);
	  GV.getInitializer()->setName(_name);
	  DEBUG_OUT("Renaming Class: "<<GV.getInitializer()->getName()<<" to: "<<_name);
        }
      }
      return true;
    }
    void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesCFG();
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

#include "llvm/Transforms/Obfuscation/FuncNameObf.h"
#include "llvm/Transforms/Obfuscation/macros.h"
#include "llvm/support/typename.h"
#include "llvm/IR/ValueSymbolTable.h"

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
      //      handleObjc(M);

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
	  renameSymbol(M, Name.str(), fnName.str());
	  DEBUG_OUT("Renaming Function: `"<< originName.str() << "` to: "<<fnName);
	} else {
	  continue;
	}
	// ref: https://llvm.org/doxygen/StripSymbols_8cpp_source.html
      }
      // Rename all struct types
      TypeFinder StructTypes;
      StructTypes.run(M, true);


      for(auto G=M.global_begin(); G!=M.global_end(); G++) {
        GlobalVariable &GV=*G;
	_name = getObfName(16);
	if (GV.isDeclaration() || GV.isDeclarationForLinker()) {
	  DEBUG_OUT("Skipping External Varable: "<< GV.getName());
	  continue;
	}
	if(GV.getName().str().find("OBJC_METH_VAR_NAME_") == 0) {
	  _name = getObfName(16);
	  llvm::Constant *val = llvm::ConstantDataArray::getString(M.getContext(), _name, true);
	  val->setName(_name);
	  //	  GV.setInitializer(val);
	  DEBUG_OUT("Setting METH_VAR: "<<GV.getInitializer()->getName()<<" to: "<<_name);
	  renameSymbol(M, GV.getName().str(), _name);
	}
	if(GV.getName().str().find("OBJC_CLASS_$_") == 0) {
	  string className = GV.getName().str();
	  DEBUG_OUT("Renaming Varable: "<<GV.getName()<<" to: "<<_name);
	  className.replace(className.find("OBJC_CLASS_$_"), strlen("OBJC_CLASS_$_"), "");

	  if(GV.hasInitializer()) {
	    _name = getObfName(8);
	    llvm::Constant *val = llvm::ConstantDataArray::getString(M.getContext(), _name, false);
	    //GV.setInitializer(val);
	    DEBUG_OUT("Setting OBJC_CLASS: "<<GV.getName()<<" to: "<<_name);
	    //GV.setName(_name);
	    renameSymbol(M, GV.getName().str(), _name);
	  }
	}
	DEBUG_OUT("Renaming Variable: "<<GV.getName()<<" to: "<<_name);
	if(GV.isConstant() && GV.hasInitializer()) {
	  GV.getInitializer()->setName(_name);
	  DEBUG_OUT("Renaming Class: "<<GV.getInitializer()->getName()<<" to: "<<_name);
        }
      }
      return true;
    }
    void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesCFG();
    }
    void renameSymbol(Module &M, string Name, string toName) {
      ValueSymbolTable ST = M.getValueSymbolTable();
      for (ValueSymbolTable::iterator VI = ST.begin(), VE = ST.end(); VI != VE; ) {
	Value *V = VI->getValue();
	++VI;
	if(V->getName().str().find(Name) == 0) {
	    if (!isa<GlobalValue>(V) || cast<GlobalValue>(V)->hasLocalLinkage()) {
	      V->setName(toName);
	    }
	}
      }
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

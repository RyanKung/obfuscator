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
    bool is_flag = false;
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
	if (F.getName().str().compare("main")==0){
	  DEBUG_OUT("Skipping main");
	}
	else if(F.empty()==false){
	  //Rename
	  _name = randomString(16);
	  DEBUG_OUT("Renaming Function: "<<F.getName()<<" to: "<<_name);
	  F.setName(randomString(16));
	}
	else{
	  DEBUG_OUT("Skipping External Function: "<<F.getName());
	}
      }
      // Rename all struct types
      TypeFinder StructTypes;
      StructTypes.run(M, true);

      for (StructType *STy : StructTypes) {
	if (STy->isLiteral() || STy->getName().empty()) {
	  DEBUG_OUT("Skipping External Struct: "<<STy->getName());
	} else {
	  _name = randomString(16);
	  DEBUG_OUT("Renaming Struct: "<<STy->getName()<<" to: "<<_name);
	  STy->setName(randomString(16));
	}
      }

      for(auto G=M.global_begin(); G!=M.global_end(); G++) {
        GlobalVariable &GV=*G;
        if (GV.getName().str().find("OBJC_CLASSLIST_REFERENCES")==0) {
	  if(GV.hasInitializer()) {
	    string className = (GV.getInitializer ()->getName()).str();
	    className.replace(className.find("OBJC_CLASS_$_"),strlen("OBJC_CLASS_$_"),"");
	    for(auto U=GV.user_begin (); U!=GV.user_end(); U++) {
	      if (Instruction* I = dyn_cast<Instruction>(*U)) {
		IRBuilder<> builder(I);
		FunctionType *objc_getClass_type = FunctionType::get(I->getType(),
								     {Type::getInt8PtrTy(M.getContext())},
								     false);
		llvm::FunctionCallee objc_getClass_Func = M.getOrInsertFunction("objc_getClass",
										objc_getClass_type);
		Value* newClassName=builder.CreateGlobalStringPtr(StringRef(className));
		CallInst* CI=builder.CreateCall(objc_getClass_Func,{newClassName});
		I->replaceAllUsesWith(CI);
		I->eraseFromParent ();
		DEBUG_OUT("Renaming Class: " << className << ": " << newClassName);
	      }
	    }
	  }
        }
      }
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
static RegisterPass<FuncNameObfPass> A("func_name", "Rename Function&Struct Name Randomly", true, true);
static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_OptimizerLast, loadPass);
static RegisterStandardPasses D(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0, loadPass);

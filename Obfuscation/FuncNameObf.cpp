#include "llvm/Transforms/Obfuscation/FuncNameObf.h"
#include "llvm/Transforms/Obfuscation/macros.h"

#define DEBUG_TYPE "funcobf"

// ref: http://mayuyu.io/2017/06/01/LLVMHacking-0x1/
// ref: https://llvm.org/doxygen/MetaRenamer_8cpp_source.html
// ref: http://mayuyu.io/2017/06/02/LLVMHacking-0x2/

using namespace llvm;
using namespace std;

static const int DARWIN_FLAG = 0x2 | 0x8;
static const int ANDROID64_FLAG = 0x00002 | 0x100;
static const int ANDROID32_FLAG = 0x0000 | 0x2;
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

    virtual bool doInitialization(Module &M) override {
      Triple tri(M.getTargetTriple());
      if (tri.getVendor() != Triple::VendorType::Apple) {
	return false;
      }
      Type *Int64Ty = Type::getInt64Ty(M.getContext());
      Type *Int32Ty = Type::getInt32Ty(M.getContext());
      Type *Int8PtrTy = Type::getInt8PtrTy(M.getContext());
      Type *Int8Ty = Type::getInt8Ty(M.getContext());
      // Generic ObjC Runtime Declarations
      FunctionType *IMPType =
        FunctionType::get(Int8PtrTy, {Int8PtrTy, Int8PtrTy}, true);
      PointerType *IMPPointerType = PointerType::get(IMPType, 0);
      vector<Type *> classReplaceMethodTypeArgs;
      classReplaceMethodTypeArgs.push_back(Int8PtrTy);
      classReplaceMethodTypeArgs.push_back(Int8PtrTy);
      classReplaceMethodTypeArgs.push_back(IMPPointerType);
      classReplaceMethodTypeArgs.push_back(Int8PtrTy);
      FunctionType *class_replaceMethod_type =
        FunctionType::get(IMPPointerType, classReplaceMethodTypeArgs, false);
      M.getOrInsertFunction("class_replaceMethod", class_replaceMethod_type);
      FunctionType *sel_registerName_type =
        FunctionType::get(Int8PtrTy, {Int8PtrTy}, false);
      M.getOrInsertFunction("sel_registerName", sel_registerName_type);
      FunctionType *objc_getClass_type =
        FunctionType::get(Int8PtrTy, {Int8PtrTy}, false);
      M.getOrInsertFunction("objc_getClass", objc_getClass_type);
      M.getOrInsertFunction("objc_getMetaClass", objc_getClass_type);
      StructType *objc_property_attribute_t_type =
	reinterpret_cast<StructType *>
	(M.getTypeByName("struct.objc_property_attribute_t"));
      if (objc_property_attribute_t_type == NULL) {
	vector<Type *> types;
	types.push_back(Int8PtrTy);
	types.push_back(Int8PtrTy);
	objc_property_attribute_t_type =
	  StructType::create(ArrayRef<Type *>(types), "struct.objc_property_attribute_t");
	M.getOrInsertGlobal("struct.objc_property_attribute_t",
			    objc_property_attribute_t_type);
      }
      vector<Type *> allocaClsTypeVector;
      vector<Type *> addIvarTypeVector;
      vector<Type *> addPropTypeVector;
      allocaClsTypeVector.push_back(Int8PtrTy);
      allocaClsTypeVector.push_back(Int8PtrTy);
      addIvarTypeVector.push_back(Int8PtrTy);
      addIvarTypeVector.push_back(Int8PtrTy);
      addPropTypeVector.push_back(Int8PtrTy);
      addPropTypeVector.push_back(Int8PtrTy);
      addPropTypeVector.push_back(objc_property_attribute_t_type->getPointerTo());
      if (tri.isArch64Bit()) {
	// We are 64Bit Device
	allocaClsTypeVector.push_back(Int64Ty);
	addIvarTypeVector.push_back(Int64Ty);
	addPropTypeVector.push_back(Int64Ty);
      } else {
	// Not 64Bit.However we are still on apple platform.So We are
	// ARMV7/ARMV7S/i386
	// PowerPC is ignored, feel free to open a PR if you want to
	allocaClsTypeVector.push_back(Int32Ty);
	addIvarTypeVector.push_back(Int32Ty);
	addPropTypeVector.push_back(Int32Ty);
      }
      addIvarTypeVector.push_back(Int8Ty);
      addIvarTypeVector.push_back(Int8PtrTy);
      // Types Collected. Now Inject Functions
      FunctionType *addIvarType =
        FunctionType::get(Int8Ty, ArrayRef<Type *>(addIvarTypeVector), false);
      M.getOrInsertFunction("class_addIvar", addIvarType);
      FunctionType *addPropType =
        FunctionType::get(Int8Ty, ArrayRef<Type *>(addPropTypeVector), false);
      M.getOrInsertFunction("class_addProperty", addPropType);
      FunctionType *class_getName_Type =
        FunctionType::get(Int8PtrTy, {Int8PtrTy}, false);
      M.getOrInsertFunction("class_getName", class_getName_Type);
      FunctionType *objc_getMetaClass_Type =
        FunctionType::get(Int8PtrTy, {Int8PtrTy}, false);
      M.getOrInsertFunction("objc_getMetaClass", objc_getMetaClass_Type);

      FunctionType *dlopen_type =
	FunctionType::get(
			  Int8PtrTy, {Int8PtrTy, Int32Ty},
			  false); // int has a length of 32 on both 32/64bit platform

      FunctionType *dlsym_type =
        FunctionType::get(Int8PtrTy, {Int8PtrTy, Int8PtrTy}, false);
      M.getOrInsertFunction("dlopen", dlopen_type);
      M.getOrInsertFunction("dlsym", dlsym_type);

      return true;
    }

    bool runOnModule(Module &M) override {
      if (!is_flag)
	return false;
      handleObjc(M);
      return true;
      for(auto &F: M){
	runOnFunction(F, M);
      }
    }

    bool runOnFunction(Function &F, Module &M) {
      Triple Tri(F.getParent()->getTargetTriple());
      if (!Tri.isAndroid() && !Tri.isOSDarwin()) {
	errs() << "Unsupported Target Triple:"<< F.getParent()->getTargetTriple() << "\n";
	return false;
      }

      for (BasicBlock &BB: F) {
	for (auto I = BB.getFirstInsertionPt(), end = BB.end(); I != end; ++I) {
	  Instruction &Inst = *I;
	  if (isa<CallInst>(&Inst) || isa<InvokeInst>(&Inst)) {
	    CallBase *CB = dyn_cast<CallBase>(&Inst);
	    Function *calledFunction = CB->getCalledFunction();
	    if (calledFunction == NULL) {
	      /*
		Note:
		For Indirect Calls:
                CalledFunction is NULL and calledValue is usually a bitcasted
		function pointer. We'll need to strip out the hiccups and obtain
		the called Function* from there
	      */
	      calledFunction =
                dyn_cast<Function>(CB->getCalledOperand()->stripPointerCasts());
	    }
	    // Simple Extracting Failed
	    // Use our own implementation
	    if (calledFunction == NULL) {
	      DEBUG_OUT("Failed To Extract Function From Indirect Call: "
			<< *CB->getCalledOperand());
	      continue;
	    }
	    // It's only safe to restrict our modification to external symbols
	    // Otherwise stripped binary will crash
	    if (!calledFunction->empty() ||
		calledFunction->getName().equals("dlsym") ||
		calledFunction->getName().equals("dlopen") ||
		calledFunction->isIntrinsic()) {
	      continue;
	    }
	    string sname = randomString(16);
            StringRef calledFunctionName = StringRef(sname);
            BasicBlock *EntryBlock = CB->getParent();
            IRBuilder<> IRB(EntryBlock, EntryBlock->getFirstInsertionPt());
            vector<Value *> dlopenargs;
	    Type *Int32Ty = Type::getInt32Ty(M.getContext());
	    Type *Int8PtrTy = Type::getInt8PtrTy(M.getContext());
            dlopenargs.push_back(Constant::getNullValue(Int8PtrTy));
            dlopenargs.push_back(ConstantInt::get(Int32Ty, DARWIN_FLAG));
	    Function *dlopen_decl = cast<Function>(M.getFunction("dlopen"));
	    Function *dlsym_decl = cast<Function>(M.getFunction("dlsym"));
	    Value *Handle =
	      IRB.CreateCall(dlopen_decl, ArrayRef<Value *>(dlopenargs));
            // Create dlsym call
            vector<Value *> args;
            args.push_back(Handle);
            args.push_back(IRB.CreateGlobalStringPtr(calledFunctionName));
            Value *fp = IRB.CreateCall(dlsym_decl, ArrayRef<Value *>(args));
            Value *bitCastedFunction =
	      IRB.CreateBitCast(fp, CB->getCalledOperand()->getType());
	    CB->setCalledOperand(bitCastedFunction);
	  }
	}
      }
    }

    void handleObjc(Module &M) {
      for(auto G=M.global_begin(); G!=M.global_end(); G++) {
        GlobalVariable &GV=*G;
        if (GV.getName().str().find("OBJC_CLASSLIST_REFERENCES") == 0) {
	  if(GV.hasInitializer()) {
	    string className = (GV.getInitializer ()->getName()).str();
	    className.replace(className.find("OBJC_CLASS_$_"), strlen("OBJC_CLASS_$_"), "");
	    for(auto U=GV.user_begin(); U!=GV.user_end(); U++) {
	      if (Instruction* I = dyn_cast<Instruction>(*U)) {
		IRBuilder<> builder(I);
		Function *objc_getClass_Func = cast<Function>(M.getFunction("objc_getClass"));
		Value* newClassName=builder.CreateGlobalStringPtr(StringRef(className));
		CallInst* CI=builder.CreateCall(objc_getClass_Func,{newClassName});
		I->replaceAllUsesWith(CI);
		I->eraseFromParent ();
		DEBUG_OUT("Renaming Class: " << className << " to: " << newClassName);
	      }
	    }
	  }
        }
	else if (GV.getName().str().find("OBJC_SELECTOR_REFERENCES") == 0) {
	  if (GV.hasInitializer()) {
	    ConstantExpr *CE = dyn_cast<ConstantExpr>(GV.getInitializer());
	    Constant *C = CE->getOperand(0);
	    GlobalVariable *SELNameGV = dyn_cast<GlobalVariable>(C);
	    ConstantDataArray *CDA =
              dyn_cast<ConstantDataArray>(SELNameGV->getInitializer());
	    StringRef SELName = CDA->getAsString(); // This is REAL Selector Name

	    for (auto U = GV.user_begin(); U != GV.user_end(); U++) {
	      if (Instruction *I = dyn_cast<Instruction>(*U)) {
	    	IRBuilder<> builder(I);
		FunctionType *fn_type = FunctionType::get(I->getType(),
								     {Type::getInt8PtrTy(M.getContext())},
								     false);
		Function *sel_registerName_Func = cast<Function>(M.getFunction("sel_registerName"));
	    	Value *newGlobalSELName = builder.CreateGlobalStringPtr(SELName);
		CallInst *CI = builder.CreateCall(sel_registerName_Func, {newGlobalSELName});
	    	Value *BCI = builder.CreateBitCast(CI, I->getType());
	    	I->replaceAllUsesWith(BCI);
	    	I->eraseFromParent();
		DEBUG_OUT("Renaming SEL: " << SELName.str() << " to: " << newGlobalSELName);
	      }
	    }
	    GV.removeDeadConstantUsers();
	    if (GV.getNumUses() == 0) {
	      GV.dropAllReferences();
	      GV.eraseFromParent();
	    }
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
static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_OptimizerLast, loadPass);
//static RegisterStandardPasses C(llvm::PassManagerBuilder::EP_ModuleOptimizerEarly, loadPass);
static RegisterStandardPasses D(llvm::PassManagerBuilder::EP_EnabledOnOptLevel0, loadPass);

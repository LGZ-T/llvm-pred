#include <llvm/Pass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <ValueProfiling.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>

using namespace llvm;

namespace{
    struct InsertTime:public FunctionPass{
        static char ID;
        InsertTime():FunctionPass(ID) {}
        bool runOnFunction(Function &f) override
        {
            Module *M = f.getParent();
            LLVMContext& Context = f.getContext();
//            Type* inst64ty = Type::getInt64Ty(Context);
            Constant* Funcentry = M->getOrInsertFunction("gettime",Type::getVoidTy(Context),NULL);
            Constant* Funcentry1 = M->getOrInsertFunction("outinfo",Type::getVoidTy(Context),NULL);
            
            if(f.getName().equals("main"))
            {
                Instruction &inst = f.front().front();
                Instruction &end = f.back().back();
                Instruction *inst_start = &inst;
                Instruction *inst_end = &end;
                CallInst::Create(Funcentry,"",inst_start);
                CallInst::Create(Funcentry,"",inst_end);
                CallInst::Create(Funcentry1,"",inst_end);
            }
            /*BasicBlock::iterator start,end;
            Module *M = bb.getParent()->getParent();
            LLVMContext& Context = bb.getContext();
            Type* int64ty = Type::getInt64Ty(Context);
            Constant* FuncEntry1 = M->getOrInsertFunction("getBBTime", Type::getVoidTy(Context),int64ty,NULL);
            //Constant* FuncEntry2 = M->getOrInsertFunction("getBBTime2", Type::getVoidTy(Context),int64ty,NULL);
            Value *args[1] = {ConstantInt::get(int64ty,bbid)};
            bbid++;
            start = bb.getFirstInsertionPt();
            end = bb.end();
            end--;
            Instruction *first = &*start;
            Instruction *last = &*end;
            if(first!=last)
            {
                CallInst::Create(FuncEntry1,args,"",first);
                CallInst::Create(FuncEntry1,args,"",last);
            }*/
            /*for(start=bb.begin(),end=bb.end();start!=end;start++)
            {
                Instruction *inst = &*start;
                opcode = inst->getOpcode();
                
            }*/
            return true;
        }
    };
}

char InsertTime::ID = 0;
static RegisterPass<InsertTime> X("InsertTime","insert time func at the beginning and end of func main",false,false);

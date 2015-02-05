#include "preheader.h"
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "util.h"
#include "debug.h"

using namespace llvm;

/* A Pass used to reshape a function define. that is replace a function declare
 * to what we defined. Extremely useful to add extra informations for lib function.
 * this also clean code, remove bitcast @func to type.
 *
 * a environment *LIBFDEF_FILE* is used to set a .ll file which contail all
 * declare function. LibFDef means library function define.
 */
class LibFReshape: public ModulePass
{
   public:
   static char ID;
   LibFReshape():ModulePass(ID) {}
   void getAnalysisUsage(AnalysisUsage& AU) const
   {
      AU.setPreservesAll();
   }
   bool runOnModule(Module& M);
};

char LibFReshape::ID = 0;

static RegisterPass<LibFReshape> X("LibFReshape", "Set a existing lib function"
      "declare to attach extra information");

bool LibFReshape::runOnModule(Module &M)
{
   const char* libfdef = getenv("LIBFDEF_FILE");
   if(!libfdef) return false;

   SMDiagnostic Err;
   Module* def = ParseIRFile(libfdef, Err, M.getContext());
   if(def == NULL){
      Err.print("error", errs());
      return false;
   }

   for(Module::iterator F = def->begin(), E = def->end(); F != E; ++F){
      Function* Old = M.getFunction(F->getName());
      if(!Old) continue;
      Old->removeFromParent();
      Constant* New = M.getOrInsertFunction(F->getName(), F->getFunctionType(), F->getAttributes());

      for(auto I = Old->user_begin(), E = Old->user_end(); I!=E; ++I){
         if(ConstantExpr* CE = dyn_cast<ConstantExpr>(*I))
            //Assume all CE is used with in a call
            CE->replaceAllUsesWith(New);
         else if(CallInst* CI = dyn_cast<CallInst>(*I))
            CI->setCalledFunction(New);
         else Assert(0,"shouldn't come to here");
      }
      /** avoid <badref> bitcast Old ; keep the use relationship of Use. though
       * bitcast has already removedFromParent. so we replace it with undef
       * trickly. */
      Old->replaceAllUsesWith(UndefValue::get(Old->getType()));
   }
   delete def;//release memory
   return false;
}

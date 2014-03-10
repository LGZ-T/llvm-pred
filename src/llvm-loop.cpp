#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/PassManager.h>
#include <llvm/Bitcode/ReaderWriter.h>

#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Transforms/Utils/SimplifyIndVar.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "loop.h"

using namespace llvm;

namespace {
	cl::opt<std::string>
		BitcodeFile(cl::Positional, cl::desc("<program bitcode file>"),
				cl::Required);
	cl::opt<std::string>
		WriteFile(cl::Positional,cl::desc("<write bitcode file>"),
				cl::Optional);
};

static ValueProfiler* VProf = NULL;

class LoopPrintPass:public FunctionPass 
{
	static char ID;
	public:
	explicit LoopPrintPass(): FunctionPass(ID) {}
	void getAnalysisUsage(AnalysisUsage &AU) const 
	{
		//AU.setPreservesAll();
		AU.addRequired<LoopInfo>();
	}
	void runOnLoop(Loop* l)
	{
		lle::Loop L(l);
		Value* CC = L.insertLoopCycle(VProf);
		outs()<<*l<<"\n";
		if(CC){
			outs()<<"cycles:"<<*L.getLoopCycle()<<"\n";
		}else
			outs()<<"cycles:unknow\n";
		if(!L->getSubLoops().empty()){
			for(auto I = L->getSubLoops().begin(),E = L->getSubLoops().end();I!=E;I++)
				runOnLoop(*I);
		}
	}
	bool runOnFunction(Function& F)
	{
		StringRef func_name = F.getName();
		outs()<<"------------------------\n";
		outs()<<"Function:"<<func_name<<"\n";
		outs()<<"------------------------\n";
		LoopInfo& LI = getAnalysis<LoopInfo>();
		for(auto ite = LI.begin(), end = LI.end();ite!=end;ite++){
			runOnLoop(*ite);
		}
		return true;
	}
};

char LoopPrintPass::ID = 0;

int main(int argc, char **argv) {
	// Print a stack trace if we signal out.
	sys::PrintStackTraceOnErrorSignal();
	PrettyStackTraceProgram X(argc, argv);

	LLVMContext &Context = getGlobalContext();
	llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.

	cl::ParseCommandLineOptions(argc, argv, "llvm profile dump decoder\n");

	// Read in the bitcode file...
	std::string ErrorMessage;
	OwningPtr<MemoryBuffer> Buffer;
	error_code ec;
	Module *M = 0;
	if (!(ec = MemoryBuffer::getFileOrSTDIN(BitcodeFile, Buffer))) {
		M = ParseBitcodeFile(Buffer.get(), Context, &ErrorMessage);
	} else
		ErrorMessage = ec.message();
	if (M == 0) {
		errs() << argv[0] << ": " << BitcodeFile << ": "
			<< ErrorMessage << "\n";
		return 1;
	}

	// Read the profiling information. This is redundant since we load it again
	// using the standard profile info provider pass, but for now this gives us
	// access to additional information not exposed via the ProfileInfo
	// interface.

	VProf = new ValueProfiler();
	FunctionPassManager f_pass_mgr(M);
	f_pass_mgr.add(new LoopInfo());
	f_pass_mgr.add(new LoopPrintPass());

	for( auto& func : *M ){
		f_pass_mgr.run(func);
	}
	VProf->runOnModule(*M);

	if(!WriteFile.empty()){
		std::string error;
		raw_fd_ostream output(WriteFile.c_str(),error);
		WriteBitcodeToFile(M, output);
	}
	return 0;
}

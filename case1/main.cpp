#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

using namespace llvm;

int main()
{
	LLVMContext c;
	auto m = std::make_unique<Module>("test module", c);
	m->print(outs(), nullptr);
	return 0;
}

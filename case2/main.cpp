#include <memory>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

using namespace llvm;

template <typename T>
using viewPtr = T*;

int main()
{
	LLVMContext c;
	auto m = std::make_unique<Module>("test module", c);
	viewPtr<Type> voidTy = Type::getVoidTy(c);
  	viewPtr<FunctionType> funcTy = FunctionType::get(voidTy, false);
	viewPtr<Function> func = Function::Create(funcTy, GlobalValue::ExternalLinkage, "test_function IR", m.get());
 
	viewPtr<BasicBlock> b = BasicBlock::Create(c, "entry_block", func);
	IRBuilder<> builder(c);
	builder.SetInsertPoint(b);
	m->print(outs(), nullptr);
	return 0;
}

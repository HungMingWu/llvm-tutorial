#include <memory>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

using namespace llvm;

template <typename T>
using viewPtr = T*;

viewPtr<GlobalVariable> createGlobalVariable(Module* m, StringRef Name, viewPtr<Type> Ty)
{
	m->getOrInsertGlobal(Name, Ty);
	return m->getNamedGlobal(Name);
}

int main()
{
	LLVMContext c;
	auto m = std::make_unique<Module>("test module", c);

	viewPtr<GlobalVariable> gv = createGlobalVariable(m.get(), "test_global_var", Type::getInt32Ty(c));
	gv->setLinkage(GlobalValue::CommonLinkage);
	gv->setAlignment(Align(4));

	IRBuilder<> builder(c);
	viewPtr<Type> voidTy = Type::getVoidTy(c);
	std::vector<Type *> paramTys(2, builder.getInt32Ty());
	viewPtr<FunctionType> funcTy = FunctionType::get(voidTy, paramTys, false);
	viewPtr<Function> func = Function::Create(funcTy, GlobalValue::ExternalLinkage, "test_function IR", m.get());
	func->getArg(0)->setName("a");
	func->getArg(1)->setName("b");

	viewPtr<BasicBlock> b = BasicBlock::Create(c, "entry_block", func);
	builder.SetInsertPoint(b);

	viewPtr<ConstantInt> one = builder.getInt32(1);
	builder.CreateRet(one);
	m->print(outs(), nullptr);
	return 0;
}

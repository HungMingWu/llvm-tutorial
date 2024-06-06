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
	std::vector<Type *> paramTys(2, builder.getInt32Ty());
	viewPtr<FunctionType> funcTy = FunctionType::get(builder.getInt32Ty(), paramTys, false);
	viewPtr<Function> func = Function::Create(funcTy, GlobalValue::ExternalLinkage, "test_function IR", m.get());

	viewPtr<Value> arg0 = func->getArg(0);
	arg0->setName("a");
	viewPtr<Value> arg1 = func->getArg(1);
	arg1->setName("b");

	viewPtr<BasicBlock> b = BasicBlock::Create(c, "entry_block", func);
	builder.SetInsertPoint(b);

	viewPtr<ConstantInt> two = builder.getInt32(2);
	viewPtr<Value> tmp = builder.CreateMul(arg1, two, "mul_ans");
  	viewPtr<Value> ret = builder.CreateAdd(arg0, tmp, "add_ans");
	builder.CreateRet(ret);
	m->print(outs(), nullptr);
	return 0;
}

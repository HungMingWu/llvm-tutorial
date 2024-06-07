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
	arg1->setName("c");

	viewPtr<BasicBlock> bEntry = BasicBlock::Create(c, "entry_block", func);
	viewPtr<BasicBlock> bIf = BasicBlock::Create(c, "if.then", func);
	viewPtr<BasicBlock> bElse = BasicBlock::Create(c, "if.else", func);
	viewPtr<BasicBlock> bIfEnd = BasicBlock::Create(c, "if.end", func);

	builder.SetInsertPoint(bEntry);
	viewPtr<Value> bPtr = builder.CreateAlloca(builder.getInt32Ty(), nullptr, "b.addr");
	viewPtr<ConstantInt> vConst5 = builder.getInt32(5);
	viewPtr<Value> vCondition = builder.CreateICmpSGT(arg0, vConst5, "compare.ret");
	builder.CreateCondBr(vCondition, bIf, bElse);

	builder.SetInsertPoint(bIf);
	viewPtr<ConstantInt> vConst6 = builder.getInt32(6);
	builder.CreateStore(vConst6, bPtr);
	builder.CreateBr(bIfEnd);

	builder.SetInsertPoint(bElse);
	builder.CreateStore(vConst5, bPtr);
	builder.CreateBr(bIfEnd);

	builder.SetInsertPoint(bIfEnd);
	viewPtr<Value> bVal = builder.CreateLoad(builder.getInt32Ty(), bPtr, "ret.val");
	builder.CreateRet(bVal);

	m->print(outs(), nullptr);
	return 0;
}

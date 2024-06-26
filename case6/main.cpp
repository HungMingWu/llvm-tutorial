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

viewPtr<Function> createFunction(viewPtr<Module> mod, viewPtr<Type> Ret_type, ArrayRef<viewPtr<Type>> Params, std::string Name, bool isVarArg = false)
{
	viewPtr<FunctionType> func_Type = FunctionType::get(Ret_type, Params, isVarArg);
	return Function::Create(func_Type, Function::ExternalLinkage, Name, mod);
}

int main()
{
	LLVMContext c;
	auto m = std::make_unique<Module>("test module", c);

	viewPtr<GlobalVariable> gv = createGlobalVariable(m.get(), "test_global_var", Type::getInt32Ty(c));
	gv->setLinkage(GlobalValue::CommonLinkage);
	gv->setAlignment(Align(4));

	IRBuilder<> builder(c);
	SmallVector<Type *> paramTys(2, builder.getInt32Ty());
	viewPtr<Function> func = createFunction(m.get(), builder.getInt32Ty(), paramTys, "test_function IR");

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
	builder.CreateBr(bIfEnd);

	builder.SetInsertPoint(bElse);
	builder.CreateBr(bIfEnd);

	builder.SetInsertPoint(bIfEnd);
	viewPtr<PHINode> phi = builder.CreatePHI(builder.getInt32Ty(), 2);
	phi->addIncoming(vConst6, bIf);
	phi->addIncoming(vConst5, bElse);

	viewPtr<Value> bVal = builder.CreateLoad(builder.getInt32Ty(), bPtr, "ret.val");
	builder.CreateRet(bVal);

	m->print(outs(), nullptr);
	return 0;
}

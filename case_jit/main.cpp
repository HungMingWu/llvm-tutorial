#include <memory>
#include <vector>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <llvm/ExecutionEngine/MCJIT.h>

using namespace llvm;

template <typename T>
using viewPtr = T*;

viewPtr<Function> createFunction(viewPtr<Module> mod, viewPtr<Type> Ret_type, ArrayRef<viewPtr<Type>> Params, std::string Name, bool isVarArg = false)
{
	viewPtr<FunctionType> func_Type = FunctionType::get(Ret_type, Params, isVarArg);
	return Function::Create(func_Type, Function::ExternalLinkage, Name, mod);
}

void createBasicBlock(LLVMContext& c, viewPtr<Function> func) {
	viewPtr<Value> arg = func->getArg(0);
	arg->setName("n");

	IRBuilder<> builder(c);

	viewPtr<BasicBlock> entry = BasicBlock::Create(c, "entry", func);
	viewPtr<BasicBlock> loop_condition = BasicBlock::Create(c, "loop_condition", func);
	viewPtr<BasicBlock> loop = BasicBlock::Create(c, "loop", func);
	viewPtr<BasicBlock> ret = BasicBlock::Create(c, "ret", func);

	builder.SetInsertPoint(entry);

	viewPtr<Value> sum = builder.CreateAlloca(builder.getInt32Ty(), nullptr, "sum");
	viewPtr<Value> i = builder.CreateAlloca(builder.getInt32Ty(), nullptr, "i");
	viewPtr<Value> n_1 = builder.CreateAlloca(builder.getInt32Ty(), nullptr, "n");

	builder.CreateStore(arg, n_1);
	builder.CreateStore(builder.getInt32(0), i);
    	builder.CreateStore(builder.getInt32(0), sum);
	builder.CreateBr(loop_condition);

	builder.SetInsertPoint(loop_condition);
	viewPtr<Value> var_i_1 = builder.CreateLoad(builder.getInt32Ty(), i);
	viewPtr<Value> var_n = builder.CreateLoad(builder.getInt32Ty(), n_1);
	viewPtr<Value> cmp_var = builder.CreateICmpSLT(var_i_1, var_n);
	builder.CreateCondBr(cmp_var, loop, ret);

	builder.SetInsertPoint(loop);
	viewPtr<Value> var_i_2 = builder.CreateLoad(builder.getInt32Ty(), i);
	viewPtr<Value> var_sum_2 = builder.CreateLoad(builder.getInt32Ty(), sum);
	viewPtr<Value> var_sum_3 = builder.CreateAdd(var_sum_2, var_i_2);
	viewPtr<Value> var_i_3 = builder.CreateLoad(builder.getInt32Ty(), i);
	viewPtr<Value> var_i_4 = builder.CreateAdd(var_i_3, builder.getInt32(1));
	builder.CreateStore(var_sum_3, sum);
	builder.CreateStore(var_i_4, i);
	builder.CreateBr(loop_condition);

	builder.SetInsertPoint(ret);
	viewPtr<Value> var_sum_4 = builder.CreateLoad(builder.getInt32Ty(), sum);
	builder.CreateRet(var_sum_4);
}

int main()
{
	LLVMContext c;
	auto m = std::make_unique<Module>("test module", c);

	SmallVector<Type *> paramTys(1, Type::getInt32Ty(c));
	viewPtr<Function> func = createFunction(m.get(), Type::getInt32Ty(c), paramTys, "sum");

	createBasicBlock(c, func);

	m->print(outs(), nullptr);

	// Need the two function call for JIT
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();

	EngineBuilder build(std::move(m));
	auto OBJ = build.setEngineKind(EngineKind::JIT).setOptLevel(llvm::CodeGenOptLevel::Default).create();
	std::unique_ptr<ExecutionEngine> ee(OBJ);
	void* address = (void *)ee->getFunctionAddress("sum");
	int ret = ((int(*)(int))address)(100);
	return ret;
}

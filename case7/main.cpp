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

	IRBuilder<> builder(c);

	viewPtr<StructType> structType = StructType::create(c, "TestStruct");
	structType->setBody({builder.getInt32Ty(), builder.getInt32Ty(), builder.getInt32Ty(), builder.getInt32Ty()});
	viewPtr<GlobalVariable> gv = createGlobalVariable(m.get(), "test_struct_var", structType);
	gv->setDSOLocal(true);
	SmallVector<viewPtr<Constant>> structInit = {builder.getInt32(1), builder.getInt32(8), builder.getInt32(8), builder.getInt32(10)};
	gv->setInitializer(ConstantStruct::get(structType, structInit));
	gv->setAlignment(MaybeAlign(4));
	gv->setSection("test_section");

	viewPtr<PointerType> ptr_type = PointerType::get(gv->getType(), 0);
	viewPtr<ArrayType> AddressTableType = ArrayType::get(ptr_type, 2);
	SmallVector<Constant *> ptrArrInit = {gv, gv};
	viewPtr<Constant> AddressTableInit = ConstantArray::get(AddressTableType, ptrArrInit);
	viewPtr<GlobalVariable> AddressTable = createGlobalVariable(m.get(), "test_struct_init", AddressTableType);
	AddressTable->setInitializer(AddressTableInit);

	m->print(outs(), nullptr);
	return 0;
}

#include "codegen.h"

#include "ast.h"
#include "tiny_exception.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"

namespace tiny {

	CodeGen::CodeGen(llvm::TargetMachine* tm) : module_(std::make_unique<llvm::Module>("tiny", llvm::getGlobalContext())), builder_(llvm::getGlobalContext())
	{
		module_->setDataLayout(tm->createDataLayout());
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(AST* ast)
	{
		for (auto& node : ast->nodes)
		{
			auto r = node->codegen(this);
		}

		return nullptr;
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(FnDeclaration* node)
	{
		std::vector<llvm::Type*> args;

		for (auto& arg : node->args)
		{
			args.push_back(get_llvm_type(arg->type.get()));
		}

		auto ft = llvm::FunctionType::get(get_llvm_type(node->return_type.get()), args, false);
		auto f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, node->name, module_.get());

		if (node->external)
		{
			return nullptr;
		}
		
		push_scope(std::make_unique<SymbolTable<LLVMSymbol>>(nullptr));
		
		auto bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entryblock", f);
		builder_.SetInsertPoint(bb);

		auto i = 0;
		for (auto& arg : f->args())
		{
			// Set the argument name to get a little bit more readable IR
			arg.setName(node->args[i++]->name);

			auto alloca = create_alloca(f, arg.getName(), arg.getType());
			auto inst = builder_.CreateStore(&arg, alloca);
			current_scope()->add_entry(arg.getName(), std::make_unique<LLVMSymbol>(alloca, get_type(arg.getType())));
		}
		
		for (auto& n : node->body)
		{
			auto r = n->codegen(this);
		}

		llvm::verifyFunction(*f);

		pop_scope();

		return nullptr;
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(ArgDeclaration* node)
	{
		return nullptr;
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(VarDeclaration* node)
	{
		auto f = builder_.GetInsertBlock()->getParent();
		auto exp_result = node->expression->codegen(this);
		auto alloca = create_alloca(f, node->name, get_llvm_type(node->type.get()));

		current_scope()->add_entry(node->name, std::make_unique<LLVMSymbol>(alloca, node->type->type));

		return create_codegen_result(builder_.CreateStore(exp_result->value, alloca));
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(BinaryOperator* node)
	{
		auto l = node->left->codegen(this);
		auto r = node->right->codegen(this);

		switch (node->op)
		{
		case TokenType::Plus: 
			return create_codegen_result(builder_.CreateAdd(l->value, r->value, "addtmp"));
			break;
		case TokenType::Minus: 
			return create_codegen_result(builder_.CreateSub(l->value, r->value, "subtmp"));
			break;
		case TokenType::Star:
			return create_codegen_result(builder_.CreateMul(l->value, r->value, "multmp"));
			break;
		case TokenType::Divide: 
			return create_codegen_result(builder_.CreateSDiv(l->value, r->value, "divtmp"));
			break;
		default: 
			throw TinyException("CodeGen::visit -> BinaryOperator -> default");
		}
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(Identifier* node)
	{
		auto entry = current_scope()->get_entry(node->name);
		return create_codegen_result(builder_.CreateLoad(entry->value->value, node->name));
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(IntLiteral* node)
	{
		return create_codegen_result(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, node->value, true)));
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(StringLiteral* node)
	{
		return create_codegen_result(builder_.CreateGlobalStringPtr(node->value));
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(RetDeclaration* node)
	{
		auto r = node->expression->codegen(this);
		builder_.CreateRet(r->value);
		return nullptr;
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(CallExp* node)
	{
		auto callee = get_function(node->name);
		
		std::vector<llvm::Value*> args;
		for (auto& arg : node->args)
		{
			auto arg_result = arg->codegen(this);
			args.push_back(arg_result->value);
		}
		
		return create_codegen_result(builder_.CreateCall(callee, args, "calltmp"));
	}

	std::unique_ptr<llvm::Module> CodeGen::execute(AST* ast)
	{
		visit(ast);
		return std::move(module_);
	}

	llvm::AllocaInst* CodeGen::create_alloca(llvm::Function* function, const std::string& name, llvm::Type* type)
	{
		auto b = llvm::IRBuilder<>(&function->getEntryBlock(), function->getEntryBlock().begin());
		return b.CreateAlloca(type, nullptr, name);
	}

	llvm::Function* CodeGen::get_function(const std::string& name) const
	{
		return module_->getFunction(name);
	}

	void CodeGen::push_scope(std::unique_ptr<SymbolTable<LLVMSymbol>> scope)
	{
		scopes_.push(std::move(scope));
	}

	void CodeGen::pop_scope()
	{
		scopes_.pop();
	}

	SymbolTable<LLVMSymbol>* CodeGen::current_scope()
	{
		return scopes_.top().get();
	}

	std::unique_ptr<CodegenResult> CodeGen::create_codegen_result(llvm::Value* v) const
	{
		return std::make_unique<CodegenResult>(v);
	}

	llvm::Type* CodeGen::get_llvm_type(const TinyType* type)
	{
		switch (type->type)
		{
		case Type::I32: 
			return llvm::Type::getInt32Ty(llvm::getGlobalContext());
		case Type::I32Ptr:
			return llvm::Type::getInt32PtrTy(llvm::getGlobalContext());
		case Type::I8:
			return llvm::Type::getInt8Ty(llvm::getGlobalContext());
		case Type::I8Ptr:
			return llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
		default: 
			throw TinyException("Default case --> CodeGen::get_llvm_type");
		}
	}

	std::unique_ptr<TinyType> CodeGen::get_type(const llvm::Type* type)
	{
		if(type->isIntegerTy(32))
		{
			if (type->isPointerTy())
				return std::make_unique<TinyType>(Type::I32Ptr);

			return std::make_unique<TinyType>(Type::I32);
		}

		if (type->isIntegerTy(8))
		{
			if (type->isPointerTy())
				return std::make_unique<TinyType>(Type::I8Ptr);

			return std::make_unique<TinyType>(Type::I8);
		}

		throw TinyException("Default case --> CodeGen::get_type");
	}
}

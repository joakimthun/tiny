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

		if(node->external)
		{
			return nullptr;
		}
		
		auto bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entryblock", f);
		builder_.SetInsertPoint(bb);
		
		for (auto& n : node->body)
		{
			auto r = n->codegen(this);
		}
		
		llvm::verifyFunction(*f);
		return nullptr;
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(ArgDeclaration* node)
	{
		return nullptr;
	}

	std::unique_ptr<CodegenResult> CodeGen::visit(VarDeclaration* node)
	{
		if (node->expression->node_type() == NodeType::StringLiteral)
		{
			auto str = static_cast<StringLiteral*>(node->expression.get());
			return create_codegen_result(builder_.CreateGlobalStringPtr(str->value, node->name));
		}

		return nullptr;
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
		return nullptr;
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
		return nullptr;
	}

	std::unique_ptr<llvm::Module> CodeGen::execute(AST* ast)
	{
		visit(ast);
		return std::move(module_);
	}

	std::unique_ptr<CodegenResult> CodeGen::create_codegen_result(llvm::Value* v)
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
}

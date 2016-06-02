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

	void CodeGen::visit(AST* ast)
	{
		for (auto& node : ast->nodes)
			node->accept(this);
	}

	void CodeGen::visit(FnDeclaration* node)
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
			return;
		}
		
		auto bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entryblock", f);
		builder_.SetInsertPoint(bb);
		
		for (auto& n : node->body)
			n->accept(this);
		
		llvm::verifyFunction(*f);
	}

	void CodeGen::visit(VarDeclaration* node)
	{

	}

	void CodeGen::visit(BinaryOperator* node)
	{
		node->left->accept(this);
		auto l = pop();
		node->right->accept(this);
		auto r = pop();

		switch (node->op)
		{
		case TokenType::Plus: 
			push(builder_.CreateAdd(l, r, "addtmp"));
			break;
		case TokenType::Minus: 
			push(builder_.CreateSub(l, r, "subtmp"));
			break;
		case TokenType::Star:
			push(builder_.CreateMul(l, r, "multmp"));
			break;
		case TokenType::Divide: 
			push(builder_.CreateSDiv(l, r, "divtmp"));
			break;
		default: 
			throw TinyException("CodeGen::visit -> BinaryOperator -> default");
		}
	}

	void CodeGen::visit(Identifier* node)
	{
	}

	void CodeGen::visit(IntLiteral* node)
	{
		push(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, node->value, true)));
	}

	void CodeGen::visit(RetDeclaration* node)
	{
		node->expression->accept(this);

		builder_.CreateRet(pop());
	}

	void CodeGen::visit(CallExp* node)
	{

	}

	std::unique_ptr<llvm::Module> CodeGen::execute(AST* ast)
	{
		visit(ast);
		return std::move(module_);
	}

	void CodeGen::push(llvm::Value* v)
	{
		value_stack_.push(v);
	}

	llvm::Value* CodeGen::pop()
	{
		auto v = value_stack_.top();
		value_stack_.pop();
		return v;
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

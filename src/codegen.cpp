#include "codegen.h"

#include "ast.h"
#include "tiny_exception.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Verifier.h"

namespace tiny {

	CodeGen::CodeGen() : builder_(llvm::getGlobalContext()), module_(llvm::make_unique<llvm::Module>("tiny module", llvm::getGlobalContext()))
	{
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

		if(node->external)
		{
			module_->getOrInsertFunction(node->name, ft);
			return;
		}

		auto f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, node->name, module_.get());
		auto bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", f);
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
		auto l = value_stack_.top();
		value_stack_.pop();
		node->right->accept(this);
		auto r = value_stack_.top();
		value_stack_.pop();

		switch (node->op)
		{
		case TokenType::Plus: 
			value_stack_.push(builder_.CreateAdd(l, r, "addtmp"));
			break;
		case TokenType::Minus: 
			value_stack_.push(builder_.CreateSub(l, r, "subtmp"));
			break;
		case TokenType::Star:
			value_stack_.push(builder_.CreateSub(l, r, "multmp"));
			break;
		case TokenType::Divide: 
			value_stack_.push(builder_.CreateSub(l, r, "divtmp"));
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
		value_stack_.push(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, node->value, true)));
	}

	void CodeGen::visit(RetDeclaration* node)
	{
		node->expression->accept(this);

		builder_.CreateRet(value_stack_.top());
		value_stack_.pop();
	}

	void CodeGen::visit(CallExp* node)
	{

	}

	void CodeGen::dump_module() const
	{
		module_->dump();
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

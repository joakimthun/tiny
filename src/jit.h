#pragma once

#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/NullResolver.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/IR/Mangler.h>
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

namespace tiny {

	class OrcJit
	{
	public:
		OrcJit(llvm::TargetMachine& tm): data_layout_(tm.createDataLayout()), compile_layer_(object_layer_, llvm::orc::SimpleCompiler(tm)) {}

		void add_module(std::unique_ptr<llvm::Module> module)
		{
			std::vector<std::unique_ptr<llvm::Module>> vec;
			vec.push_back(std::move(module));
			compile_layer_.addModuleSet(std::move(vec), std::make_unique<llvm::SectionMemoryManager>(), std::make_unique<llvm::orc::NullResolver>());
		}

		template<class TSignature>
		std::function<TSignature> get_function_ptr(std::string name, bool exported_symbols_only = false)
		{
			auto symbol = compile_layer_.findSymbol(mangle(name), exported_symbols_only);
			return reinterpret_cast<TSignature*>(symbol.getAddress());
		}

	private:
		llvm::DataLayout data_layout_;
		llvm::orc::ObjectLinkingLayer<> object_layer_;
		llvm::orc::IRCompileLayer<llvm::orc::ObjectLinkingLayer<>> compile_layer_;

		std::string mangle(std::string name) const
		{
			std::string mangled_name;

			{
				llvm::raw_string_ostream ostream(mangled_name);
				llvm::Mangler::getNameWithPrefix(ostream, name, data_layout_);
			}

			return mangled_name;
		}
	};
}
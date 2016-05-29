#pragma once

#include <unordered_map>

#include "type.h"

namespace tiny {

	struct Symbol
	{
		Symbol(const std::string& n, std::unique_ptr<TinyType> t) : name(n), type(std::move(t)) {}
		std::string name;
		std::unique_ptr<TinyType> type;
	};

	class SymbolTable
	{
	public:
		SymbolTable(const SymbolTable* parent) : parent_(parent) {}

		bool has_entry(const std::string& name) const
		{
			auto it = symbols_.find(name);
			if (it != symbols_.end())
			{
				return true;
			}

			if (parent_ != nullptr)
				return parent_->has_entry(name);

			return false;
		}

		const Symbol* get_entry(const std::string& name) const
		{
			auto it = symbols_.find(name);
			if (it != symbols_.end())
			{
				return it->second.get();
			}

			if (parent_ != nullptr)
				return parent_->get_entry(name);

			return nullptr;
		}

		void add_entry(const std::string& name, std::unique_ptr<TinyType> type)
		{
			symbols_.insert(std::make_pair(name, std::make_unique<Symbol>(name, std::move(type))));
		}

	private:
		const SymbolTable* parent_;
		std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols_;
	};

}
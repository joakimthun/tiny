#pragma once

#include <unordered_map>

namespace tiny {

	template<class TValue>
	struct Symbol
	{
		Symbol(const std::string& n, std::unique_ptr<TValue> v) : name(n), value(std::move(v)) {}
		std::string name;
		std::unique_ptr<TValue> value;
	};

	template<class TValue>
	class SymbolTable
	{
	public:
		SymbolTable(SymbolTable* parent) : parent_(parent) {}

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

		const Symbol<TValue>* get_entry(const std::string& name) const
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

		void add_entry(const std::string& name, std::unique_ptr<TValue> v)
		{
		}

		void add_root_entry(const std::string& name, std::unique_ptr<TValue> v)
		{
			if (parent_ != nullptr)
				return parent_->add_root_entry(name, std::move(v));

			add_entry(name, std::move(v));
		}

	private:
		SymbolTable* parent_;
		std::unordered_map<std::string, std::unique_ptr<Symbol<TValue>>> symbols_;
	};

}
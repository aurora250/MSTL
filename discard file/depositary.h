#ifndef MSTL_DEPOSITARY_H__
#define MSTL_DEPOSITARY_H__
#include <functional>
#include <memory>
#include <string>
#include "concepts.hpp"
#include "hash_map.hpp"
using namespace MSTL::concepts;
MSTL_BEGIN_NAMESPACE__

class __deposit_node_base {
public:
	virtual ~__deposit_node_base() = 0;
};

template <typename T, typename... Args>
class deposit : public __deposit_node_base {
	using Func = std::function<T(Args...)>;
public:
	deposit(Func _func) : func_(_func) {}
	T run(Args&&... _args) { return func_(std::forward<Args>(_args)...); }
private:
	Func func_;
};

class Depositary {
	using deposit_ptr = std::shared_ptr<__deposit_node_base>;
public:
	template <typename Func>
	void register_deposit(const std::string& register_name, Func&& func) {
		deposit_map_.emplace(register_name, deposit_ptr(&std::forward<Func>(func)));
	};

	template <typename T, typename... Args>
	T excute(const std::string& _register_name, Args&&... _args) {
		if (deposit_map_.count(_register_name)) {
			auto depos_ptr = deposit_map_[_register_name];
			deposit<T, Args...>* run_depos_ptr = dynamic_cast<deposit<T, Args...>*>(depos_ptr.get());
			return run_depos_ptr->run(std::forward<Args>(_args)...);
		}
		else {
#ifdef MSTL_STATE_DEBUG__
			std::cout << "Invalid register name, return default value." << std::endl;
#endif
			return T();
		}
	}
private:
	hash_map<std::string, deposit_ptr> deposit_map_;
};

MSTL_END_NAMESPACE__

#endif // MSTL_DEPOSITARY_H__

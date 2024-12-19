#ifndef MSTL_DEPOSITARY_H__
#define MSTL_DEPOSITARY_H__
#include <functional>
#include <string>
#include <memory>
#include <future>
#include "concepts.hpp"
#include "hash_map.hpp"
using namespace MSTL::concepts;
MSTL_BEGIN_NAMESPACE__

class __base_deposit {
public:
	virtual ~__base_deposit() = 0;
};

template <NVoidT T, typename... Args>
class deposit : public __base_deposit {
	using Func = std::function<T(Args...)>;
public:
	deposit(Func _func) : func_(_func) {}
	T run(Args&&... _args) { return func_(std::forward<Args>(_args)...); }
private:
	Func func_;
};

class Depositary {
	using __base_deposit_ptr = std::shared_ptr<__base_deposit>;
public:
	template <typename Func>
	void register_deposit(const std::string& register_name, Func&& func) {
		//deposit_map_.insert(pair(_register_name, __base_deposit_ptr(new DeposT(_target))));
		deposit_map_.emplace(register_name, __base_deposit_ptr(&std::forward<Func>(func)));
	};
	template <typename Func, typename... Args>
	decltype(auto) register_deposit(const std::string& register_name, Func&& func, Args&&... args) {
		using Result = decltype(func(args));
		auto deposit = std::make_shared<std::packaged_task<Result()>>(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
		deposit_map_.emplace(register_name, __base_deposit_ptr(&std::forward<Func>(func)));
		auto res = deposit->get_future();
		return res;
	}


	template <NVoidT T, typename... Args>
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
	hash_map<std::string, __base_deposit_ptr> deposit_map_;
};

MSTL_END_NAMESPACE__

#endif // MSTL_DEPOSITARY_H__

#ifndef DEPOSITARY_H
#define DEPOSITARY_H
#include <functional>
#include <unordered_map>
#include <string>
#include "basiclib.h"

#if _HAS_CXX20    // concepts
namespace MSTL {
	using namespace MSTL::concepts;
	class _base_deposit {
	public:
		virtual ~_base_deposit();
	};

	template<NVoidT T, class... Args>
	class deposit : public _base_deposit
	{
		using Func = std::function<T(Args...)>;
	public:
		deposit(Func _func) : _func(_func) {}
		T run(Args&&... _args) { return _func(std::forward<Args>(_args)...); }
	private:
		Func _func;
	};

	class depositary
	{
		using _base_deposit_ptr = std::shared_ptr<_base_deposit>;
	public:
		template<class DeposT>
		void register_deposit(const std::string& _register_name, const DeposT& _target) {
			_deposit_map.insert(std::pair<std::string, _base_deposit_ptr>(_register_name, _base_deposit_ptr(new DeposT(_target))));
		};

		template<class T, class... Args>
		T excute(const std::string& _register_name, Args&&... _args) {
			if (_deposit_map.count(_register_name)) {
				auto depos_ptr = _deposit_map[_register_name];
				deposit<T, Args...>* run_depos_ptr = dynamic_cast<deposit<T, Args...>*>(depos_ptr.get());
				return run_depos_ptr->run(std::forward<Args>(_args)...);
			}
			else {
				std::cout << "Invalid register_name, return a default value." << std::endl;
				return T();
			}
		}
	private:
		std::unordered_map<std::string, _base_deposit_ptr> _deposit_map;
	};
}
#endif

#endif


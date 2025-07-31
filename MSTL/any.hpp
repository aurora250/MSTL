#ifndef MSTL_ANY_HPP__
#define MSTL_ANY_HPP__
#include "errorlib.hpp"
#include "utility.hpp"
#ifdef MSTL_COMPILER_GNUC__
#include <cxxabi.h>
#endif
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_DERIVED_CLASS(AnyCastError, TypeCastError, "Cast From any Type Failed.")

class any {
private:
    union storage_internal {
		constexpr storage_internal() = default;
		storage_internal(const storage_internal&) = delete;
		storage_internal& operator=(const storage_internal&) = delete;

		void* ptr_ = nullptr;
		aligned_storage_t<sizeof(ptr_), alignof(void*)> buffer_;
    };

    enum ANY_INNER_OPERATION {
        ACCESS, GET_TYPE_INFO, COPY, DESTROY, SWAP
    };

    union ArgT {
        void* obj_ptr_;
        const std::type_info* type_ptr_;
        any* any_ptr_;
    };

    template <typename T>
    struct internal_manage {
        static void manage(ANY_INNER_OPERATION, const any*, ArgT*);

        template <typename U>
        static void create(storage_internal& storage, U&& value) {
            void* ptr = &storage.buffer_;
            ::new (ptr) T(_MSTL forward<U>(value));
        }

        template <typename... Args>
        static void create(storage_internal& storage, Args&&... args) {
            void* ptr = &storage.buffer_;
            ::new (ptr) T(_MSTL forward<Args>(args)...);
        }

        static T* access(const storage_internal& storage) {
            const void* ptr = &storage.buffer_;
            return static_cast<T*>(const_cast<void*>(ptr));
        }
    };

    template <typename T>
    struct external_manage {
        static void manage(ANY_INNER_OPERATION, const any*, ArgT*);

        template<typename U>
        static void create(storage_internal& storage, U&& value) {
            storage.ptr_ = new T(_MSTL forward<U>(value));
        }
        template <typename... Args>
        static void create(storage_internal& storage, Args&&... args) {
            storage.ptr_ = new T(_MSTL forward<Args>(args)...);
        }
        static T* access(const storage_internal& storage) {
            return static_cast<T*>(storage.ptr_);
        }
    };

    template <typename T>
    using manage_t = conditional_t<
        is_nothrow_move_constructible_v<T> && sizeof(T) <= sizeof(storage_internal) && alignof(T) <= alignof(storage_internal)
        , internal_manage<T>, external_manage<T>>;

private:
    void (* manage_)(ANY_INNER_OPERATION, const any*, ArgT*);
    storage_internal storage_;

    template <typename T>
    friend const T* any_cast(const any* value) noexcept;

    template <typename T, typename... Args, typename Manager = manage_t<T>>
    void try_emplace(Args&&... args) {
	    reset();
        Manager::create(storage_, _MSTL forward<Args>(args)...);
        manage_ = &Manager::manage;
    }

    template <typename T, typename U, typename... Args, typename Manager = manage_t<T>>
    void try_emplace(std::initializer_list<U> ilist, Args&&... args) {
	    reset();
	    Manager::create(storage_, ilist, _MSTL forward<Args>(args)...);
	    manage_ = &Manager::manage;
    }

public:
    constexpr any() noexcept : manage_(nullptr) {}

    any(const any& x) {
        if (!x.has_value())
	        manage_ = nullptr;
        else {
	        ArgT arg{};
	        arg.any_ptr_ = this;
	        x.manage_(COPY, &x, &arg);
	    }
    }

    any& operator =(const any& rh) {
        *this = any(rh);
        return *this;
    }

    any(any&& x) noexcept {
        if (!x.has_value())
            manage_ = nullptr;
        else {
            ArgT arg{};
            arg.any_ptr_ = this;
            x.manage_(SWAP, &x, &arg);
        }
    }

    any& operator =(any&& rh) noexcept {
        if (!rh.has_value())
            reset();
        else if (this != &rh) {
            reset();
            ArgT arg{};
            arg.any_ptr_ = this;
            rh.manage_(SWAP, &rh, &arg);
        }
        return *this;
    }

    template <typename T, typename VT = decay_t<T>, typename Manager = manage_t<VT>,
        enable_if_t<is_copy_constructible_v<VT> && !is_same_v<inplace_construct_tag, VT> && !is_same_v<VT, any>, int> = 0>
    any(T&& value) : manage_(&Manager::manage) {
        Manager::create(storage_, _MSTL forward<T>(value));
    }

    template <typename T, typename VT = decay_t<T>,
        enable_if_t<!is_same_v<VT, any> && is_copy_constructible_v<VT>, int> = 0>
    any& operator =(T&& rh) {
        *this = any(_MSTL forward<T>(rh));
        return *this;
    }

    template <typename T, typename... Args, typename VT = decay_t<T>, typename Manager = manage_t<VT>,
        enable_if_t<conjunction_v<is_copy_constructible<VT>, is_constructible<VT, Args&&...>>, int> = 0>
    explicit any(inplace_construct_tag, Args&&... args) : manage_(&Manager::manage) {
        Manager::create(storage_, _MSTL forward<Args>(args)...);
    }

    template <typename T, typename U, typename... Args, typename VT = decay_t<T>, typename Manager = manage_t<VT>,
        enable_if_t<conjunction_v<is_copy_constructible<VT>, is_constructible<VT, std::initializer_list<U>&, Args&&...>>, int> = 0>
    explicit any(inplace_construct_tag, std::initializer_list<U> ilist, Args&&... args) : manage_(&Manager::manage) {
	    Manager::create(storage_, ilist, _MSTL forward<Args>(args)...);
    }

    ~any() {
        reset();
    }

    template <typename T, typename... Args, typename VT = decay_t<T>,
        enable_if_t<conjunction_v<is_copy_constructible<VT>, is_constructible<VT, Args&&...>>, int> = 0>
    VT emplace(Args&&... args) {
        try_emplace<VT>(_MSTL forward<Args>(args)...);
        return *manage_t<VT>::access(storage_);
    }

    template <typename T, typename U, typename... Args, typename VT = decay_t<T>,
        enable_if_t<conjunction_v<is_copy_constructible<VT>, is_constructible<VT, std::initializer_list<U>&, Args&&...>>, int> = 0>
    VT emplace(std::initializer_list<U> ilist, Args&&... args) {
        try_emplace<VT, U>(ilist, _MSTL forward<Args>(args)...);
        return *manage_t<VT>::access(storage_);
    }

    void reset() noexcept {
        if (has_value()) {
            manage_(DESTROY, this, nullptr);
            manage_ = nullptr;
        }
    }

    MSTL_NODISCARD bool has_value() const noexcept { return manage_ != nullptr; }

    MSTL_NODISCARD const std::type_info& type() const noexcept {
        if (!has_value())
            return typeid(void);
        ArgT arg{};
        manage_(GET_TYPE_INFO, this, &arg);
        return *arg.type_ptr_;
    }

    void swap(any& rh) noexcept {
        if (!has_value() && !rh.has_value()) return;
        if (has_value() && rh.has_value()) {
            if (this == &rh) return;
            any tmp;
            ArgT arg{};
            arg.any_ptr_ = &tmp;
            rh.manage_(SWAP, &rh, &arg);
            arg.any_ptr_ = &rh;
            manage_(SWAP, this, &arg);
            arg.any_ptr_ = this;
            tmp.manage_(SWAP, &tmp, &arg);
        }
        else {
            any* emp = !has_value() ? this : &rh;
            any* full = !has_value() ? &rh : this;
            ArgT arg{};
            arg.any_ptr_ = emp;
            full->manage_(SWAP, full, &arg);
        }
    }
};

template <typename T, typename... Args,
	enable_if_t<is_constructible_v<any, inplace_construct_tag, Args...>, int> = 0>
any make_any(Args&&... args) {
	return any(inplace_construct_tag{}, _MSTL forward<Args>(args)...);
}

template <typename T, typename U, typename... Args,
    enable_if_t<is_constructible_v<any, inplace_construct_tag, std::initializer_list<U>&, Args...>, int> = 0>
any make_any(std::initializer_list<U> ilist, Args&&... args) {
	return any(inplace_construct_tag{}, ilist, _MSTL forward<Args>(args)...);
}


template <typename T>
const T* any_cast(const any* value) noexcept {
    MSTL_IF_CONSTEXPR (is_object_v<T>)
        if (value) {
            using U = remove_cv_t<T>;
            MSTL_IF_CONSTEXPR (!(is_same_v<decay_t<U>, U> || is_copy_constructible_v<U>))
                return nullptr;
            if (value->manage_ == &any::manage_t<U>::manage || value->type() == typeid(T))
                return static_cast<const T*>(any::manage_t<U>::access(value->storage_));
        }
    return nullptr;
}

template <typename T>
T* any_cast(any* value) noexcept {
    return const_cast<T*>(any_cast<T>(const_cast<const any*>(value)));
}

template <typename T>
T any_cast(const any& value) {
    using U = remove_cvref_t<T>;
    static_assert(disjunction_v<is_reference<T>, is_copy_constructible<T>, is_constructible<T, const U&>>,
        "type T must be valid to cast from any.");
    auto ptr = any_cast<U>(&value);
    if (ptr)
        return static_cast<T>(*ptr);
    Exception(AnyCastError());
    return T();
}

template <typename T>
void any::internal_manage<T>::manage(const ANY_INNER_OPERATION oper, const any* value, ArgT* arg) {
    auto ptr = reinterpret_cast<const T*>(&value->storage_.buffer_);
    switch (oper) {
        case ACCESS: {
            arg->obj_ptr_ = const_cast<T*>(ptr);
            break;
        }
        case GET_TYPE_INFO: {
            arg->type_ptr_ = &typeid(T);
            break;
        }
        case COPY: {
            ::new(&arg->any_ptr_->storage_.buffer_) T(*ptr);
            arg->any_ptr_->manage_ = value->manage_;
            break;
        }
        case DESTROY: {
            ptr->~T();
            break;
        }
        case SWAP: {
            ::new(&arg->any_ptr_->storage_.buffer_) T(_MSTL move(*const_cast<T*>(ptr)));
            ptr->~T();
            arg->any_ptr_->manage_ = value->manage_;
            const_cast<any*>(value)->manage_ = nullptr;
            break;
        }
        default: break;
    }
}

template<typename T>
void any::external_manage<T>::manage(const ANY_INNER_OPERATION oper, const any* value, ArgT* arg) {
    auto ptr = static_cast<const T*>(value->storage_.ptr_);
    switch (oper) {
        case ACCESS: {
            arg->obj_ptr_ = const_cast<T*>(ptr);
            break;
        }
        case GET_TYPE_INFO: {
            arg->type_ptr_ = &typeid(T);
            break;
        }
        case COPY: {
            arg->any_ptr_->storage_.ptr_ = new T(*ptr);
            arg->any_ptr_->manage_ = value->manage_;
            break;
        }
        case DESTROY: {
            delete ptr;
            break;
        }
        case SWAP: {
            arg->any_ptr_->storage_.ptr_ = value->storage_.ptr_;
            arg->any_ptr_->manage_ = value->manage_;
            const_cast<any*>(value)->manage_ = nullptr;
            break;
        }
        default: break;
    }
}

MSTL_END_NAMESPACE__
#endif // MSTL_ANY_HPP__

#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include <initializer_list>
#include "memory.hpp"
#include "algobase.hpp"
#include "iterator.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T, typename Ref = T&, typename Ptr = T*>
struct vector_iterator {
	using iterator_category = std::random_access_iterator_tag;
	using value_type		= T;
	using pointer			= Ptr;
	using reference			= Ref;
	using difference_type	= ptrdiff_t;
	using size_type			= size_t;
	using self				= vector_iterator<T, Ref, Ptr>;
	using iterator			= vector_iterator<T, T&, T*>;
	using const_iterator	= vector_iterator<T, const T&, const T*>;

private:
	pointer ptr_;

public:
	MSTL_CONSTEXPR vector_iterator() : ptr_(nullptr) {}
	explicit MSTL_CONSTEXPR vector_iterator(pointer ptr) : ptr_(ptr) {}
	MSTL_CONSTEXPR vector_iterator(const iterator& x) noexcept : ptr_(x.ptr_) {}

	MSTL_CONSTEXPR self& operator=(const self& rh) noexcept {
		if (*this == rh) return *this;
		this->ptr_ = rh.ptr_;
		return *this;
	}

	~vector_iterator() noexcept = default;

	MSTL_NODISCARD MSTL_CONSTEXPR reference operator*() const noexcept { return *ptr_; }
	MSTL_NODISCARD MSTL_CONSTEXPR pointer operator->() const noexcept { return ptr_; }

	MSTL_CONSTEXPR self& operator ++() noexcept {
		++ptr_;
		return *this;
	}
	MSTL_CONSTEXPR self operator ++(int) noexcept {
		self temp = *this;
		++ptr_;
		return temp;
	}
	MSTL_CONSTEXPR self& operator --() noexcept {
		--ptr_;
		return *this;
	}
	MSTL_CONSTEXPR self operator --(int) noexcept {
		self temp = *this;
		--ptr_;
		return temp;
	}

	MSTL_CONSTEXPR self& operator +=(difference_type n) noexcept {
		ptr_ += n;
		return *this;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR self operator +(difference_type n) const noexcept {
		return vector_iterator(ptr_ + n);
	}
	MSTL_NODISCARD friend MSTL_CONSTEXPR self operator +(difference_type n, const self& it) {
		return it + n;
	}

	MSTL_CONSTEXPR self& operator -=(difference_type n) noexcept {
		ptr_ -= n;
		return *this;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR self operator -(difference_type n) const noexcept {
		return vector_iterator(ptr_ - n);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR difference_type operator -(const self& other) const noexcept {
		return ptr_ - other.ptr_;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR reference operator [](difference_type n) noexcept {
		return *(ptr_ + n);
	}

	MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const self& x) const noexcept {
		return ptr_ == x.ptr_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const self& x) const noexcept {
		return !(ptr_ == x.ptr_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(const self& x) const noexcept {
		return ptr_ < x.ptr_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(const self& x) const noexcept {
		return x.ptr_ < ptr_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(const self& x) const noexcept {
		return !(ptr_ > x.ptr_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(const self& x) const noexcept {
		return !(ptr_ < x.ptr_);
	}

	MSTL_NODISCARD MSTL_CONSTEXPR const value_type* unwrapped() const noexcept {
		return unfancy_maybe_null(ptr_);
	}
	MSTL_CONSTEXPR void seek_to(const value_type* iter) noexcept {
		ptr_ = refancy_maybe_null<pointer>(const_cast<value_type*>(iter));
	}
};

template <typename T, typename Alloc = standard_allocator<T>>
class vector {
#ifdef MSTL_VERSION_20__	
	static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
	static_assert(is_same_v<T, typename Alloc::value_type>, "allocator type mismatch.");
	static_assert(is_object_v<T>, "vector only contains object types.");

public:
	using value_type				= T;
	using pointer					= T*;
	using iterator					= vector_iterator<T, T&, T*>;
	using const_iterator			= vector_iterator<T, const T&, const T*>;
	using reverse_iterator			= MSTL::reverse_iterator<iterator>;
	using const_reverse_iterator	= MSTL::reverse_iterator<const_iterator>;
	using reference					= T&;
	using const_reference			= const T&;
	using size_type					= size_t;
	using difference_type			= ptrdiff_t;
	using self						= vector<T, Alloc>;
	using allocator_type			= Alloc;

private:
	pointer start_;
	pointer finish_;
	pointer end_of_storage_;
	allocator_type alloc_;

	MSTL_CONSTEXPR inline void range_check(size_type position) const noexcept {
		MSTL_DEBUG_VERIFY__(
			position < static_cast<size_type>(finish_ - start_), "vector index out of range"
		);
	}
	MSTL_CONSTEXPR inline void range_check(iterator position) const noexcept {
		MSTL_DEBUG_VERIFY__(
			(&*position) >= start_ && (&*position) <= finish_, "vector insert out of ranges."
		);
	}

	MSTL_CONSTEXPR pointer allocate_and_fill(size_type n, const T& x) {
		pointer result = alloc_.allocate(n);
		MSTL::uninitialized_fill_n(result, n, x);
		return result;
	}

	MSTL_CONSTEXPR void fill_initialize(size_type n, const T& x) {
		start_ = (allocate_and_fill)(n, x);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}

	template <typename Iterator>
		requires(forward_iterator<Iterator>)
	MSTL_CONSTEXPR pointer allocate_and_copy(size_type n, Iterator first, Iterator last) {
		pointer result = alloc_.allocate(n);
		MSTL::uninitialized_copy(&*first, &*last, result);
		return result;
	}

	template <typename Iterator>
		requires(input_iterator<Iterator>)
	MSTL_CONSTEXPR void range_initialize(Iterator first, Iterator last) {
		for (; first != last; ++first) push_back(*first);
	}

	template <typename Iterator>
		requires(forward_iterator<Iterator>)
	MSTL_CONSTEXPR void range_initialize(Iterator first, Iterator last) {
		size_type n = MSTL::distance(first, last);
		start_ = (allocate_and_copy)(n, first, last);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}

	MSTL_CONSTEXPR void deallocate() {
		if (start_) alloc_.deallocate(start_, end_of_storage_ - start_);
	}

	template <typename Iterator>
	MSTL_CONSTEXPR void range_insert(iterator position, Iterator first, Iterator last) {
		range_check(position);
		if (first == last) return;
		size_t n = MSTL::distance(first, last);
		if (size_t(end_of_storage_ - finish_) >= n) {
			const size_t elems_after = size_t(end() - position);
			iterator old_finish = end();
			if (elems_after > n) {
				MSTL::uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				MSTL::copy_backward(position, old_finish - n, old_finish);
				MSTL::copy(first, last, position);
			}
			else {
				Iterator mid = first;
				MSTL::advance(mid, elems_after);
				MSTL::uninitialized_copy(mid, last, finish_);
				finish_ += (n - elems_after);
				MSTL::uninitialized_copy(position, old_finish, finish_);
				finish_ += elems_after;
				MSTL::copy(first, mid, position);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + MSTL::max(old_size, n);
			pointer new_start = alloc_.allocate(len);
			pointer new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(begin(), position, new_start);
			new_finish = MSTL::uninitialized_copy(first, last, new_finish);
			new_finish = MSTL::uninitialized_copy(position, end(), new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}

public:
	MSTL_CONSTEXPR vector() 
		noexcept(is_nothrow_default_constructible_v<T>)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(1, T());
		finish_ = start_;
		end_of_storage_ = finish_;
	}

	MSTL_CONSTEXPR explicit vector(size_type n) 
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, T());
	}
	MSTL_CONSTEXPR explicit vector(size_type n, const T& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, value);
	}
	MSTL_CONSTEXPR explicit vector(int n, const T& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, value);
	}
	MSTL_CONSTEXPR explicit vector(long n, const T& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, value);
	}
	MSTL_CONSTEXPR explicit vector(size_type n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, MSTL::forward<T>(value));
	}
	MSTL_CONSTEXPR explicit vector(int n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, MSTL::forward<T>(value));
	}
	MSTL_CONSTEXPR explicit vector(long n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, MSTL::forward<T>(value));
	}

	MSTL_CONSTEXPR vector(const self& x) 
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		start_ = (allocate_and_copy)(x.cend() - x.cbegin(), x.cbegin(), x.cend());
		finish_ = start_ + (x.cend() - x.cbegin());
		end_of_storage_ = finish_;
	}

	MSTL_CONSTEXPR self& operator =(const self& x) {
		if (MSTL::addressof(x) == this) return *this;
		clear();
		insert(end(), x.cbegin(), x.cend());
	}

	MSTL_CONSTEXPR vector(self&& x) noexcept
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(swap)(x);
	}

	MSTL_CONSTEXPR self& operator =(self&& x) noexcept {
		if (MSTL::addressof(x) == this) return *this;
		clear();
		(swap)(x);
		return *this;
	}

	template <typename Iterator>
		requires(input_iterator<Iterator>)
	MSTL_CONSTEXPR vector(Iterator first, Iterator last)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		MSTL_DEBUG_VERIFY__(first <= last, "vector iterator-constructor out of ranges.");
		range_initialize(first, last);
	}

	MSTL_CONSTEXPR vector(std::initializer_list<T> x) 
		: vector(x.begin(), x.end()) {}

	MSTL_CONSTEXPR self& operator =(std::initializer_list<T> x) {
		if (x.size() > capacity()) {
			iterator new_ = (allocate_and_copy)(x.end() - x.begin(), x.begin(), x.end());
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_;
			end_of_storage_ = start_ + (x.end() - x.begin());
		}
		else if (size() >= x.size()) {
			iterator i = MSTL::copy(x.begin(), x.end(), begin());
			MSTL::destroy(i, finish_);
		}
		else {
			MSTL::copy(x.begin(), x.begin() + size(), start_);
			MSTL::uninitialized_copy(x.begin() + size(), x.end(), finish_);
		}
		finish_ = start_ + x.size();
		return *this;
	}

	~vector() {
		MSTL::destroy(start_, finish_);
		deallocate();
	}

	MSTL_NODISCARD MSTL_CONSTEXPR iterator begin() noexcept { return iterator(start_); }
	MSTL_NODISCARD MSTL_CONSTEXPR iterator end() noexcept { return iterator(finish_); }
	MSTL_NODISCARD MSTL_CONSTEXPR const_iterator cbegin() const noexcept {
		return const_iterator(start_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_iterator cend() const noexcept {
		return const_iterator(finish_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}
	MSTL_NODISCARD MSTL_CONSTEXPR reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	MSTL_NODISCARD MSTL_CONSTEXPR size_type size() const noexcept {
		return static_cast<size_type>(finish_ - start_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR size_type max_size() const noexcept {
		return static_cast<size_type>(-1) / sizeof(T);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR size_type capacity() const noexcept {
		return static_cast<size_type>(end_of_storage_ - start_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR bool empty() const noexcept {
		return start_ == finish_;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR pointer data() noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const pointer data() const noexcept { return start_; }

	MSTL_NODISCARD MSTL_CONSTEXPR allocator_type get_allocator() const noexcept { return allocator_type(); }

	MSTL_NODISCARD MSTL_CONSTEXPR reference front() noexcept { return *start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference front() const noexcept { return *start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR reference back() noexcept { return *(finish_ - 1); }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference back() const noexcept { return *(finish_ - 1); }

	MSTL_CONSTEXPR void reserve(size_type n) {
		MSTL_DEBUG_VERIFY__(n < max_size(), "vector reserve out of allocate bounds.");
		if (capacity() >= n) return;
		size_type new_capacity = MSTL::max(capacity() * 2, n);
		size_type old_size = size();
		pointer tmp = (allocate_and_copy)(new_capacity, start_, finish_);
		MSTL::destroy(start_, finish_);
		deallocate();
		start_ = tmp;
		finish_ = tmp + old_size;
		end_of_storage_ = start_ + new_capacity;
	}

	MSTL_CONSTEXPR void resize(size_type new_size, const T& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}
	MSTL_CONSTEXPR void resize(size_type new_size, T&& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), MSTL::forward<T>(x));
	}
	MSTL_CONSTEXPR void resize(size_type new_size) {
		resize(new_size, T());
	}

	template <typename... U>
	MSTL_CONSTEXPR void emplace(iterator position, U&&... args) {
		range_check(position);
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, *(finish_ - 1));
			++finish_;
			MSTL::copy_backward(&*position, finish_ - 2, finish_ - 1);
			MSTL::construct(&*position, MSTL::forward<U>(args)...);
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		pointer new_start = alloc_.allocate(len);
		pointer new_finish = new_start;
		new_finish = MSTL::uninitialized_copy(start_, &*position, new_start);
		MSTL::construct(new_finish, MSTL::forward<U>(args)...);
		++new_finish;
		new_finish = MSTL::uninitialized_copy(&*position, finish_, new_finish);
		MSTL::destroy(begin(), end());
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		end_of_storage_ = new_start + len;
	}

	template <typename... U>
	MSTL_CONSTEXPR void emplace_back(U&&... args) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, MSTL::forward<U>(args)...);
			++finish_;
		}
		else (emplace)(end(), MSTL::forward<U>(args)...);
	}

	MSTL_CONSTEXPR void push_back(const T& val) {
		(emplace_back)(val);
	}
	MSTL_CONSTEXPR void push_back(T&& val) {
		(emplace_back)(MSTL::forward<T>(val));
	}

	MSTL_CONSTEXPR void pop_back() noexcept {
		MSTL::destroy(finish_);
		--finish_;
	}

	MSTL_CONSTEXPR void assign(size_type n, const T& value) {
		if (n > capacity()) {
			reserve(n);
			insert(begin(), n, value);
		}
		else if (n > size()) {
			MSTL::fill(begin(), end(), value);
			finish_ = MSTL::uninitialized_fill_n(finish_, n - size(), value);
		}
		else {
			MSTL::fill_n(begin(), n, value);
			erase(begin() + n, end());
		}
	}

	template <class Iterator>
		requires(input_iterator<Iterator>)
	void assign(Iterator first, Iterator last) {
		MSTL_DEBUG_VERIFY__(first <= last, "vector assign out of ranges.");
		pointer cur = start_;
		for ( ; first != last && cur != finish_; ++first, ++cur)
			*cur = *first;
		
		if (first == last)
			erase(cur, finish_);
		else
			insert(finish_, first, last);
	}

	template <typename Iterator>
		requires(forward_iterator<Iterator>)
	MSTL_CONSTEXPR void assign(Iterator first, Iterator last) {
		MSTL_DEBUG_VERIFY__(first <= last, "vector assign out of ranges.");
		size_t n = MSTL::distance(first, last);
		if (n > capacity()) {
			clear();
			range_insert(begin(), first, last);
		}
		else if (n > size()) {
			Iterator mid = first;
			MSTL::advance(mid, size());
			MSTL::copy(first, mid, begin());
			finish_ = MSTL::uninitialized_copy(mid, last, finish_);
		}
		else {
			MSTL::copy(first, last, begin());
			erase(begin() + n, end());
		}
	}

	MSTL_CONSTEXPR void assign(const std::initializer_list<T>& il) {
		assign(il.begin(), il.end());
	}

	MSTL_CONSTEXPR iterator erase(iterator first, iterator last) 
		noexcept(is_nothrow_move_assignable_v<value_type>) {
		MSTL_DEBUG_VERIFY__(first <= last, "vector erase out of ranges.");
		pointer i = MSTL::copy(&*last, finish_, &*first);
		MSTL::destroy(i, finish_);
		finish_ = finish_ - (last - first);
		return iterator(finish_);
	}

	MSTL_CONSTEXPR iterator erase(iterator position)
		noexcept(is_nothrow_move_assignable_v<value_type>) {
		range_check(position);
		if ((position + 1) != end()) 
			MSTL::copy(&*(position + 1), finish_, &*position);
		--finish_;
		MSTL::destroy(finish_);
		return position;
	}

	MSTL_CONSTEXPR void clear() noexcept { erase(begin(), end()); }

	template <typename U = T>
	MSTL_CONSTEXPR iterator insert(iterator position, U&& x) {
		range_check(position);
		size_type n = position - begin();
		(emplace)(position, MSTL::forward<U>(x));
		return begin() + n;
	}

	MSTL_CONSTEXPR iterator insert(iterator position) {
		range_check(position);
		return insert(position, T());
	}

	template <typename Iterator>
		requires(input_iterator<Iterator>)
	MSTL_CONSTEXPR void insert(iterator position, Iterator first, Iterator last) {
		range_check(position);
		MSTL_DEBUG_VERIFY__(
			first <= last, "vector insert resource iterator out of ranges."
		);
		range_insert(position, first, last);
	}

	MSTL_CONSTEXPR void insert(iterator position, std::initializer_list<T> l) {
		range_check(position);
		range_insert(position, l.begin(), l.end());
	}

	template <typename U = T>
	MSTL_CONSTEXPR void insert(iterator position, size_type n, U&& x) {
		range_check(position);
		if (n == 0) return;
		if (size_type(end_of_storage_ - finish_) >= n) {
			const size_type elems_after = finish_ - (&*position);
			pointer old_finish = finish_;
			if (elems_after > n) {
				MSTL::uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				MSTL::copy_backward(&*position, old_finish - n, old_finish);
				MSTL::fill(position, position + n, x);
			}
			else {
				MSTL::uninitialized_fill_n(finish_, n - elems_after, x);
				finish_ += n - elems_after;
				MSTL::uninitialized_copy(&*position, old_finish, finish_);
				finish_ += elems_after;
				MSTL::fill(&*position, old_finish, x);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			pointer new_start = alloc_.allocate(len);
			pointer new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(start_, &*position, new_start);
			new_finish = MSTL::uninitialized_fill_n(new_finish, n, x);
			new_finish = MSTL::uninitialized_copy(&*position, finish_, new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}

	MSTL_CONSTEXPR void swap(self& x) noexcept {
		if (MSTL::addressof(x) == this) return;
		MSTL::swap(start_, x.start_);
		MSTL::swap(finish_, x.finish_);
		MSTL::swap(end_of_storage_, x.end_of_storage_);
	}

	MSTL_NODISCARD MSTL_CONSTEXPR const_reference at(size_type _pos) const {
		range_check(_pos);
		return *(start_ + _pos);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR reference at(size_type _pos) {
		return const_cast<reference>(
			static_cast<const self*>(this)->at(_pos)
			);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference operator [](size_type _pos) const {
		return this->at(_pos);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR reference operator [](size_type _pos) {
		return this->at(_pos);
	}
};
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return lh.size() == rh.size() && MSTL::equal(lh.cbegin(), lh.cend(), rh.cbegin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh == rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return MSTL::lexicographical_compare(lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend());
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return rh < lh;
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh < rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh > rh);
}
template <class T, class Alloc>
MSTL_CONSTEXPR void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) noexcept {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_VECTOR_HPP__

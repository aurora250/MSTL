#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T, typename Ref = T&, typename Ptr = T*>
struct vector_iterator {
#ifdef MSTL_VERSION_20__
	using iterator_category = contiguous_iterator_tag;
#else
	using iterator_category = random_access_iterator_tag;
#endif // MSTL_VERSION_20__
	using value_type		= T;
	using pointer			= Ptr;
	using reference			= Ref;
	using difference_type	= ptrdiff_t;
	using size_type			= size_t;
	using self				= vector_iterator<T, Ref, Ptr>;
	using iterator			= vector_iterator<T, T&, T*>;
	using const_iterator	= vector_iterator<T, const T&, const T*>;

private:
	pointer ptr_ = nullptr;

public:
	constexpr vector_iterator() = default;

	explicit constexpr vector_iterator(pointer ptr) : ptr_(ptr) {}

	constexpr vector_iterator(const iterator& x) noexcept : ptr_(x.ptr_) {}
	constexpr self& operator =(const iterator& rh) noexcept {
		if (MSTL::addressof(rh) == this) return *this;
		this->ptr_ = rh.ptr_;
		return *this;
	}

	constexpr vector_iterator(iterator&& x) noexcept : ptr_(x.ptr_) { x.ptr_ = nullptr; }
	constexpr self& operator =(iterator&& rh) noexcept {
		if (MSTL::addressof(rh) == this) return *this;
		this->ptr_ = rh.ptr_;
		rh.ptr_ = nullptr;
		return *this;
	}

	constexpr vector_iterator(const const_iterator& x) noexcept : ptr_(x.ptr_) {}
	constexpr self& operator =(const const_iterator& rh) noexcept {
		if (MSTL::addressof(rh) == this) return *this;
		this->ptr_ = rh.ptr_;
		return *this;
	}

	constexpr vector_iterator(const_iterator&& x) noexcept : ptr_(x.ptr_) { x.ptr_ = nullptr; }
	constexpr self& operator =(const_iterator&& rh) noexcept {
		if (MSTL::addressof(rh) == this) return *this;
		this->ptr_ = rh.ptr_;
		rh.ptr_ = nullptr;
		return *this;
	}

	MSTL_CONSTEXPR20 ~vector_iterator() noexcept = default;

	MSTL_NODISCARD constexpr reference operator *() const noexcept { return *ptr_; }
	MSTL_NODISCARD constexpr pointer operator ->() const noexcept { return ptr_; }

	constexpr self& operator ++() noexcept {
		++ptr_;
		return *this;
	}
	constexpr self operator ++(int) noexcept {
		self temp = *this;
		++ptr_;
		return temp;
	}
	constexpr self& operator --() noexcept {
		--ptr_;
		return *this;
	}
	constexpr self operator --(int) noexcept {
		self temp = *this;
		--ptr_;
		return temp;
	}

	constexpr self& operator +=(difference_type n) noexcept {
		ptr_ += n;
		return *this;
	}
	MSTL_NODISCARD constexpr self operator +(difference_type n) const noexcept {
		return self(ptr_ + n);
	}
	MSTL_NODISCARD friend constexpr self operator +(difference_type n, const self& it) {
		return it + n;
	}

	constexpr self& operator -=(difference_type n) noexcept {
		ptr_ -= n;
		return *this;
	}
	MSTL_NODISCARD constexpr self operator -(difference_type n) const noexcept {
		return self(ptr_ - n);
	}
	MSTL_NODISCARD constexpr difference_type operator -(const self& other) const noexcept {
		return ptr_ - other.ptr_;
	}

	MSTL_NODISCARD constexpr reference operator [](difference_type n) noexcept {
		return *(ptr_ + n);
	}

	MSTL_NODISCARD constexpr bool operator ==(const self& x) const noexcept {
		return ptr_ == x.ptr_;
	}
	MSTL_NODISCARD constexpr bool operator !=(const self& x) const noexcept {
		return !(ptr_ == x.ptr_);
	}
	MSTL_NODISCARD constexpr bool operator <(const self& x) const noexcept {
		return ptr_ < x.ptr_;
	}
	MSTL_NODISCARD constexpr bool operator >(const self& x) const noexcept {
		return x.ptr_ < ptr_;
	}
	MSTL_NODISCARD constexpr bool operator <=(const self& x) const noexcept {
		return !(ptr_ > x.ptr_);
	}
	MSTL_NODISCARD constexpr bool operator >=(const self& x) const noexcept {
		return !(ptr_ < x.ptr_);
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
	MSTL_BUILD_TYPE_ALIAS(T)

	using iterator					= vector_iterator<T, T&, T*>;
	using const_iterator			= vector_iterator<T, const T&, const T*>;
	using reverse_iterator			= MSTL::reverse_iterator<iterator>;
	using const_reverse_iterator	= MSTL::reverse_iterator<const_iterator>;

	using self						= vector<T, Alloc>;
	using allocator_type			= Alloc;

private:
	pointer start_ = nullptr;
	pointer finish_ = nullptr;
	compressed_pair<allocator_type, pointer> pair_{ default_construct_tag{}, nullptr };

	constexpr void range_check(const size_type position) const noexcept {
		MSTL_DEBUG_VERIFY__(
			position < static_cast<size_type>(finish_ - start_), "vector index out of ranges."
		);
	}
	constexpr void range_check(const iterator& position) const noexcept {
		MSTL_DEBUG_VERIFY__(
			&*position >= start_ && &*position <= finish_, "vector index out of ranges."
		);
	}

	constexpr pointer allocate_and_fill(size_type n, const T& x) {
		pointer result = pair_.get_base().allocate(n);
		MSTL::uninitialized_fill_n(result, n, x);
		return result;
	}

	constexpr void fill_initialize(size_type n, const T& x) {
		start_ = (allocate_and_fill)(n, x);
		finish_ = start_ + n;
		pair_.value = finish_;
	}

	template <typename Iterator>
	constexpr pointer allocate_and_copy(size_type n, Iterator first, Iterator last) {
		pointer result = pair_.get_base().allocate(n);
		MSTL::uninitialized_copy(first, last, result);
		return result;
	}

	template <typename Iterator, enable_if_t<
		is_iter_v<Iterator> && !is_fwd_iter_v<Iterator>, int> = 0>
	constexpr void range_initialize(Iterator first, Iterator last) {
		for (; first != last; ++first)
			push_back(*first);
	}
	template <typename Iterator, enable_if_t<is_fwd_iter_v<Iterator>, int> = 0>
	constexpr void range_initialize(Iterator first, Iterator last) {
		size_type n = MSTL::distance(first, last);
		start_ = (allocate_and_copy)(n, first, last);
		finish_ = start_ + n;
		pair_.value = finish_;
	}

	constexpr void deallocate() {
		if (start_) pair_.get_base().deallocate(start_, pair_.value - start_);
	}

	template <typename Iterator>
	constexpr void range_insert(iterator position, Iterator first, Iterator last) {
		range_check(position);
		if (first == last) return;
		const size_t n = MSTL::distance(first, last);
		if (static_cast<size_t>(pair_.value - finish_) >= n) {
			const auto elems_after = static_cast<size_t>(end() - position);
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
			pointer new_start = pair_.get_base().allocate(len);
			pointer new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(begin(), position, new_start);
			new_finish = MSTL::uninitialized_copy(first, last, new_finish);
			new_finish = MSTL::uninitialized_copy(position, end(), new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			pair_.value = new_start + len;
		}
	}

	template <typename Iterator, enable_if_t<!is_fwd_iter_v<Iterator>, int> = 0>
	constexpr void assign_aux(Iterator first, Iterator last) {
		const size_t n = MSTL::distance(first, last);
		MSTL_DEBUG_VERIFY__(n >= 0, "vector assign out of ranges.");
		pointer cur = start_;
		for (; first != last && cur != finish_; ++first, ++cur)
			*cur = *first;
		if (first == last)
			erase(cur, finish_);
		else
			insert(finish_, first, last);
	}
	template <typename Iterator, enable_if_t<is_fwd_iter_v<Iterator>, int> = 0>
	constexpr void assign_aux(Iterator first, Iterator last) {
		const size_t n = MSTL::distance(first, last);
		MSTL_DEBUG_VERIFY__(n >= 0, "vector assign out of ranges.");
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

public:
	constexpr vector() 
		noexcept(is_nothrow_default_constructible_v<T>) {
		(fill_initialize)(1, T());
		finish_ = start_;
		pair_.value = finish_;
	}

	constexpr explicit vector(const size_type n) {
		(fill_initialize)(n, T());
	}
	constexpr explicit vector(const size_type n, const T& value) {
		(fill_initialize)(n, value);
	}
	constexpr explicit vector(const int n, const T& value) {
		(fill_initialize)(n, value);
	}
	constexpr explicit vector(const long n, const T& value) {
		(fill_initialize)(n, value);
	}
	constexpr explicit vector(const size_type n, T&& value) {
		(fill_initialize)(n, MSTL::forward<T>(value));
	}
	constexpr explicit vector(const int n, T&& value) {
		(fill_initialize)(n, MSTL::forward<T>(value));
	}
	constexpr explicit vector(const long n, T&& value) {
		(fill_initialize)(n, MSTL::forward<T>(value));
	}

	constexpr vector(const self& x) {
		start_ = (allocate_and_copy)(x.cend() - x.cbegin(), x.cbegin(), x.cend());
		finish_ = start_ + (x.cend() - x.cbegin());
		pair_.value = finish_;
	}
	constexpr self& operator =(const self& x) {
		if (MSTL::addressof(x) == this) return *this;
		clear();
		insert(end(), x.cbegin(), x.cend());
		return *this;
	}

	constexpr vector(self&& x) noexcept {
		swap(x);
	}
	constexpr self& operator =(self&& x) noexcept {
		if (MSTL::addressof(x) == this) return *this;
		clear();
		swap(x);
		return *this;
	}

	template <typename Iterator>
	constexpr vector(Iterator first, Iterator last) {
		MSTL_DEBUG_VERIFY__(first <= last, "vector iterator-constructor out of ranges.");
		range_initialize(first, last);
	}

	constexpr vector(std::initializer_list<T> x) 
		: vector(x.begin(), x.end()) {}

	constexpr self& operator =(std::initializer_list<T> x) {
		if (x.size() > capacity()) {
			iterator new_ = (allocate_and_copy)(x.end() - x.begin(), x.begin(), x.end());
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_;
			pair_.value = start_ + (x.end() - x.begin());
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

	MSTL_CONSTEXPR20 ~vector() {
		MSTL::destroy(start_, finish_);
		deallocate();
	}

	MSTL_NODISCARD constexpr iterator begin() noexcept { return iterator(start_); }
	MSTL_NODISCARD constexpr iterator end() noexcept { return iterator(finish_); }
	MSTL_NODISCARD constexpr const_iterator cbegin() const noexcept {
		return const_iterator(start_);
	}
	MSTL_NODISCARD constexpr const_iterator cend() const noexcept {
		return const_iterator(finish_);
	}
	MSTL_NODISCARD constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}
	MSTL_NODISCARD constexpr reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}
	MSTL_NODISCARD constexpr const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}
	MSTL_NODISCARD constexpr const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	MSTL_NODISCARD constexpr size_type size() const noexcept {
		return static_cast<size_type>(finish_ - start_);
	}
	MSTL_NODISCARD constexpr size_type max_size() const noexcept {
		return static_cast<size_type>(-1) / sizeof(T);
	}
	MSTL_NODISCARD constexpr size_type capacity() const noexcept {
		return static_cast<size_type>(pair_.value - start_);
	}
	MSTL_NODISCARD constexpr bool empty() const noexcept {
		return start_ == finish_;
	}

	MSTL_NODISCARD constexpr pointer data() noexcept {
		MSTL_DEBUG_VERIFY__(!empty(), "data called on empty vector");
		return start_;
	}
	MSTL_NODISCARD constexpr const pointer data() const noexcept {
		MSTL_DEBUG_VERIFY__(!empty(), "data called on empty vector");
		return start_;
	}

	MSTL_NODISCARD constexpr allocator_type get_allocator() const noexcept { return allocator_type(); }

	MSTL_NODISCARD constexpr reference front() noexcept {
		MSTL_DEBUG_VERIFY__(!empty(), "front called on empty vector");
		return *start_; 
	}
	MSTL_NODISCARD constexpr const_reference front() const noexcept {
		MSTL_DEBUG_VERIFY__(!empty(), "front called on empty vector");
		return *start_;
	}
	MSTL_NODISCARD constexpr reference back() noexcept {
		MSTL_DEBUG_VERIFY__(!empty(), "back called on empty vector");
		return *(finish_ - 1);
	}
	MSTL_NODISCARD constexpr const_reference back() const noexcept {
		MSTL_DEBUG_VERIFY__(!empty(), "back called on empty vector");
		return *(finish_ - 1);
	}

	constexpr void reserve(const size_type n) {
		MSTL_DEBUG_VERIFY__(n < max_size(), "vector reserve out of allocate bounds.");
		if (capacity() >= n) return;
		size_type new_capacity = MSTL::max(capacity() * 2, n);
		size_type old_size = size();
		pointer tmp = (allocate_and_copy)(new_capacity, start_, finish_);
		MSTL::destroy(start_, finish_);
		deallocate();
		start_ = tmp;
		finish_ = tmp + old_size;
		pair_.value = start_ + new_capacity;
	}

	constexpr void resize(size_type new_size, const T& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}
	constexpr void resize(const size_type new_size) {
		resize(new_size, T());
	}

	template <typename... U>
	constexpr void emplace(iterator position, U&&... args) {
		range_check(position);
		if (finish_ != pair_.value) {
			MSTL::construct(finish_, *(finish_ - 1));
			++finish_;
			MSTL::copy_backward(&*position, finish_ - 2, finish_ - 1);
			MSTL::construct(&*position, MSTL::forward<U>(args)...);
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		pointer new_start = pair_.get_base().allocate(len);
		pointer new_finish = MSTL::uninitialized_copy(start_, &*position, new_start);
		MSTL::construct(new_finish, MSTL::forward<U>(args)...);
		++new_finish;
		new_finish = MSTL::uninitialized_copy(&*position, finish_, new_finish);
		MSTL::destroy(begin(), end());
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		pair_.value = new_start + len;
	}

	template <typename... U>
	constexpr void emplace_back(U&&... args) {
		if (finish_ != pair_.value) {
			MSTL::construct(finish_, MSTL::forward<U>(args)...);
			++finish_;
		}
		else (emplace)(end(), MSTL::forward<U>(args)...);
	}

	constexpr void push_back(const T& val) {
		(emplace_back)(val);
	}
	constexpr void push_back(T&& val) {
		(emplace_back)(MSTL::forward<T>(val));
	}

	constexpr void pop_back() noexcept {
		MSTL::destroy(finish_);
		--finish_;
	}

	constexpr void assign(size_type n, const T& value) {
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

	template <typename Iterator, enable_if_t<is_iter_v<Iterator>, int> = 0>
	constexpr void assign(Iterator first, Iterator last) {
		this->assign_aux(first, last);
	}

	constexpr void assign(std::initializer_list<T> l) {
		assign(l.begin(), l.end());
	}

	constexpr iterator insert(iterator position, const T& x) {
		size_type n = position - begin();
		(emplace)(position, x);
		return begin() + n;
	}
	constexpr iterator insert(iterator position, T&& x) {
		size_type n = position - begin();
		(emplace)(position, MSTL::forward<T>(x));
		return begin() + n;
	}

	constexpr iterator insert(iterator position) {
		return insert(position, T());
	}

	template <typename Iterator>
	constexpr void insert(iterator position, Iterator first, Iterator last) {
		MSTL_DEBUG_VERIFY__(
			MSTL::distance(first, last) >= 0, "vector insert resource iterator out of ranges."
		);
		range_insert(position, first, last);
	}

	constexpr void insert(iterator position, std::initializer_list<T> l) {
		range_insert(position, l.begin(), l.end());
	}
	constexpr void insert(iterator position, size_type n, const T& x) {
		range_check(position);
		if (n == 0) return;
		if (static_cast<size_type>(pair_.value - finish_) >= n) {
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
			const size_type len = old_size + MSTL::max(old_size, n);
			pointer new_start = pair_.get_base().allocate(len);
			pointer new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(start_, &*position, new_start);
			new_finish = MSTL::uninitialized_fill_n(new_finish, n, x);
			new_finish = MSTL::uninitialized_copy(&*position, finish_, new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			pair_.value = new_start + len;
		}
	}

	constexpr iterator erase(iterator first, iterator last) 
		noexcept(is_nothrow_move_assignable_v<value_type>) {
		MSTL_DEBUG_VERIFY__(MSTL::distance(first, last) >= 0, "vector erase out of ranges.");
		pointer i = MSTL::copy(&*last, finish_, &*first);
		MSTL::destroy(i, finish_);
		finish_ = finish_ - (last - first);
		return iterator(finish_);
	}

	constexpr iterator erase(iterator position)
		noexcept(is_nothrow_move_assignable_v<value_type>) {
		range_check(position);
		if (position + 1 != end())
			MSTL::copy(position + 1, end(), position);
		--finish_;
		MSTL::destroy(finish_);
		return position;
	}

	constexpr void clear() noexcept { erase(begin(), end()); }

	constexpr void swap(self& x) noexcept {
		if (MSTL::addressof(x) == this) return;
		MSTL::swap(start_, x.start_);
		MSTL::swap(finish_, x.finish_);
		pair_.swap(x.pair_);
	}

	MSTL_NODISCARD constexpr const_reference at(const size_type position) const {
		range_check(position);
		return *(start_ + position);
	}
	MSTL_NODISCARD constexpr reference at(const size_type position) {
		return const_cast<reference>(static_cast<const self*>(this)->at(position));
	}
	MSTL_NODISCARD constexpr const_reference operator [](const size_type position) const {
		return this->at(position);
	}
	MSTL_NODISCARD constexpr reference operator [](const size_type position) {
		return this->at(position);
	}
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Iterator, typename Alloc>
vector(Iterator, Iterator, Alloc = Alloc()) -> vector<iter_val_t<Iterator>, Alloc>;
#endif

template <typename T, typename Alloc>
MSTL_NODISCARD constexpr bool operator ==(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return lh.size() == rh.size() && MSTL::equal(lh.cbegin(), lh.cend(), rh.cbegin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD constexpr bool operator !=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh == rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD constexpr bool operator <(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return MSTL::lexicographical_compare(lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend());
}
template <typename T, typename Alloc>
MSTL_NODISCARD constexpr bool operator >(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return rh < lh;
}
template <typename T, typename Alloc>
MSTL_NODISCARD constexpr bool operator >=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh < rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD constexpr bool operator <=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh > rh);
}
template <typename T, typename Alloc>
constexpr void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) noexcept {
	x.swap(y);
}

MSTL_END_NAMESPACE__
#endif // MSTL_VECTOR_HPP__

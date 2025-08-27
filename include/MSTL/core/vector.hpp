#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T, typename Alloc>
class vector;

template <bool IsConst, typename Vector>
struct vector_iterator {
private:
	using container_type	= Vector;
	using iterator			= vector_iterator<false, container_type>;
	using const_iterator	= vector_iterator<true, container_type>;

public:
#ifdef MSTL_VERSION_20__
	using iterator_category = contiguous_iterator_tag;
#else
	using iterator_category = random_access_iterator_tag;
#endif // MSTL_VERSION_20__
	using value_type		= typename container_type::value_type;
	using reference			= conditional_t<IsConst, typename container_type::const_reference, typename container_type::reference>;
	using pointer			= conditional_t<IsConst, typename container_type::const_pointer, typename container_type::pointer>;
	using difference_type	= typename container_type::difference_type;
	using size_type			= typename container_type::size_type;

	using self				= vector_iterator<IsConst, container_type>;

private:
	pointer ptr_ = nullptr;
	const container_type* vec_ = nullptr;

	template <bool, typename> friend struct vector_iterator;

public:
	MSTL_CONSTEXPR20 vector_iterator() = default;
    MSTL_CONSTEXPR20 vector_iterator(pointer ptr) : ptr_(ptr) {}
	MSTL_CONSTEXPR20 vector_iterator(pointer ptr, const container_type* vec) : ptr_(ptr), vec_(vec) {}

	MSTL_CONSTEXPR20 vector_iterator(const iterator& x) noexcept
	: ptr_(const_cast<pointer>(x.ptr_)), vec_(x.vec_) {}

	MSTL_CONSTEXPR20 self& operator =(const iterator& rh) noexcept {
		if (_MSTL addressof(rh) == this) return *this;
		ptr_ = const_cast<pointer>(rh.ptr_);
		vec_ = rh.vec_;
		return *this;
	}

	MSTL_CONSTEXPR20 vector_iterator(iterator&& x) noexcept
	: ptr_(const_cast<pointer>(x.ptr_)), vec_(x.vec_) {
		x.ptr_ = nullptr;
		x.vec_ = nullptr;
	}

	MSTL_CONSTEXPR20 self& operator =(iterator&& rh) noexcept {
		if (_MSTL addressof(rh) == this) return *this;
		ptr_ = const_cast<pointer>(rh.ptr_);
		vec_ = rh.vec_;
		rh.ptr_ = nullptr;
		rh.vec_ = nullptr;
		return *this;
	}

	MSTL_CONSTEXPR20 vector_iterator(const const_iterator& x) noexcept
	: ptr_(const_cast<pointer>(x.ptr_)), vec_(x.vec_) {}

	MSTL_CONSTEXPR20 self& operator =(const const_iterator& rh) noexcept {
		if (_MSTL addressof(rh) == this) return *this;
		ptr_ = const_cast<pointer>(rh.ptr_);
		vec_ = rh.vec_;
		return *this;
	}

	MSTL_CONSTEXPR20 vector_iterator(const_iterator&& x) noexcept
	: ptr_(const_cast<pointer>(x.ptr_)), vec_(x.vec_) {
		x.ptr_ = nullptr;
		x.vec_ = nullptr;
	}

	MSTL_CONSTEXPR20 self& operator =(const_iterator&& rh) noexcept {
		if (_MSTL addressof(rh) == this) return *this;
		ptr_ = const_cast<pointer>(rh.ptr_);
		vec_ = rh.vec_;
		rh.ptr_ = nullptr;
		rh.vec_ = nullptr;
		return *this;
	}

	MSTL_CONSTEXPR20 ~vector_iterator() noexcept = default;

	MSTL_NODISCARD MSTL_CONSTEXPR20 reference operator *() const noexcept {
		MSTL_DEBUG_VERIFY(ptr_ && vec_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
		MSTL_DEBUG_VERIFY(vec_->start_ <= ptr_ && ptr_ <= vec_->finish_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
		return *ptr_;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 pointer operator ->() const noexcept {
		return &operator*();
	}

	MSTL_CONSTEXPR20 self& operator ++() noexcept {
		MSTL_DEBUG_VERIFY(ptr_ && vec_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_INCREMENT));
		MSTL_DEBUG_VERIFY(ptr_ < vec_->finish_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_INCREMENT));
		++ptr_;
		return *this;
	}

	MSTL_CONSTEXPR20 self operator ++(int) noexcept {
		self temp = *this;
		++*this;
		return temp;
	}

	MSTL_CONSTEXPR20 self& operator --() noexcept {
		MSTL_DEBUG_VERIFY(ptr_ && vec_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_DECREMENT));
		MSTL_DEBUG_VERIFY(vec_->start_ < ptr_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_DECREMENT));
		--ptr_;
		return *this;
	}

	MSTL_CONSTEXPR20 self operator --(int) noexcept {
		self temp = *this;
		--*this;
		return temp;
	}

	MSTL_CONSTEXPR20 self& operator +=(difference_type n) noexcept {
		if (n < 0) {
			MSTL_DEBUG_VERIFY((ptr_ && vec_) || n == 0, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_DECREMENT));
			MSTL_DEBUG_VERIFY(n >= vec_->start_ - ptr_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_DECREMENT));
		}
		else if (n > 0) {
			MSTL_DEBUG_VERIFY((ptr_ && vec_) || n == 0, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(vector_iterator, __MSTL_DEBUG_TAG_INCREMENT));
			MSTL_DEBUG_VERIFY(n <= vec_->finish_ - ptr_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(vector_iterator, __MSTL_DEBUG_TAG_INCREMENT));
		}
		ptr_ += n;
		return *this;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 self operator +(difference_type n) const noexcept {
		auto tmp = *this;
		tmp += n;
		return tmp;
	}

	MSTL_NODISCARD friend MSTL_CONSTEXPR20 self operator +(difference_type n, const self& it) {
		return it + n;
	}

	MSTL_CONSTEXPR20 self& operator -=(difference_type n) noexcept {
		ptr_ += -n;
		return *this;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 self operator -(difference_type n) const noexcept {
		auto tmp = *this;
		tmp -= n;
		return tmp;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 difference_type operator -(const self& x) const noexcept {
		MSTL_DEBUG_VERIFY(vec_ == x.vec_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(vector_iterator));
		return static_cast<difference_type>(ptr_ - x.ptr_);
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 reference operator [](difference_type n) noexcept {
		return *(*this + n);
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(const self& x) const noexcept {
		MSTL_DEBUG_VERIFY(vec_ == x.vec_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(vector_iterator));
		return ptr_ == x.ptr_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(const self& x) const noexcept {
		return !(*this == x);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <(const self& x) const noexcept {
		MSTL_DEBUG_VERIFY(vec_ == x.vec_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(vector_iterator));
		return ptr_ < x.ptr_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >(const self& x) const noexcept {
		return x < *this;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <=(const self& x) const noexcept {
		return !(*this > x);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >=(const self& x) const noexcept {
		return !(*this < x);
	}
};

template <typename T, typename Alloc = allocator<T>>
class vector {
#ifdef MSTL_VERSION_20__
	static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
	static_assert(is_same_v<T, typename Alloc::value_type>, "allocator type mismatch.");
	static_assert(is_object_v<T>, "vector only contains object types.");

public:
	MSTL_BUILD_TYPE_ALIAS(T)
	using allocator_type			= Alloc;
	using self						= vector<T, Alloc>;

	using iterator					= vector_iterator<false, self>;
	using const_iterator			= vector_iterator<true, self>;
	using reverse_iterator			= _MSTL reverse_iterator<iterator>;
	using const_reverse_iterator	= _MSTL reverse_iterator<const_iterator>;

private:
	pointer start_ = nullptr;
	pointer finish_ = nullptr;
	compressed_pair<allocator_type, pointer> pair_{ _MSTL_TAG default_construct_tag{}, nullptr };

	template <bool, typename> friend struct vector_iterator;

private:
	MSTL_CONSTEXPR20 void range_check(const size_type position) const noexcept {
		MSTL_DEBUG_VERIFY(
			position < static_cast<size_type>(finish_ - start_), "vector index out of ranges."
		);
	}

	MSTL_CONSTEXPR20 pointer allocate_and_fill(size_type n, T&& x) {
		pointer result = pair_.get_base().allocate(n);
		_MSTL uninitialized_fill_n(result, n, _MSTL forward<T>(x));
		return result;
	}

	MSTL_CONSTEXPR20 void fill_initialize(size_type n, T&& x) {
		start_ = this->allocate_and_fill(n, _MSTL forward<T>(x));
		finish_ = start_ + n;
		pair_.value = finish_;
	}

	template <typename Iterator>
	MSTL_CONSTEXPR20 pointer allocate_and_copy(size_type n, Iterator first, Iterator last) {
		pointer result = pair_.get_base().allocate(n);
			_MSTL uninitialized_copy(first, last, result);
		return result;
	}

	template <typename Iterator, enable_if_t<
		is_iter_v<Iterator> && !is_ranges_fwd_iter_v<Iterator>, int> = 0>
	MSTL_CONSTEXPR20 void range_initialize(Iterator first, Iterator last) {
		for (; first != last; ++first)
			push_back(*first);
	}
	template <typename Iterator, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
	MSTL_CONSTEXPR20 void range_initialize(Iterator first, Iterator last) {
		size_type n = _MSTL distance(first, last);
		start_ = (allocate_and_copy)(n, first, last);
		finish_ = start_ + n;
		pair_.value = finish_;
	}

	MSTL_CONSTEXPR20 void deallocate() {
		if (start_) pair_.get_base().deallocate(start_, pair_.value - start_);
	}

	template <typename Iterator>
	MSTL_CONSTEXPR20 void range_insert(iterator position, Iterator first, Iterator last) {
		if (first == last) return;
		const size_t n = _MSTL distance(first, last);
		if (static_cast<size_t>(pair_.value - finish_) >= n) {
			const auto elems_after = static_cast<size_t>(end() - position);
			iterator old_finish = end();
			if (elems_after > n) {
				_MSTL uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				_MSTL copy_backward(position, old_finish - n, old_finish);
				_MSTL copy(first, last, position);
			}
			else {
				Iterator mid = first;
				_MSTL advance(mid, elems_after);
				_MSTL uninitialized_copy(mid, last, finish_);
				finish_ += (n - elems_after);
				_MSTL uninitialized_copy(position, old_finish, finish_);
				finish_ += elems_after;
				_MSTL copy(first, mid, position);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + _MSTL max(old_size, n);
			pointer new_start = pair_.get_base().allocate(len);
			pointer new_finish = new_start;
			new_finish = _MSTL uninitialized_copy(begin(), position, new_start);
			new_finish = _MSTL uninitialized_copy(first, last, new_finish);
			new_finish = _MSTL uninitialized_copy(position, end(), new_finish);
			_MSTL destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			pair_.value = new_start + len;
		}
	}

	template <typename Iterator, enable_if_t<!is_ranges_fwd_iter_v<Iterator>, int> = 0>
	MSTL_CONSTEXPR20 void assign_aux(Iterator first, Iterator last) {
		const size_t n = _MSTL distance(first, last);
		MSTL_DEBUG_VERIFY(n >= 0, "vector assign out of ranges.");
		pointer cur = start_;
		for (; first != last && cur != finish_; ++first, ++cur)
			*cur = *first;
		if (first == last)
			erase(cur, finish_);
		else
			insert(finish_, first, last);
	}

	template <typename Iterator, enable_if_t<is_ranges_fwd_iter_v<Iterator>, int> = 0>
	MSTL_CONSTEXPR20 void assign_aux(Iterator first, Iterator last) {
		const size_t n = _MSTL distance(first, last);
		MSTL_DEBUG_VERIFY(n >= 0, "vector assign out of ranges.");
		if (n > capacity()) {
			clear();
			range_insert(begin(), first, last);
		}
		else if (n > size()) {
			Iterator mid = first;
			_MSTL advance(mid, size());
			_MSTL copy(first, mid, begin());
			finish_ = _MSTL uninitialized_copy(mid, last, finish_);
		}
		else {
			_MSTL copy(first, last, begin());
			erase(begin() + n, end());
		}
	}

public:
	MSTL_CONSTEXPR20 vector()
		noexcept(is_nothrow_default_constructible_v<T>) {
		this->fill_initialize(1, _MSTL move(T()));
		finish_ = start_;
		pair_.value = finish_;
	}

	MSTL_CONSTEXPR20 explicit vector(const size_type n) {
		this->fill_initialize(n, _MSTL move(T()));
	}
	MSTL_CONSTEXPR20 explicit vector(const size_type n, const T& value) {
		this->fill_initialize(n, value);
	}
	MSTL_CONSTEXPR20 explicit vector(const int n, const T& value) {
		this->fill_initialize(n, value);
	}
	MSTL_CONSTEXPR20 explicit vector(const long n, const T& value) {
		this->fill_initialize(n, value);
	}
	MSTL_CONSTEXPR20 explicit vector(const size_type n, T&& value) {
		this->fill_initialize(n, _MSTL forward<T>(value));
	}
	MSTL_CONSTEXPR20 explicit vector(const int n, T&& value) {
		this->fill_initialize(n, _MSTL forward<T>(value));
	}
	MSTL_CONSTEXPR20 explicit vector(const long n, T&& value) {
		this->fill_initialize(n, _MSTL forward<T>(value));
	}

	MSTL_CONSTEXPR20 vector(const self& x) {
		start_ = (allocate_and_copy)(x.cend() - x.cbegin(), x.cbegin(), x.cend());
		finish_ = start_ + (x.cend() - x.cbegin());
		pair_.value = finish_;
	}
	MSTL_CONSTEXPR20 self& operator =(const self& x) {
		if (_MSTL addressof(x) == this) return *this;
		clear();
		insert(end(), x.cbegin(), x.cend());
		return *this;
	}

	MSTL_CONSTEXPR20 vector(self&& x) noexcept {
		swap(x);
	}
	MSTL_CONSTEXPR20 self& operator =(self&& x) noexcept {
		if (_MSTL addressof(x) == this) return *this;
		clear();
		swap(x);
		return *this;
	}

	template <typename Iterator>
	MSTL_CONSTEXPR20 vector(Iterator first, Iterator last) {
		MSTL_DEBUG_VERIFY(first <= last, "vector iterator-constructor out of ranges.");
		range_initialize(first, last);
	}

	MSTL_CONSTEXPR20 vector(std::initializer_list<T> x)
		: vector(x.begin(), x.end()) {}

	MSTL_CONSTEXPR20 self& operator =(std::initializer_list<T> x) {
		if (x.size() > capacity()) {
			iterator new_ = (allocate_and_copy)(x.end() - x.begin(), x.begin(), x.end());
			_MSTL destroy(start_, finish_);
			deallocate();
			start_ = new_;
			pair_.value = start_ + (x.end() - x.begin());
		}
		else if (size() >= x.size()) {
			iterator i = _MSTL copy(x.begin(), x.end(), begin());
			_MSTL destroy(i, finish_);
		}
		else {
			_MSTL copy(x.begin(), x.begin() + size(), start_);
			_MSTL uninitialized_copy(x.begin() + size(), x.end(), finish_);
		}
		finish_ = start_ + x.size();
		return *this;
	}

	MSTL_CONSTEXPR20 ~vector() {
		clear();
		deallocate();
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 iterator begin() noexcept { return iterator(start_, this); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 iterator end() noexcept { return iterator(finish_, this); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_iterator begin() const noexcept { return cbegin(); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_iterator end() const noexcept { return cend(); }
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_iterator cbegin() const noexcept { return const_iterator(start_, this); }
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_iterator cend() const noexcept { return const_iterator(finish_, this); }
	MSTL_NODISCARD MSTL_CONSTEXPR20 reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reverse_iterator rend() const noexcept { return crend(); }
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

	MSTL_NODISCARD MSTL_CONSTEXPR20 size_type size() const noexcept {
		return static_cast<size_type>(finish_ - start_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 size_type max_size() const noexcept {
		return static_cast<size_type>(-1) / sizeof(T);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 size_type capacity() const noexcept {
		return static_cast<size_type>(pair_.value - start_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 bool empty() const noexcept {
		return start_ == finish_;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 pointer data() noexcept {
		MSTL_DEBUG_VERIFY(!empty(), "data called on empty vector");
		return start_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_pointer data() const noexcept {
		MSTL_DEBUG_VERIFY(!empty(), "data called on empty vector");
		return start_;
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 allocator_type get_allocator() const noexcept { return allocator_type(); }

	MSTL_NODISCARD MSTL_CONSTEXPR20 reference front() noexcept {
		MSTL_DEBUG_VERIFY(!empty(), "front called on empty vector");
		return *start_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference front() const noexcept {
		MSTL_DEBUG_VERIFY(!empty(), "front called on empty vector");
		return *start_;
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 reference back() noexcept {
		MSTL_DEBUG_VERIFY(!empty(), "back called on empty vector");
		return *(finish_ - 1);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference back() const noexcept {
		MSTL_DEBUG_VERIFY(!empty(), "back called on empty vector");
		return *(finish_ - 1);
	}

	MSTL_CONSTEXPR20 void reserve(const size_type n) {
		MSTL_DEBUG_VERIFY(n < max_size(), "vector reserve out of allocate bounds.");
		if (capacity() >= n) return;
		size_type new_capacity = _MSTL max(capacity() * 2, n);
		size_type old_size = size();
		pointer tmp = (allocate_and_copy)(new_capacity, start_, finish_);
		_MSTL destroy(start_, finish_);
		deallocate();
		start_ = tmp;
		finish_ = tmp + old_size;
		pair_.value = start_ + new_capacity;
	}

	MSTL_CONSTEXPR20 void resize(size_type new_size, const T& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}
	MSTL_CONSTEXPR20 void resize(const size_type new_size) {
		resize(new_size, T());
	}

	template <typename... U>
	MSTL_CONSTEXPR20 void emplace(iterator position, U&&... args) {
		if (finish_ != pair_.value) {
			_MSTL construct(finish_, _MSTL move(*(finish_ - 1)));
			++finish_;
			_MSTL move_backward(position, _MSTL prev(end(), -2), _MSTL prev(end()));
			_MSTL construct(&*position, _MSTL forward<U>(args)...);
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		pointer new_start = pair_.get_base().allocate(len);
		pointer new_finish = _MSTL uninitialized_move(begin(), position, new_start);
		_MSTL construct(new_finish, _MSTL forward<U>(args)...);
		++new_finish;
		new_finish = _MSTL uninitialized_move(position, end(), new_finish);
		_MSTL destroy(begin(), end());
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		pair_.value = new_start + len;
	}

	template <typename... U>
	MSTL_CONSTEXPR20 void emplace_back(U&&... args) {
		if (finish_ != pair_.value) {
			_MSTL construct(finish_, _MSTL forward<U>(args)...);
			++finish_;
		}
		else this->emplace(end(), _MSTL forward<U>(args)...);
	}

	MSTL_CONSTEXPR20 void push_back(const T& val) {
	    if (finish_ != pair_.value) {
	        _MSTL construct(finish_, val);
	        ++finish_;
	    }
	    else this->emplace(end(), val);
	}
	MSTL_CONSTEXPR20 void push_back(T&& val) {
		this->emplace_back(_MSTL move(val));
	}

	MSTL_CONSTEXPR20 void pop_back() noexcept {
		_MSTL destroy(finish_);
		--finish_;
	}

	MSTL_CONSTEXPR20 void assign(size_type n, const T& value) {
		if (n > capacity()) {
			reserve(n);
			insert(begin(), n, value);
		}
		else if (n > size()) {
			_MSTL fill(begin(), end(), value);
			finish_ = _MSTL uninitialized_fill_n(finish_, n - size(), value);
		}
		else {
			_MSTL fill_n(begin(), n, value);
			erase(begin() + n, end());
		}
	}

	template <typename Iterator, enable_if_t<is_iter_v<Iterator>, int> = 0>
	MSTL_CONSTEXPR20 void assign(Iterator first, Iterator last) {
		this->assign_aux(first, last);
	}

	MSTL_CONSTEXPR20 void assign(std::initializer_list<T> l) {
		assign(l.begin(), l.end());
	}

	MSTL_CONSTEXPR20 iterator insert(iterator position, const T& x) {
		size_type n = position - begin();
		(emplace)(position, x);
		return begin() + n;
	}
	MSTL_CONSTEXPR20 iterator insert(iterator position, T&& x) {
		size_type n = position - begin();
		(emplace)(position, _MSTL forward<T>(x));
		return begin() + n;
	}

	MSTL_CONSTEXPR20 iterator insert(iterator position) {
		return insert(position, T());
	}

	template <typename Iterator>
	MSTL_CONSTEXPR20 void insert(iterator position, Iterator first, Iterator last) {
		MSTL_DEBUG_VERIFY(
			distance(first, last) >= 0, "vector insert resource iterator out of ranges."
		);
		range_insert(position, first, last);
	}

	MSTL_CONSTEXPR20 void insert(iterator position, std::initializer_list<T> l) {
		range_insert(position, l.begin(), l.end());
	}
	MSTL_CONSTEXPR20 void insert(iterator position, size_type n, const T& x) {
		if (n == 0) return;
		if (static_cast<size_type>(pair_.value - finish_) >= n) {
			const size_type elems_after = _MSTL distance(begin(), position);
			iterator old_finish = end();
			if (elems_after > n) {
				_MSTL uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				_MSTL copy_backward(position, old_finish - n, old_finish);
				_MSTL fill(position, position + n, x);
			}
			else {
				_MSTL uninitialized_fill_n(finish_, n - elems_after, x);
				finish_ += n - elems_after;
				_MSTL uninitialized_copy(position, old_finish, end());
				finish_ += elems_after;
				_MSTL fill(position, old_finish, x);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + _MSTL max(old_size, n);
			pointer new_start = pair_.get_base().allocate(len);
			pointer new_finish = new_start;
			new_finish = _MSTL uninitialized_copy(begin(), position, new_start);
			new_finish = _MSTL uninitialized_fill_n(new_finish, n, x);
			new_finish = _MSTL uninitialized_copy(position, end(), new_finish);
			_MSTL destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			pair_.value = new_start + len;
		}
	}

	MSTL_CONSTEXPR20 iterator erase(iterator first, iterator last)
		noexcept(is_nothrow_move_assignable_v<value_type>) {
		MSTL_DEBUG_VERIFY(_MSTL distance(first, last) >= 0, "vector erase out of ranges.");

	    const auto elems_after = end() - last;
	    if (elems_after > 0) {
	        _MSTL move(last, end(), first);
	    }
	    pointer new_finish = finish_ - (last - first);
	    _MSTL destroy(new_finish, finish_);
	    finish_ = new_finish;
	    return first;
	}

	MSTL_CONSTEXPR20 iterator erase(iterator position)
		noexcept(is_nothrow_move_assignable_v<value_type>) {
		if (position + 1 != end())
			_MSTL copy(position + 1, end(), position);
		--finish_;
		_MSTL destroy(finish_);
		return position;
	}

	MSTL_CONSTEXPR20 void clear() noexcept {
	    if (empty()) return;
	    _MSTL destroy(start_, finish_);
	    finish_ = start_;
	}

	MSTL_CONSTEXPR20 void shrink_to_fit() {
		if (capacity() == size()) return;
		if (size() == 0) {
			deallocate();
			start_ = finish_ = pair_.value = nullptr;
			return;
		}
		pointer new_start = pair_.get_base().allocate(size());
		pointer new_finish = _MSTL uninitialized_copy(start_, finish_, new_start);
		_MSTL destroy(start_, finish_);
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		pair_.value = new_start + size();
	}

	MSTL_CONSTEXPR20 void swap(self& x) noexcept {
		if (_MSTL addressof(x) == this) return;
		_MSTL swap(start_, x.start_);
		_MSTL swap(finish_, x.finish_);
		pair_.swap(x.pair_);
	}

	MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference at(const size_type position) const {
		range_check(position);
		return *(start_ + position);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 reference at(const size_type position) {
		return const_cast<reference>(static_cast<const self*>(this)->at(position));
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference operator [](const size_type position) const {
		return this->at(position);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR20 reference operator [](const size_type position) {
		return this->at(position);
	}
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Iterator, typename Alloc>
vector(Iterator, Iterator, Alloc = Alloc()) -> vector<iter_val_t<Iterator>, Alloc>;
#endif

template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return lh.size() == rh.size() && _MSTL equal(lh.cbegin(), lh.cend(), rh.cbegin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh == rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return _MSTL lexicographical_compare(lh.cbegin(), lh.cend(), rh.cbegin(), rh.cend());
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return rh < lh;
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh < rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <=(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return !(lh > rh);
}
template <typename T, typename Alloc>
MSTL_CONSTEXPR20 void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) noexcept {
	x.swap(y);
}

MSTL_END_NAMESPACE__
#endif // MSTL_VECTOR_HPP__

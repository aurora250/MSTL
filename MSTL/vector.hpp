#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include <initializer_list>
#include "memory.hpp"
#include "algobase.hpp"
#include "iterator.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename T, typename Alloc = default_standard_alloc<T>>
class vector {
public:
	typedef T					value_type;
	typedef T*					pointer;
	typedef T*					iterator;
	typedef const T*			const_iterator;
	typedef T&					reference;
	typedef const T&			const_reference;
	typedef size_t				size_type;
	typedef ptrdiff_t			difference_type;
	typedef vector<T, Alloc>	self;
private:
	typedef Alloc data_allocator;
	typedef std::_Rebind_alloc_t<Alloc, T> alloc_type;

	iterator start_;
	iterator finish_;
	iterator end_of_storage_;
	data_allocator alloc_;

	MSTL_CONSTEXPR void range_check(size_type _pos) const {
		MSTL_DEBUG_VERIFY__(
			_pos < static_cast<size_type>(finish_ - start_), "vector index out of range"
		);
	}
	MSTL_CONSTEXPR bool in_boundary(size_type _pos) const noexcept {
		return _pos < size() ? true : false;
	}
	MSTL_CONSTEXPR void fill_initialize(size_type n, T&& x) {
		start_ = allocate_and_fill(n, std::forward<T>(x));
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}
	MSTL_CONSTEXPR iterator allocate_and_fill(size_type n, T&& x) {
		iterator result = alloc_.allocate(n);
		MSTL::uninitialized_fill_n(result, n, std::forward<T>(x));
		return result;
	}
	template <typename Iterator>
		requires(ForwardIterator<Iterator>)
	MSTL_CONSTEXPR iterator allocate_and_copy(size_type n, Iterator first, Iterator last) {
		iterator result = alloc_.allocate(n);
		MSTL::uninitialized_copy(first, last, result);
		return result;
	}
	template <typename Iterator>
		requires(InputIterator<Iterator>)
	MSTL_CONSTEXPR void range_initialize(Iterator first, Iterator last) {
		for (; first != last; ++first) push_back(*first);
	}
	template <typename Iterator>
		requires(ForwardIterator<Iterator>)
	MSTL_CONSTEXPR void range_initialize(Iterator first, Iterator last) {
		size_type n = 0;
		MSTL::distance(first, last, n);
		start_ = allocate_and_copy(n, first, last);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}
	MSTL_CONSTEXPR void deallocate() {
		if (start_) alloc_.deallocate(start_, end_of_storage_ - start_);
	}
	MSTL_CONSTEXPR void insert_aux(iterator position, T&& x) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, *(finish_ - 1));
			++finish_;
			MSTL::copy_backward(position, finish_ - 2, finish_ - 1);
			*position = x;
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = alloc_.allocate(len);
		iterator new_finish = new_start;
		new_finish = MSTL::uninitialized_copy(start_, position, new_start);
		MSTL::construct(new_finish, std::forward<T>(x));
		++new_finish;
		new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
		MSTL::destroy(begin(), end());
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		end_of_storage_ = new_start + len;
	}
	MSTL_CONSTEXPR void insert_aux(iterator position, const T& x) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, *(finish_ - 1));
			++finish_;
			MSTL::copy_backward(position, finish_ - 2, finish_ - 1);
			*position = x;
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = alloc_.allocate(len);
		iterator new_finish = new_start;
		new_finish = MSTL::uninitialized_copy(start_, position, new_start);
		MSTL::construct(new_finish, x);
		++new_finish;
		new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
		MSTL::destroy(begin(), end());
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		end_of_storage_ = new_start + len;
	}
	template <typename Iterator>
		requires(InputIterator<Iterator>)
	MSTL_CONSTEXPR void range_insert(iterator pos, Iterator first, Iterator last) {
		for (; first != last; ++first) {
			pos = this->insert(pos, *first);
			++pos;
		}
	}
	template <typename Iterator>
		requires(ForwardIterator<Iterator>)
	MSTL_CONSTEXPR void range_insert(iterator position, Iterator first, Iterator last) {
		if (first == last) return;
		size_type n = 0;
		MSTL::distance(first, last, n);
		if (size_type(end_of_storage_ - finish_) >= n) {
			const size_type elems_after = finish_ - position;
			iterator old_finish = finish_;
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
				finish_ += n - elems_after;
				MSTL::uninitialized_copy(position, old_finish, finish_);
				finish_ += elems_after;
				MSTL::copy(first, mid, position);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + maximum(old_size, n);
			iterator new_start = alloc_.allocate(len);
			iterator new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(start_, position, new_start);
			new_finish = MSTL::uninitialized_copy(first, last, new_finish);
			new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}
public:
	MSTL_CONSTEXPR vector() noexcept(NothrowDefaultConstructible<T>)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		fill_initialize(1, T());
		finish_ = start_;
		end_of_storage_ = finish_;
	}

	MSTL_CONSTEXPR explicit vector(size_type n) 
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		fill_initialize(n, T());
	}
	MSTL_CONSTEXPR explicit vector(size_type n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		fill_initialize(n, std::forward<T>(value));
	}
	MSTL_CONSTEXPR explicit vector(int n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		fill_initialize(n, std::forward<T>(value));
	}
	MSTL_CONSTEXPR explicit vector(long n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		fill_initialize(n, std::forward<T>(value));
	}

	MSTL_CONSTEXPR vector(const self& x) 
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		start_ = (allocate_and_copy)(x.const_end() - x.const_begin(), x.const_begin(), x.const_end());
		finish_ = start_ + (x.const_end() - x.const_begin());
		end_of_storage_ = finish_;
	}
	MSTL_CONSTEXPR self& operator =(const self& x) {
		if (x == *this) return *this;
		clear();
		insert(end(), x.const_begin(), x.const_end());
	}
	MSTL_CONSTEXPR vector(self&& x) noexcept
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		swap(std::forward<self>(x));
	}
	MSTL_CONSTEXPR self& operator =(self&& x) noexcept {
		if (x == *this) return *this;
		clear();
		swap(std::forward<self>(x));
		return *this;
	}

	MSTL_CONSTEXPR vector(std::initializer_list<T>&& x) 
		: vector(x.begin(), x.end()) {}
	MSTL_CONSTEXPR self& operator =(std::initializer_list<T>&& x) {
		if (x == *this) return *this;
		if (x.size() > capacity()) {
			iterator new_ = (allocate_and_copy)(x.end() - x.begin(), x.begin(), x.end());
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_;
			end_of_storage_ = start_ + (x.end() - x.begin());
		}
		else if (size() >= x.size()) {
			iterator i = copy(x.begin(), x.end(), begin());
			MSTL::destroy(i, finish_);
		}
		else {
			MSTL::copy(x.begin(), x.begin() + size(), start_);
			MSTL::uninitialized_copy(x.begin() + size(), x.end(), finish_);
		}
		finish_ = start_ + x.size();
		return *this;
	}

	template <typename Iterator>
		requires(InputIterator<Iterator>)
	MSTL_CONSTEXPR vector(Iterator first, Iterator last) 
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		range_initialize(first, last);
	}

	~vector() {
		MSTL::destroy(start_, finish_);
		deallocate();
	}

	MSTL_NODISCARD MSTL_CONSTEXPR iterator begin() noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_iterator const_begin() const noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR iterator end() noexcept { return finish_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_iterator const_end() const noexcept { return finish_; }

	MSTL_NODISCARD MSTL_CONSTEXPR size_type size() const noexcept { return size_type(finish_ - start_); }
	MSTL_NODISCARD MSTL_CONSTEXPR size_type capacity() const noexcept { return size_type(end_of_storage_ - start_); }
	MSTL_NODISCARD MSTL_CONSTEXPR bool empty() const noexcept { return start_ == finish_; }

	MSTL_NODISCARD MSTL_CONSTEXPR T* data() noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const T* data() const noexcept { return start_; }

	MSTL_NODISCARD MSTL_CONSTEXPR reference front() noexcept { return *start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference front() const noexcept { return *start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR reference back() noexcept { return *(finish_ - 1); }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference back() const noexcept { return *(finish_ - 1); }

	MSTL_CONSTEXPR void reserve(size_type n) {
		if (capacity() > n) return;
		size_type old_size = size();
		iterator tmp = allocate_and_copy(n, start_, finish_);
		MSTL::destroy(start_, finish_);
		deallocate();
		start_ = tmp;
		finish_ = tmp + old_size;
		end_of_storage_ = start_ + n;
	}
	MSTL_CONSTEXPR void resize(size_type new_size, T&& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), std::forward<T>(x));
	}
	MSTL_CONSTEXPR void resize(size_type new_size) {
		resize(new_size, T());
	}
	MSTL_CONSTEXPR void push_back(T&& val) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, std::forward<T>(val));
			++finish_;
		}
		else insert_aux(end(), std::forward<T>(val));
	}
	MSTL_CONSTEXPR void push_back(const T& val) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, val);
			++finish_;
		}
		else insert_aux(end(), val);
	}
	MSTL_CONSTEXPR void pop_back() noexcept {
		MSTL::destroy(finish_);
		--finish_;
	}
	MSTL_CONSTEXPR iterator erase(iterator first, iterator last) 
		noexcept(NothrowMoveAssignable<value_type>) {
		iterator i = MSTL::copy(last, finish_, first);
		MSTL::destroy(i, finish_);
		finish_ = finish_ - (last - first);
		return finish_;
	}
	MSTL_CONSTEXPR iterator erase(iterator position)
		noexcept(NothrowMoveAssignable<value_type>) {
		if (position + 1 != end()) MSTL::copy(position + 1, finish_, position);
		--finish_;
		MSTL::destroy(finish_);
		return position;
	}
	MSTL_CONSTEXPR void clear() noexcept { erase(begin(), end()); }
	MSTL_CONSTEXPR iterator insert(iterator position, T&& x) {
		size_type n = position - begin();
		if (finish_ != end_of_storage_ && position == end()) {
			MSTL::construct(finish_, std::forward<T>(x));
			++finish_;
		}
		else insert_aux(position, std::forward<T>(x));
		return begin() + n;
	}
	MSTL_CONSTEXPR iterator insert(iterator position) {
		return this->insert(position, T());
	}
	template <typename Iterator>
		requires(InputIterator<Iterator>)
	MSTL_CONSTEXPR void insert(iterator position, Iterator first, Iterator last) {
		this->range_insert(position, first, last);
	}
	MSTL_CONSTEXPR void insert(iterator position, std::initializer_list<T> l) {
		this->range_insert(position, l.begin(), l.end());
	}
	MSTL_CONSTEXPR void insert(iterator position, size_type n, T&& x) {
		if (n == 0) return;
		if (size_type(end_of_storage_ - finish_) >= n) {
			const size_type elems_after = finish_ - position;
			iterator old_finish = finish_;
			if (elems_after > n) {
				MSTL::uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				MSTL::copy_backward(position, old_finish - n, old_finish);
				MSTL::fill(position, position + n, std::forward<T>(x));
			}
			else {
				MSTL::uninitialized_fill_n(finish_, n - elems_after, std::forward<T>(x));
				finish_ += n - elems_after;
				MSTL::uninitialized_copy(position, old_finish, finish_);
				finish_ += elems_after;
				MSTL::fill(position, old_finish, std::forward<T>(x));
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + maximum(old_size, n);
			iterator new_start = alloc_.allocate(len);
			iterator new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(start_, position, new_start);
			new_finish = MSTL::uninitialized_fill_n(new_finish, n, std::forward<T>(x));
			new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}
	MSTL_CONSTEXPR void swap(self&& x) noexcept {
		std::swap(start_, x.start_);
		std::swap(finish_, x.finish_);
		std::swap(end_of_storage_, x.end_of_storage_);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference at(size_type _pos) const noexcept {
		range_check(_pos);
		return *(start_ + _pos);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR reference at(size_type _pos) noexcept {
		return const_cast<reference>(
			static_cast<const self*>(this)->at(_pos)
			);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference operator [](size_type _pos) const noexcept {
		return this->at(_pos);
	}
	MSTL_NODISCARD MSTL_CONSTEXPR reference operator [](size_type _pos) noexcept {
		return this->at(_pos);
	}
};
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return lh.size() == rh.size() && MSTL::equal(lh.const_begin(), lh.const_end(), rh.const_begin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(const vector<T, Alloc>& lh, const vector<T, Alloc>& rh) {
	return MSTL::lexicographical_compare(lh.const_begin(), lh.const_end(), rh.const_begin(), rh.const_end());
}
template <class T, class Alloc>
MSTL_CONSTEXPR void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) noexcept {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_VECTOR_HPP__

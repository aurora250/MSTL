#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include <initializer_list>
#include "memory.hpp"
#include "algobase.hpp"
#include "iterator.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <typename T, typename Alloc = standard_allocator<T>>
class vector {
public:
	typedef T					value_type;
	typedef T*					pointer;
	typedef T*					iterator;
	typedef const T*			const_iterator;
	typedef MSTL::reverse_iterator<iterator> reverse_iterator;
	typedef MSTL::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef T&					reference;
	typedef const T&			const_reference;
	typedef size_t				size_type;
	typedef ptrdiff_t			difference_type;
	typedef vector<T, Alloc>	self;
private:
	typedef Alloc alloc_type;

	iterator start_;
	iterator finish_;
	iterator end_of_storage_;
	alloc_type alloc_;

	MSTL_CONSTEXPR void range_check(size_type _pos) const {
		MSTL_DEBUG_VERIFY__(
			_pos < static_cast<size_type>(finish_ - start_), "vector index out of range"
		);
	}
	template <typename U = T>
	MSTL_CONSTEXPR iterator allocate_and_fill(size_type n, U&& x) {
		iterator result = alloc_.allocate(n);
		MSTL::uninitialized_fill_n(result, n, std::forward<U>(x));
		return result;
	}
	template <typename U = T>
	MSTL_CONSTEXPR void fill_initialize(size_type n, U&& x) {
		start_ = (allocate_and_fill)(n, std::forward<U>(x));
		finish_ = start_ + n;
		end_of_storage_ = finish_;
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
		size_type n = MSTL::distance(first, last);
		start_ = (allocate_and_copy)(n, first, last);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}
	MSTL_CONSTEXPR void deallocate() {
		if (start_) alloc_.deallocate(start_, end_of_storage_ - start_);
	}
	template <typename... U>
	MSTL_CONSTEXPR void emplace_aux(iterator position, U&&... args) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, *(finish_ - 1));
			++finish_;
			MSTL::copy_backward(position, finish_ - 2, finish_ - 1);
			MSTL::construct(position, std::forward<U>(args)...);
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = alloc_.allocate(len);
		iterator new_finish = new_start;
		new_finish = MSTL::uninitialized_copy(start_, position, new_start);
		MSTL::construct(new_finish, std::forward<U>(args)...);
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
	MSTL_CONSTEXPR void range_insert(iterator position, Iterator first, Iterator last) {
		for (; first != last; ++first) {
			position = insert(position, *first);
			++position;
		}
	}
	template <typename Iterator>
		requires(ForwardIterator<Iterator>)
	MSTL_CONSTEXPR void range_insert(iterator position, Iterator first, Iterator last) {
		if (first == last) return;
		size_type n = MSTL::distance(first, last);
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
		(fill_initialize)(n, std::forward<T>(value));
	}
	MSTL_CONSTEXPR explicit vector(int n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, std::forward<T>(value));
	}
	MSTL_CONSTEXPR explicit vector(long n, T&& value)
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		(fill_initialize)(n, std::forward<T>(value));
	}

	MSTL_CONSTEXPR vector(const self& x) 
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		start_ = (allocate_and_copy)(x.cend() - x.cbegin(), x.cbegin(), x.cend());
		finish_ = start_ + (x.cend() - x.cbegin());
		end_of_storage_ = finish_;
	}
	MSTL_CONSTEXPR self& operator =(const self& x) {
		if (std::addressof(x) == this) return *this;
		clear();
		insert(end(), x.cbegin(), x.cend());
	}
	MSTL_CONSTEXPR vector(self&& x) noexcept
		: start_(0), finish_(0), end_of_storage_(0), alloc_() {
		swap(x);
	}
	MSTL_CONSTEXPR self& operator =(self&& x) noexcept {
		if (std::addressof(x) == this) return *this;
		clear();
		swap(x);
		return *this;
	}

	MSTL_CONSTEXPR vector(const std::initializer_list<T>& x) 
		: vector(x.begin(), x.end()) {}
	MSTL_CONSTEXPR self& operator =(const std::initializer_list<T>& x) {
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
	MSTL_NODISCARD MSTL_CONSTEXPR iterator end() noexcept { return finish_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_iterator cbegin() const noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_iterator cend() const noexcept { return finish_; }
	MSTL_NODISCARD MSTL_CONSTEXPR reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
	MSTL_NODISCARD MSTL_CONSTEXPR reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}
	MSTL_NODISCARD MSTL_CONSTEXPR const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	MSTL_NODISCARD MSTL_CONSTEXPR size_type size() const noexcept { return size_type(finish_ - start_); }
	MSTL_NODISCARD MSTL_CONSTEXPR size_type capacity() const noexcept { return size_type(end_of_storage_ - start_); }
	MSTL_NODISCARD MSTL_CONSTEXPR bool empty() const noexcept { return start_ == finish_; }

	MSTL_NODISCARD MSTL_CONSTEXPR T* data() noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const T* data() const noexcept { return start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR alloc_type get_allocator() const noexcept { return alloc_; }

	MSTL_NODISCARD MSTL_CONSTEXPR reference front() noexcept { return *start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference front() const noexcept { return *start_; }
	MSTL_NODISCARD MSTL_CONSTEXPR reference back() noexcept { return *(finish_ - 1); }
	MSTL_NODISCARD MSTL_CONSTEXPR const_reference back() const noexcept { return *(finish_ - 1); }

	MSTL_CONSTEXPR void reserve(size_type n) {
		if (capacity() > n) return;
		size_type old_size = size();
		iterator tmp = (allocate_and_copy)(n, start_, finish_);
		MSTL::destroy(start_, finish_);
		deallocate();
		start_ = tmp;
		finish_ = tmp + old_size;
		end_of_storage_ = start_ + n;
	}
	MSTL_CONSTEXPR void resize(size_type new_size, const T& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}
	MSTL_CONSTEXPR void resize(size_type new_size, T&& x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), std::forward<T>(x));
	}
	MSTL_CONSTEXPR void resize(size_type new_size) {
		resize(new_size, T());
	}
	MSTL_CONSTEXPR void push_back(const T& val) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, val);
			++finish_;
		}
		else (emplace_aux)(end(), val);
	}
	MSTL_CONSTEXPR void push_back(T&& val) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, std::forward<T>(val));
			++finish_;
		}
		else (emplace_aux)(end(), std::forward<T>(val));
	}
	MSTL_CONSTEXPR void pop_back() noexcept {
		MSTL::destroy(finish_);
		--finish_;
	}
	template <typename... U>
	MSTL_CONSTEXPR void emplace_back(U&&... args) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, std::forward<U>(args)...);
			++finish_;
		}
		else (emplace_aux)(end(), std::forward<U>(args)...);
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
	template <typename U = T>
	MSTL_CONSTEXPR iterator insert(iterator position, U&& x) {
		size_type n = position - begin();
		if (finish_ != end_of_storage_ && position == end()) {
			MSTL::construct(finish_, std::forward<U>(x));
			++finish_;
		}
		else (emplace_aux)(position, std::forward<U>(x));
		return begin() + n;
	}
	MSTL_CONSTEXPR iterator insert(iterator position) {
		return insert(position, T());
	}
	template <typename Iterator>
		requires(InputIterator<Iterator>)
	MSTL_CONSTEXPR void insert(iterator position, Iterator first, Iterator last) {
		range_insert(position, first, last);
	}
	MSTL_CONSTEXPR void insert(iterator position, std::initializer_list<T> l) {
		range_insert(position, l.begin(), l.end());
	}
	template <typename U = T>
	MSTL_CONSTEXPR void insert(iterator position, size_type n, U&& x) {
		if (n == 0) return;
		if (size_type(end_of_storage_ - finish_) >= n) {
			const size_type elems_after = finish_ - position;
			iterator old_finish = finish_;
			if (elems_after > n) {
				MSTL::uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				MSTL::copy_backward(position, old_finish - n, old_finish);
				MSTL::fill(position, position + n, std::forward<U>(x));
			}
			else {
				MSTL::uninitialized_fill_n(finish_, n - elems_after, std::forward<U>(x));
				finish_ += n - elems_after;
				MSTL::uninitialized_copy(position, old_finish, finish_);
				finish_ += elems_after;
				MSTL::fill(position, old_finish, std::forward<U>(x));
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + maximum(old_size, n);
			iterator new_start = alloc_.allocate(len);
			iterator new_finish = new_start;
			new_finish = MSTL::uninitialized_copy(start_, position, new_start);
			new_finish = MSTL::uninitialized_fill_n(new_finish, n, std::forward<U>(x));
			new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}
	MSTL_CONSTEXPR void swap(self& x) noexcept {
		std::swap(start_, x.start_);
		std::swap(finish_, x.finish_);
		std::swap(end_of_storage_, x.end_of_storage_);
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
MSTL_CONSTEXPR void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) noexcept(noexcept(x.swap(y))) {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_VECTOR_HPP__

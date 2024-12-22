#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include <initializer_list>
#include "memory.hpp"
#include "algobase.hpp"
#include "iterator.hpp"

MSTL_BEGIN_NAMESPACE__

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

	iterator start_;
	iterator finish_;
	iterator end_of_storage_;
	data_allocator alloc_;

	inline void range_check(size_type _pos) const {
		Exception(in_boundary(_pos), RangeError());
	}
	inline bool in_boundary(size_type _pos) const {
		if (_pos < 0) return false;
		else return _pos < size() ? true : false;
	}
	void fill_initialize(size_type n, const_reference value) {
		start_ = allocate_and_fill(n, value);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}
	iterator allocate_and_fill(size_type n, const_reference x) {
		iterator result = alloc_.allocate(n);
		MSTL_TRY__{
			MSTL::uninitialized_fill_n(result, n, x);
		}
		MSTL_CATCH_UNWIND_THROW_M__(alloc_.deallocate(result, n));
		return result;
	}
	template <typename ForwardIterator>
	iterator allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last) {
		iterator result = alloc_.allocate(n);
		MSTL_TRY__{
			MSTL::uninitialized_copy(first, last, result);
		}
		MSTL_CATCH_UNWIND_THROW_M__(alloc_.deallocate(result, n));
		return result;
	}
	template <typename InputIterator>
	void range_initialize(InputIterator first, InputIterator last, std::input_iterator_tag) {
		for (; first != last; ++first) push_back(*first);
	}
	template <typename ForwardIterator>
	void range_initialize(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag) {
		size_type n = 0;
		MSTL::distance(first, last, n);
		start_ = allocate_and_copy(n, first, last);
		finish_ = start_ + n;
		end_of_storage_ = finish_;
	}
	void deallocate() {
		if (start_) alloc_.deallocate(start_, end_of_storage_ - start_);
	}
	void insert_aux(iterator position, const_reference x) {
		if (finish_ != end_of_storage_) {
			construct(finish_, *(finish_ - 1));
			++finish_;
			value_type x_copy = x;
			MSTL::copy_backward(position, finish_ - 2, finish_ - 1);
			*position = x_copy;
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = alloc_.allocate(len);
		iterator new_finish = new_start;
		MSTL_TRY__{
			new_finish = MSTL::uninitialized_copy(start_, position, new_start);
			MSTL::construct(new_finish, x);
			++new_finish;
			new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
		}
		MSTL_CATCH_UNWIND_THROW_M__(
			MSTL::destroy(new_start, new_finish);
			alloc_.deallocate(new_start, len));
		MSTL::destroy(begin(), end());
		deallocate();
		start_ = new_start;
		finish_ = new_finish;
		end_of_storage_ = new_start + len;
	}
	template <typename InputIterator>
	void range_insert(iterator pos, InputIterator first,
					  InputIterator last, std::input_iterator_tag) {
		for (; first != last; ++first) {
			pos = insert(pos, *first);
			++pos;
		}
	}
	template <typename ForwardIterator>
	void range_insert(iterator position, ForwardIterator first,
					  ForwardIterator last, std::forward_iterator_tag) {
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
				ForwardIterator mid = first;
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
			MSTL_TRY__{
			  new_finish = MSTL::uninitialized_copy(start_, position, new_start);
			  new_finish = MSTL::uninitialized_copy(first, last, new_finish);
			  new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
			}
			MSTL_CATCH_UNWIND_THROW_M__(
				MSTL::destroy(new_start, new_finish);
				alloc_.deallocate(new_start, len));
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}
public:
	vector() : start_(0), finish_(0), end_of_storage_(0), alloc_() {
		fill_initialize(1, T());
		finish_ = start_;
		end_of_storage_ = finish_;
	}
	vector(size_type n) {
		fill_initialize(n, T());
	}
	vector(size_type n, const_reference value) {
		fill_initialize(n, value);
	}
	explicit vector(int n, const_reference value) {
		fill_initialize(n, value);
	}
	explicit vector(long n, const_reference value) {
		fill_initialize(n, value);
	}
	explicit vector(MSTL_LONG_LONG_TYPE__ n, const_reference value) {
		fill_initialize(n, value);
	}
	vector(const vector<T, Alloc>& x) {
		start_ = allocate_and_copy(x.const_end() - x.const_begin(), x.const_begin(), x.const_end());
		finish_ = start_ + (x.const_end() - x.const_begin());
		end_of_storage_ = finish_;
	}
	explicit vector(const std::initializer_list<T>& l) : vector(l.begin(), l.end()) {}
	template <typename InputIterator>
	vector(InputIterator first, InputIterator last) : start_(), finish_(), end_of_storage_() {
		range_initialize(first, last, (MSTL::iterator_category)(first));
	}
	self& operator =(const self& x) {
		if (x == *this) return *this;
		if (x.size() > capacity()) {
			iterator _new = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = _new;
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
	~vector() {
		MSTL::destroy(start_, finish_);
		deallocate();
	}

	iterator begin() { return start_; }
	iterator end() { return finish_; }
	const_iterator const_begin() const { return start_; }
	const_iterator const_end() const { return finish_; }
	size_type size() const { return size_type(finish_ - start_); }
	size_type capacity() const { return size_type(end_of_storage_ - start_); }
	bool empty() const { return start_ == finish_; }

	bool operator ==(const self& rh) const {
		return size() == rh.size() && equal(const_begin(), const_end(), rh.const_begin());
	}
	bool operator <(const self& rh) const {
		return lexicographical_compare(const_begin(), const_end(), rh.const_begin(), rh.const_end());
	}
	reference operator[](size_type n) { 
		range_check(n);
		return *(start_ + n); 
	}
	const_reference operator[](size_type n) const {
		range_check(n);
		return *(start_ + n); 
	}

	reference front() { return *start_; }
	const_reference front() const { return *start_; }
	reference back() { return *(finish_ - 1); }
	const_reference back() const { return *(finish_ - 1); }

	void reserve(size_type n) {
		if (capacity() > n) return;
		const size_type old_size = size();
		iterator tmp = allocate_and_copy(n, start_, finish_);
		MSTL::destroy(start_, finish_);
		deallocate();
		start_ = tmp;
		finish_ = tmp + old_size;
		end_of_storage_ = start_ + n;
	}
	void resize(size_type new_size, const_reference x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}
	void resize(size_type new_size) {
		resize(new_size, T());
	}
	void push_back(const_reference val) {
		if (finish_ != end_of_storage_) {
			MSTL::construct(finish_, val);
			++finish_;
		}
		else insert_aux(end(), val);
	}
	void pop_back() {
		MSTL::destroy(finish_);
		--finish_;
	}
	iterator erase(iterator first, iterator last) {
		iterator i = copy(last, finish_, first);
		destroy(i, finish_);
		finish_ = finish_ - (last - first);
		return finish_;
	}
	iterator erase(iterator position) {
		if (position + 1 != end()) MSTL::copy(position + 1, finish_, position);
		--finish_;
		destory(finish_);
		return position;
	}
	void clear() { erase(begin(), end()); }
	iterator insert(iterator position, const T& x) {
		size_type n = position - begin();
		if (finish_ != end_of_storage_ && position == end()) {
			MSTL::construct(finish_, x);
			++finish_;
		}
		else insert_aux(position, x);
		return begin() + n;
	}
	iterator insert(iterator position) {
		return insert(position, T());
	}
	template <typename InputIterator>
	void insert(iterator position, InputIterator first, InputIterator last) {
		range_insert(position, first, last, (iterator_category)(first));
	}
	void insert(iterator position, size_type n, const_reference x) {
		if (n == 0) return;
		if (size_type(end_of_storage_ - finish_) >= n) {
			value_type x_copy = x;
			const size_type elems_after = finish_ - position;
			iterator old_finish = finish_;
			if (elems_after > n) {
				MSTL::uninitialized_copy(finish_ - n, finish_, finish_);
				finish_ += n;
				MSTL::copy_backward(position, old_finish - n, old_finish);
				MSTL::fill(position, position + n, x_copy);
			}
			else {
				MSTL::uninitialized_fill_n(finish_, n - elems_after, x_copy);
				finish_ += n - elems_after;
				MSTL::uninitialized_copy(position, old_finish, finish_);
				finish_ += elems_after;
				MSTL::fill(position, old_finish, x_copy);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + maximum(old_size, n);
			iterator new_start = alloc_.allocate(len);
			iterator new_finish = new_start;
			MSTL_TRY__{
			  new_finish = MSTL::uninitialized_copy(start_, position, new_start);
			  new_finish = MSTL::uninitialized_fill_n(new_finish, n, x);
			  new_finish = MSTL::uninitialized_copy(position, finish_, new_finish);
			}
			MSTL_CATCH_UNWIND_THROW_M__(
				MSTL::destroy(new_start, new_finish);
				alloc_.deallocate(new_start, len));
			MSTL::destroy(start_, finish_);
			deallocate();
			start_ = new_start;
			finish_ = new_finish;
			end_of_storage_ = new_start + len;
		}
	}
	void swap(self& x) {
		std::swap(start_, x.start_);
		std::swap(finish_, x.finish_);
		std::swap(end_of_storage_, x.end_of_storage_);
	}
};
template <class T, class Alloc>
inline void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_VECTOR_HPP__

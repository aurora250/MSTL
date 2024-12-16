#ifndef MSTL_VECTOR_HPP__
#define MSTL_VECTOR_HPP__
#include <initializer_list>
#include "memory.hpp"
#include "errorlib.h"
#include "container.h"
#include "algobase.hpp"

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

	static const char* __type__;
private:
	typedef Alloc data_allocator;

	iterator start;
	iterator finish;
	iterator end_of_storage;
	data_allocator alloc;

	inline void __range_check(size_type _pos) const {
		Exception(__in_boundary(_pos), new RangeError());
	}
	inline bool __in_boundary(size_type _pos) const {
		if (_pos < 0) return false;
		else return _pos < size() ? true : false;
	}
	void fill_initialize(size_type n, const_reference value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_storage = finish;
	}
	iterator allocate_and_fill(size_type n, const_reference x) {
		iterator result = alloc.allocate(n);
		MSTL_TRY__{
			MSTL::uninitialized_fill_n(result, n, x);
		}
		MSTL_CATCH_UNWIND_THROW_M__(alloc.deallocate(result, n));
		return result;
	}
	template <typename ForwardIterator>
	iterator allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last) {
		iterator result = alloc.allocate(n);
		MSTL_TRY__{
			MSTL::uninitialized_copy(first, last, result);
		}
		MSTL_CATCH_UNWIND_THROW_M__(alloc.deallocate(result, n));
		return result;
	}
	template <typename InputIterator>
	void range_initialize(InputIterator first, InputIterator last, std::input_iterator_tag) {
		for (; first != last; ++first) push_back(*first);
	}
	template <typename ForwardIterator>
	void range_initialize(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag) {
		size_type n = 0;
		distance(first, last, n);
		start = allocate_and_copy(n, first, last);
		finish = start + n;
		end_of_storage = finish;
	}
	void deallocate() {
		if (start) alloc.deallocate(start, end_of_storage - start);
	}
	void insert_aux(iterator position, const_reference x) {
		if (finish != end_of_storage) {
			construct(finish, *(finish - 1));
			++finish;
			value_type x_copy = x;
			copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
			return;
		}
		const size_type old_size = size();
		const size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = alloc.allocate(len);
		iterator new_finish = new_start;
		MSTL_TRY__{
			new_finish = uninitialized_copy(start, position, new_start);
			construct(new_finish, x);
			++new_finish;
			new_finish = uninitialized_copy(position, finish, new_finish);
		}
		MSTL_CATCH_UNWIND_THROW_M__(
			destroy(new_start, new_finish);
			alloc.deallocate(new_start, len));
		destroy(begin(), end());
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + len;
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
		distance(first, last, n);
		if (size_type(end_of_storage - finish) >= n) {
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				copy(first, last, position);
			}
			else {
				ForwardIterator mid = first;
				advance(mid, elems_after);
				uninitialized_copy(mid, last, finish);
				finish += n - elems_after;
				uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				copy(first, mid, position);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = alloc.allocate(len);
			iterator new_finish = new_start;
			MSTL_TRY__{
			  new_finish = uninitialized_copy(start, position, new_start);
			  new_finish = uninitialized_copy(first, last, new_finish);
			  new_finish = uninitialized_copy(position, finish, new_finish);
			}
			MSTL_CATCH_UNWIND_THROW_M__(
				destroy(new_start, new_finish);
				alloc.deallocate(new_start, len));
			destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
public:
	vector() : start(0), finish(0), end_of_storage(0), alloc() {
		fill_initialize(1, T());
		finish = start;
		end_of_storage = finish;
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
	explicit vector(const self& x) {
		start = allocate_and_copy(x.const_end() - x.const_begin(), x.const_begin(), x.const_end());
		finish = start + (x.const_end() - x.const_begin());
		end_of_storage = finish;
	}
	explicit vector(const std::initializer_list<T>& l) : vector(l.begin(), l.end()) {}
	template <typename InputIterator>
	vector(InputIterator first, InputIterator last) : start(), finish(), end_of_storage() {
		range_initialize(first, last, iterator_category(first));
	}
	self& operator =(const self& x) {
		if (x == *this) return *this;
		if (x.size() > capacity()) {
			iterator _new = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
			destroy(start, finish);
			deallocate();
			start = _new;
			end_of_storage = start + (x.end() - x.begin());
		}
		else if (size() >= x.size()) {
			iterator i = copy(x.begin(), x.end(), begin());
			destroy(i, finish);
		}
		else {
			copy(x.begin(), x.begin() + size(), start);
			uninitialized_copy(x.begin() + size(), x.end(), finish);
		}
		finish = start + x.size();
		return *this;
	}
	~vector() {
		MSTL::destroy(start, finish);
		deallocate();
	}

	iterator begin() { return start; }
	iterator end() { return finish; }
	const_iterator const_begin() const { return start; }
	const_iterator const_end() const { return finish; }
	size_type size() const { return size_type(finish - start); }
	size_type capacity() const { return size_type(end_of_storage - start); }
	bool empty() const { return start == finish; }

	bool operator ==(const self& rh) const {
		return size() == rh.size() && equal(const_begin(), const_end(), rh.const_begin());
	}
	bool operator <(const self& rh) const {
		return lexicographical_compare(const_begin(), const_end(), rh.const_begin(), rh.const_end());
	}
	reference operator[](size_type n) { 
		__range_check(n);
		return *(start + n); 
	}
	const_reference operator[](size_type n) const {
		__range_check(n);
		return *(start + n); 
	}

	reference front() { return *start; }
	const_reference front() const { return *start; }
	reference back() { return *(finish - 1); }
	const_reference back() const { return *(finish - 1); }

	void reserve(size_type n) {
		if (capacity() > n) return;
		const size_type old_size = size();
		iterator tmp = allocate_and_copy(n, start, finish);
		MSTL::destroy(start, finish);
		deallocate();
		start = tmp;
		finish = tmp + old_size;
		end_of_storage = start + n;
	}
	void resize(size_type new_size, const_reference x) {
		if (new_size < size()) erase(begin() + new_size, end());
		else insert(end(), new_size - size(), x);
	}
	void resize(size_type new_size) {
		resize(new_size, T());
	}
	void push_back(const_reference val) {
		if (finish != end_of_storage) {
			MSTL::construct(finish, val);
			++finish;
		}
		else insert_aux(end(), val);
	}
	void pop_back() {
		MSTL::destroy(finish);
		--finish;
	}
	iterator erase(iterator first, iterator last) {
		iterator i = copy(last, finish, first);
		destroy(i, finish);
		finish = finish - (last - first);
		return finish;
	}
	iterator erase(iterator position) {
		if (position + 1 != end()) MSTL::copy(position + 1, finish, position);
		--finish;
		destory(finish);
		return position;
	}
	void clear() { erase(begin(), end()); }
	iterator insert(iterator position, const T& x) {
		size_type n = position - begin();
		if (finish != end_of_storage && position == end()) {
			MSTL::construct(finish, x);
			++finish;
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
		if (size_type(end_of_storage - finish) >= n) {
			value_type x_copy = x;
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				MSTL::uninitialized_copy(finish - n, finish, finish);
				finish += n;
				MSTL::copy_backward(position, old_finish - n, old_finish);
				MSTL::fill(position, position + n, x_copy);
			}
			else {
				MSTL::uninitialized_fill_n(finish, n - elems_after, x_copy);
				finish += n - elems_after;
				MSTL::uninitialized_copy(position, old_finish, finish);
				finish += elems_after;
				MSTL::fill(position, old_finish, x_copy);
			}
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size + max(old_size, n);
			iterator new_start = alloc.allocate(len);
			iterator new_finish = new_start;
			MSTL_TRY__{
			  new_finish = MSTL::uninitialized_copy(start, position, new_start);
			  new_finish = MSTL::uninitialized_fill_n(new_finish, n, x);
			  new_finish = MSTL::uninitialized_copy(position, finish, new_finish);
			}
			MSTL_CATCH_UNWIND_THROW_M__(
				MSTL::destroy(new_start, new_finish);
				alloc.deallocate(new_start, len));
			MSTL::destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
	void swap(self& x) {
		std::swap(start, x.start);
		std::swap(finish, x.finish);
		std::swap(end_of_storage, x.end_of_storage);
	}
};
template <typename T, typename Alloc>
const char* vector<T, Alloc>::__type__ = "vector";

template <class T, class Alloc>
inline void swap(vector<T, Alloc>& x, vector<T, Alloc>& y) {
	x.swap(y);
}

MSTL_END_NAMESPACE__

#endif // MSTL_VECTOR_HPP__

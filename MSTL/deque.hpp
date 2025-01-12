#ifndef MSTL_DEQUE_HPP__
#define MSTL_DEQUE_HPP__
#include "iterator.hpp"
#include "memory.hpp"
#include "algobase.hpp"
#include "mathlib.h"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

MSTL_CONSTEXPR size_t deque_buf_size(size_t n, size_t sz) noexcept {
    return n != 0 ? n : (sz < size_t(power(2, 9)) ? size_t(size_t(power(2, 9)) / sz) : 1);
}

template <typename T, typename Ref = T&, typename Ptr = T*, size_t BufSize = 0>
struct deque_iterator {
    // iterator_traits:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T                               value_type;
    typedef Ptr                             pointer;
    typedef Ref                             reference;
    typedef ptrdiff_t                       difference_type;

    typedef size_t                          size_type;
    typedef T*                              link_type;
    typedef link_type*                      map_pointer;
    typedef deque_iterator<T, Ref, Ptr, BufSize>            self;
    typedef deque_iterator<T, T&, T*, BufSize>              iterator;
    typedef deque_iterator<T, const T&, const T*, BufSize>  const_iterator;

    link_type cur_;
    link_type first_;
    link_type last_;
    map_pointer node_;

    MSTL_CONSTEXPR static size_t buff_size() noexcept {
        return deque_buf_size(BufSize, sizeof(T));
    }
    void set_node(map_pointer new_node) {
        node_ = new_node;
        first_ = *new_node;
        last_ = first_ + difference_type(buff_size());
    }

    deque_iterator(link_type cur = nullptr, link_type first = nullptr,
        link_type last = nullptr, map_pointer node = nullptr) noexcept :
        cur_(cur), first_(first), last_(last), node_(node) {}
    deque_iterator(const iterator& x) noexcept :
        cur_(x.cur_), first_(x.first_), last_(x.last_), node_(x.node_) {}
    self& operator =(const self& rh) noexcept {
		if(*this == rh) return *this;
		this->cur_ = rh.cur_;
		this->first_ = rh.first_;
		this->last_ = rh.last_;
		this->node_ = rh.node_;
		return *this;
	}
    ~deque_iterator() noexcept = default;

    MSTL_NODISCARD reference operator *() const noexcept { return *cur_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }
    difference_type operator -(const self& x) const noexcept {
        return (node_ - x.node_ - 1) * buff_size() + (cur_ - first_) + (x.last_ - x.cur_);
    }
    self& operator ++() noexcept {
        ++cur_;
        if (cur_ == last_) {
            set_node(node_ + 1);
            cur_ = first_;
        }
        return *this;
    }
    self operator ++(int) noexcept {
        self temp = *this;
        ++*this;
        return temp;
    }
    self& operator --() noexcept {
        if (cur_ == first_) {
            set_node(node_ - 1);
            cur_ = last_;
        }
        --cur_;
        return *this;
    }
    self operator --(int) noexcept {
        self temp = *this;
        --*this;
        return temp;
    }
    self& operator +=(difference_type n) noexcept {
        difference_type offset = n + (cur_ - first_);
        if (offset >= 0 && offset < difference_type(buff_size()))
            cur_ += n;
        else {
            difference_type node_offset = offset > 0 ? 
                offset / difference_type(buff_size())
                : -difference_type((-offset - 1) / buff_size()) - 1;
            set_node(node_ + node_offset);
            cur_ = first_ + (offset - node_offset * difference_type(buff_size()));
        }
        return *this;
    }
    self& operator -=(difference_type n) noexcept { return *this += -n; }
    MSTL_NODISCARD self operator +(difference_type n) const noexcept {
        self temp = *this;
        return temp += n;
    }
    MSTL_NODISCARD self operator -(difference_type n) const noexcept {
        self temp = *this;
        return temp -= n;
    }
    MSTL_NODISCARD reference operator [](difference_type n) noexcept { return *(*this + n); }

    MSTL_NODISCARD bool operator ==(const self& x) const noexcept { return cur_ == x.cur_; }
    MSTL_NODISCARD bool operator !=(const self& x) const noexcept { return !(*this == x); }
    MSTL_NODISCARD bool operator <(const self& x) const noexcept {
        return node_ == x.node_ ? (cur_ < x.cur_) : (node_ < x.node_);
    }
    MSTL_NODISCARD bool operator >(const self& x) const noexcept { return x < (*this); }
    MSTL_NODISCARD bool operator <=(const self& x) const noexcept { return !(*this > x); }
    MSTL_NODISCARD bool operator >=(const self& x) const noexcept { return !(*this < x); }
};

template<typename T, typename Alloc = default_standard_alloc<T>, size_t BufSize = 0>
class deque {
public:
    typedef T                           value_type;
    typedef value_type*                 pointer;
    typedef const value_type*           const_pointer;
    typedef value_type&                 reference;
    typedef const value_type&           const_reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;
    typedef deque<T, Alloc, BufSize>    self;
    typedef deque_iterator<T, T&, T*, BufSize>              iterator;
    typedef deque_iterator<T, const T&, const T*, BufSize>  const_iterator;
private:
    typedef pointer* map_pointer;
    typedef Alloc    data_allocator;
    typedef default_standard_alloc<pointer>  map_allocator;

    data_allocator data_alloc_;
    map_allocator map_alloc_;
    map_pointer map_;
    size_type map_size_;
    iterator start_;
    iterator finish_;

    MSTL_CONSTEXPR static size_t buff_size() {
        return deque_buf_size(BufSize, sizeof(T));
    }
    void create_map_and_nodes(size_type n) {
        size_type node_nums = n / buff_size() + 1;
        map_size_ = maximum(size_type(8), node_nums + 2);
        map_ = map_alloc_.allocate(map_size_);
        map_pointer nstart = map_ + (map_size_ - node_nums) / 2;
        map_pointer nfinish = nstart + node_nums - 1;
        map_pointer cur;
        MSTL_TRY__{
            for (cur = nstart; cur <= nfinish; ++cur) *cur = data_alloc_.allocate(buff_size());
        }
        MSTL_CATCH_UNWIND_THROW_M__(std::cout << "error" << std::endl);
        start_.set_node(nstart);
        finish_.set_node(nfinish);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_ + n % buff_size();
    }
    void fill_initialize(size_type n, T&& x) {
        create_map_and_nodes(n);
        map_pointer cur;
        for (cur = start_.node_; cur < finish_.node_; ++cur)
            MSTL::uninitialized_fill(*cur, *cur + buff_size(), std::forward<T>(x));
        MSTL::uninitialized_fill(finish_.first_, finish_.cur_, std::forward<T>(x));
    }
    void reallocate_map(size_type nodes_to_add, bool add_at_front) {
        size_type old_num_nodes = finish_.node_ - start_.node_ + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;
        map_pointer new_start;
        if (map_size_ > 2 * new_num_nodes) {
            new_start = map_ + (map_size_ - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            if (new_start < start_.node_) MSTL::copy(start_.node_, finish_.node_ + 1, new_start);
            else MSTL::copy_backward(start_.node_, finish_.node_ + 1, new_start + old_num_nodes);
        }
        else {
            size_type new_map_size = map_size_ + maximum(map_size_, nodes_to_add) + 2;
            map_pointer new_map = map_alloc_.allocate(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            MSTL::copy(start_.node_, finish_.node_ + 1, new_start);
            map_alloc_.deallocate(map_, map_size_);
            map_ = new_map;
            map_size_ = new_map_size;
        };
        start_.set_node(new_start);
        finish_.set_node(new_start + old_num_nodes - 1);
    }
    void push_back_aux(T&& x) {
        reserve_map_at_back();
        *(finish_.node_ + 1) = data_alloc_.allocate(buff_size());
        MSTL::construct(finish_.cur_, std::forward<T>(x));
        finish_.set_node(finish_.node_ + 1);
        finish_.cur_ = finish_.first_;
    }
    void push_back_aux(const T& x) {
        reserve_map_at_back();
        *(finish_.node_ + 1) = data_alloc_.allocate(buff_size());
        MSTL::construct(finish_.cur_, x);
        finish_.set_node(finish_.node_ + 1);
        finish_.cur_ = finish_.first_;
    }
    void push_front_aux(T&& x) {
        reserve_map_at_front();
        *(start_.node_ - 1) = data_alloc_.allocate(buff_size());
        start_.set_node(start_.node_ - 1);
        start_.cur_ = start_.last_ - 1;
        MSTL::construct(start_.cur_, std::forward<T>(x));
    }
    void push_front_aux(const T& x) {
        reserve_map_at_front();
        *(start_.node_ - 1) = data_alloc_.allocate(buff_size());
        start_.set_node(start_.node_ - 1);
        start_.cur_ = start_.last_ - 1;
        MSTL::construct(start_.cur_, x);
    }
    void pop_back_aux() {
        data_alloc_.deallocate(finish_.first_, buff_size());
        finish_.set_node(finish_.node_ - 1);
        finish_.cur_ = finish_.last_ - 1;
        MSTL::destroy(finish_.cur_);
    }
    void pop_front_aux() {
        MSTL::destroy(start_.cur_);
        data_alloc_.deallocate(start_.first_, buff_size());
        start_.set_node(start_.node_ + 1);
        start_.cur_ = start_.first_;
    }
    template <typename U>
    iterator insert_aux(iterator pos, U&& x) 
        requires(NothrowMoveAssignable<value_type> && NothrowAssignableFrom<T, U>) {
        difference_type index = pos - start_;
        if (size_t(index < size() / 2)) {
            push_front(front());
            iterator front1 = start_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start_ + index;
            iterator pos1 = pos;
            ++pos1;
            MSTL::copy(front2, pos1, front1);
        }
        else {
            push_back(back());
            iterator back1 = finish_;
            --back1;
            iterator back2 = back1;
            --back2;
            pos = start_ + index;
            MSTL::copy_backward(pos, back2, back1);
        }
        *pos = x;
        return pos;
    }
    template <typename Iterator>
        requires(InputIterator<Iterator>)
    iterator insert_aux(iterator pos, Iterator first, Iterator last) {
        difference_type n = last - first;
        difference_type index = pos - start_;
        if (index < size() / 2) {
            if (n <= start_.cur_ - start_.first_) {
                iterator new_start = start_ - n;
                MSTL::copy(start_, pos, new_start);
                pos = new_start + index;
                MSTL::copy(first, last, pos);
                start_ = new_start;
            }
            else {
                size_type needs =
                    difference_type(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_map_at_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_start = start_ - n;
                MSTL::copy(start_, pos, new_start);
                pos = new_start + index;
                MSTL::copy(first, last, pos);
                start_ = new_start;
            }
        }
        else {
            if (finish_.last_ - finish_.cur_ - 1 >= n) {
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(pos, finish_, new_finish);
                MSTL::copy(first, last, pos);
                finish_ = new_finish;
                pos = start_ + index;
            }
            else {
                difference_type left = finish_.last_ - finish_.cur_ - 1;
                size_type needs = difference_type(n - left - 1) / buff_size() + 1;
                reserve_map_at_back(needs);
                map_pointer cur = finish_.node_ + 1;
                for (; needs > 0; ++cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(pos, finish_, new_finish);
                MSTL::copy(first, last, pos);
                finish_ = new_finish;
                pos = start_ + index;
            }
        }
        return pos;
    }
    iterator insert_aux(iterator pos, size_type n, T&& x) {
        difference_type index = pos - start_;
        if (index < difference_type(size() / 2)) {
            if (n <= size_type(start_.cur_ - start_.first_)) {
                iterator new_start = start_ - n;
                MSTL::copy(start_, pos, new_start);
                pos = new_start + index;
                MSTL::fill_n(pos, n, std::forward<T>(x));
                start_ = new_start;
            }
            else {
                size_type needs =
                    difference_type(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_map_at_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), std::forward<T>(x));
                }
                iterator new_start = start_ - n;
                MSTL::copy(start_, pos, new_start);
                pos = new_start + index;
                MSTL::fill_n(pos, n, std::forward<T>(x));
                start_ = new_start;
            }
        }
        else {
            if (n <= size_type(finish_.last_ - finish_.cur_ - 1)) {
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(pos, finish_, new_finish);
                MSTL::fill_n(pos, n, std::forward<T>(x));
                finish_ = new_finish;
                pos = start_ + index;
            }
            else {
                difference_type left = finish_.last_ - finish_.cur_ - 1;
                size_type needs = difference_type(n - left - 1) / buff_size() + 1;
                reserve_map_at_back(needs);
                map_pointer cur = finish_.node_ + 1;
                for (; needs > 0; ++cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(pos, finish_, new_finish);
                MSTL::fill_n(pos, n, std::forward<T>(x));
                finish_ = new_finish;
                pos = start_ + index;
            }
        }
        return pos;
    }
public:
    deque() : data_alloc_(), map_alloc_(), map_(0),
        map_size_(1), start_(0), finish_(0) {
        map_ = map_alloc_.allocate(1);
        *map_ = data_alloc_.allocate(buff_size());
        start_.set_node(map_);
        finish_.set_node(map_);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_;
    }
    explicit deque(size_type n, T&& x = T()) :
        data_alloc_(), map_alloc_(), map_(0), map_size_(0), start_(), finish_() {
        fill_initialize(n, std::forward<T>(x));
    }

    template <typename Iterator>
        requires(InputIterator<Iterator>)
    deque(Iterator first, Iterator last) :
        data_alloc_(), map_alloc_(), map_(0), map_size_(0), start_(), finish_() {
        difference_type n = last - first;
        create_map_and_nodes(n);
        for (map_pointer cur = start_.node_; cur < finish_.node_; ++cur) {
            MSTL::uninitialized_copy(first, first + buff_size(), *cur);
            first = first + buff_size();
        }
        MSTL::uninitialized_copy(first, last, finish_.first_);
    }
    deque(const std::initializer_list<T>& _lls)
        : deque(_lls.begin(), _lls.end()) {}
    self& operator =(const std::initializer_list<T>& x) {
        if (*this == x) return *this;
        clear();
        insert(end(), x.begin(), x.end());
        return *this;
    }

    deque(const self& x) 
        : deque(x.const_begin(), x.const_end()) {}
    self& operator =(const self& x) {
        if (*this == x) return *this;
        clear();
        insert(end(), x.const_begin(), x.const_end());
        return *this;
    }

    explicit deque(self&& x) noexcept : data_alloc_(), map_alloc_(),
        map_(0), map_size_(0), start_(0), finish_(0) {
        swap(std::forward<self>(x));
    }
    self& operator =(self&& x) noexcept {
        if (*this == x) {
            x.clear();
            return *this;
        }
        clear();
        swap(std::forward<self>(x));
        return *this;
    }

    ~deque() {
        clear();
        MSTL::destroy(map_);
    }

    MSTL_NODISCARD iterator begin() noexcept { return iterator(start_); }
    MSTL_NODISCARD const_iterator const_begin() const noexcept { return const_iterator(start_); }
    MSTL_NODISCARD iterator end() noexcept { return iterator(finish_); }
    MSTL_NODISCARD const_iterator const_end() const noexcept { return const_iterator(finish_); }

    MSTL_NODISCARD reference front() noexcept { return *start_; }
    MSTL_NODISCARD const_reference front() const noexcept { return *start_; }
    MSTL_NODISCARD reference back() noexcept { return *(finish_ - 1); }
    MSTL_NODISCARD const_reference back() const noexcept { return *(finish_ - 1); }

    MSTL_NODISCARD size_type size() const noexcept { return finish_ - start_; }
    MSTL_NODISCARD bool empty() const noexcept { return finish_ == start_; }

    void resize(size_type new_size, T&& x) {
        if (new_size < size()) 
            erase(start_ + new_size, finish_);
        else 
            insert(finish_, new_size - size(), std::forward<T>(x));
    }
    void resize(size_type new_size) { resize(new_size, value_type()); }

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (map_size_ - (finish_.node_ - map_) - 1 < nodes_to_add) reallocate_map(nodes_to_add, false);
    }
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (size_type(start_.node_ - map_) < nodes_to_add) reallocate_map(nodes_to_add, true);
    }

    void push_back(T&& x) {
        if (finish_.cur_ != finish_.last_ - 1) {
            MSTL::construct(finish_.cur_, std::forward<T>(x));
            ++finish_.cur_;
        }
        else push_back_aux(std::forward<T>(x));
    }
    void push_back(const T& x) {
        if (finish_.cur_ != finish_.last_ - 1) {
            MSTL::construct(finish_.cur_, x);
            ++finish_.cur_;
        }
        else push_back_aux(x);
    }

    void push_front(T&& x) {
        if (start_.cur_ != start_.first_) {
            MSTL::construct(start_.cur_ - 1, std::forward<T>(x));
            --start_.cur_;
        }
        else push_front_aux(std::forward<T>(x));
    }
    void push_front(const T& x) {
        if (start_.cur_ != start_.first_) {
            MSTL::construct(start_.cur_ - 1, x);
            --start_.cur_;
        }
        else push_front_aux(x);
    }

    void pop_back() noexcept {
        if (finish_.cur_ != finish_.first_) {
            --finish_.cur_;
            MSTL::destroy(finish_.cur_);
        }
        else pop_back_aux();
    }
    void pop_front() noexcept {
        if (start_.cur_ != start_.last_ - 1) {
            MSTL::destroy(start_.cur_);
            ++start_.cur_;
        }
        else pop_front_aux();
    }

    void clear() noexcept {
        map_pointer cur = start_.node_;
        for (++cur; cur < finish_.node_; ++cur) {
            MSTL::destroy(*cur, *cur + buff_size());
            data_alloc_.deallocate(*cur, buff_size());
        }
        if (start_.node_ == finish_.node_) MSTL::destroy(start_.cur_, finish_.cur_);
        else {
            MSTL::destroy(finish_.first_, finish_.cur_);
            data_alloc_.deallocate(finish_.first_, buff_size());
            MSTL::destroy(start_.cur_, start_.last_);
        }
        finish_ = start_;
    }

    iterator erase(iterator pos) noexcept {
        iterator next = pos;
        ++next;
        difference_type n = pos - start_;
        if (n < size() / 2) {
            MSTL::copy_backward(start_, pos, next);
            pop_front();
        }
        else {
            MSTL::copy(next, finish_, pos);
            pop_back();
        }
        return start_ + n;
    }
    iterator erase(iterator first, iterator last) noexcept {
        if (first == start_ && last == finish_) {
            clear();
            return finish_;
        }
        difference_type len = last - first;
        difference_type n = first - start_;
        if (n < difference_type((size() - len) / 2)) {
            MSTL::copy_backward(start_, first, last);
            iterator new_start = start_ + len;
            MSTL::destroy(start_, new_start);
            for (map_pointer cur = start_.node_; cur < new_start.node_; ++cur)
                data_alloc_.deallocate(*cur, buff_size());
            start_ = new_start;
        }
        else {
            MSTL::copy(last, finish_, first);
            iterator new_finish = finish_ - len;
            MSTL::destroy(new_finish, finish_);
            for (map_pointer cur = new_finish.node_ + 1; cur <= finish_.node_; ++cur)
                data_alloc_.deallocate(*cur, buff_size());
            finish_ = new_finish;
        }
        return start_ + n;
    }
    template <typename Iterator>
        requires(InputIterator<Iterator>)
    iterator insert(iterator position, Iterator first, Iterator last) {
        if (position == start_) {
            for (--last; first != last; --last) {
                push_front(*last);
            }
            push_front(*first);
            return start_;
        }
        else if (position == finish_) {
            for (; first != last; ++first) push_back(*first);
            return finish_ - 1;
        }
        else return insert_aux(position, first, last);
    }
    iterator insert(iterator position, const std::initializer_list<T>& l) {
        insert(position, l.begin(), l.end());
    }
    iterator insert(iterator position, size_t n, T&& x) {
        if (position == start_) {
            for (size_t i = 0; i < n; i++) push_front(std::forward<T>(x));
            return start_;
        }
        else if (position == finish_) {
            for (size_t i = 0; i < n; i++) push_back(std::forward<T>(x));
            return finish_ - 1;
        }
        else return insert_aux(position, size_type(n), std::forward<T>(x));

    }
    iterator insert(iterator position, T&& x) {
        if (position.cur_ == start_.cur_) {
            push_front(std::forward<T>(x));
            return start_;
        }
        else if (position.cur_ == finish_.cur_) {
            push_back(std::forward<T>(x));
            return finish_ - 1;
        }
        else return insert_aux(position, std::forward<T>(x));
    }
    void swap(self&& x) noexcept {
        std::swap(start_, x.start_);
        std::swap(finish_, x.finish_);
        std::swap(map_, x.map_);
        std::swap(map_size_, x.map_size_);
    }
    MSTL_NODISCARD const_reference at(size_type _pos) const noexcept {
        return const_iterator(start_)[_pos];
    }
    MSTL_NODISCARD reference at(size_type _pos) noexcept {
        return const_cast<reference>(
            static_cast<const self*>(this)->at(_pos)
            );
    }
    MSTL_NODISCARD const_reference operator [](size_type _pos) const noexcept {
        return this->at(_pos);
    }
    MSTL_NODISCARD reference operator [](size_type _pos) noexcept {
        return this->at(_pos);
    }
};
template <class T, class Alloc>
MSTL_NODISCARD inline bool operator ==(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return lh.size() == rh.size() && MSTL::equal(lh.begin(), lh.end(), rh.begin());
}
template <class T, class Alloc>
MSTL_NODISCARD inline bool operator !=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh == rh);
}
template <class T, class Alloc>
MSTL_NODISCARD inline bool operator <(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return MSTL::lexicographical_compare(lh.begin(), lh.end(), rh.begin(), rh.end());
}
template <class T, class Alloc>
MSTL_NODISCARD inline bool operator >(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return rh < lh;
}
template <class T, class Alloc>
MSTL_NODISCARD inline bool operator <=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh > rh);
}
template <class T, class Alloc>
MSTL_NODISCARD inline bool operator >=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh < rh);
}
template <class T, class Alloc>
inline void swap(deque<T, Alloc>& lh, deque<T, Alloc>& rh) {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__

#endif // MSTL_DEQUE_HPP__

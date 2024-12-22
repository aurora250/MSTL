#ifndef MSTL_DEQUE_HPP__
#define MSTL_DEQUE_HPP__

#include "iterator.hpp"
#include "memory.hpp"
#include "algobase.hpp" 

MSTL_BEGIN_NAMESPACE__

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

    static size_t buff_size() {
        return deque_buf_size(BufSize, sizeof(T));
    }
    void set_node(map_pointer new_node) {
        node_ = new_node;
        first_ = *new_node;
        last_ = first_ + difference_type(buff_size());
    }

    deque_iterator(link_type cur = nullptr, link_type first = nullptr,
        link_type last = nullptr, map_pointer node = nullptr) :
        cur_(cur), first_(first), last_(last), node_(node) {}
    deque_iterator(const iterator& x) :
        cur_(x.cur_), first_(x.first_), last_(x.last_), node_(x.node_) {}
    self& operator =(const self& rh) {
		if(*this == rh) return *this;
		this->cur_ = rh.cur_;
		this->first_ = rh.first_;
		this->last_ = rh.last_;
		this->node_ = rh.node_;
		return *this;
	}
    ~deque_iterator() = default;

    reference operator *()const { return *cur_; }
    pointer operator ->()const { return &(operator*()); }
    difference_type operator -(const self& x)const {
        return difference_type((node_ - x.node_ - 1) * buff_size() + (cur_ - first_) + (x.last_ - x.cur_));
    }
    self& operator ++() {
        ++cur_;
        if (cur_ == last_) {
            set_node(node_ + 1);
            cur_ = first_;
        }
        return *this;
    }
    self operator ++(int) {
        self temp = *this;
        ++*this;
        return temp;
    }
    self& operator --() {
        if (cur_ == first_) {
            set_node(node_ - 1);
            cur_ = last_;
        }
        --cur_;
        return *this;
    }
    self operator --(int) {
        self temp = *this;
        --*this;
        return temp;
    }
    self& operator +=(difference_type n) {
        difference_type all = (cur_ - first_) + n;
        if (n >= 0) {
            if (all >= buff_size()) {
                difference_type node_num = all / buff_size();
                difference_type node_left = all - node_num * buff_size();
                node_ = set_node(node_ + node_num);
                cur_ = first_ + node_left;
            }
            else {
                cur_ = cur_ + n;
            }
        }
        else {
            if (all < 0) {
                difference_type node_num = (-all) / buff_size() + 1;
                difference_type node_left = (-all) - (node_num - 1) * buff_size();
                node_ = set_node(node_ - node_num);
                cur_ = last_ - node_left - 1;
            }
            else {
                cur_ = cur_ - n;
            }
        }
    }
    self& operator -=(difference_type n) { return *this += -n; }
    self operator +(difference_type n)const {
        self temp = *this;
        return temp += n;
    }
    self operator -(difference_type n)const {
        self temp = *this;
        return temp -= n;
    }
    reference operator [](difference_type n) { return *(*this + n); }
    bool operator ==(const self& x)const { return cur_ == x.cur_; }
    bool operator !=(const self& x)const { return !(*this == x); }
    bool operator <(const self& x)const { return node_ == x.node_ ? (cur_ < x.cur_) : (node_ < x.node_); }
};

template<typename T, typename Alloc = default_standard_alloc<T>, size_t BufSize = 0>
class deque {
public:
    typedef T                           value_type;
    typedef value_type*                 pointer;
    typedef value_type&                 reference;
    typedef const value_type&           reference_const;
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

    static size_t buff_size() {
        return deque_buf_size(BufSize, sizeof(T));
    }
    void __create_map_and_nodes(size_type n) {
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
    void __fill_initialize(size_type n, reference_const x) {
        __create_map_and_nodes(n);
        map_pointer cur;
        for (cur = start_.node_; cur < finish_.node_; ++cur)
            MSTL::uninitialized_fill(*cur, *cur + buff_size(), x);
        MSTL::uninitialized_fill(finish_.first_, finish_.cur_, x);
    }
    void __reallocate_map(size_type nodes_to_add, bool add_at_front) {
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
    void __push_back_aux(reference_const x) {
        value_type x_copy = x;
        reserve_map_at_back();
        *(finish_.node_ + 1) = data_alloc_.allocate(buff_size());
        MSTL::construct(finish_.cur_, x_copy);
        finish_.set_node(finish_.node_ + 1);
        finish_.cur_ = finish_.first_;
    }
    void __push_front_aux(reference_const x) {
        value_type x_copy = x;
        reserve_map_at_front();
        *(start_.node_ - 1) = data_alloc_.allocate(buff_size());
        start_.set_node(start_.node_ - 1);
        start_.cur_ = start_.last_ - 1;
        MSTL::construct(start_.cur_, x_copy);
    }
    void __pop_back_aux() {
        data_alloc_.deallocate(finish_.first_, buff_size());
        finish_.set_node(finish_.node_ - 1);
        finish_.cur_ = finish_.last_ - 1;
        MSTL::destroy(finish_.cur_);
    }
    void __pop_front_aux() {
        MSTL::destroy(start_.cur_);
        data_alloc_.deallocate(start_.first_, buff_size());
        start_.set_node(start_.node_ + 1);
        start_.cur_ = start_.first_;
    }
    iterator __insert_aux(iterator pos, reference_const x) {
        difference_type index = pos - start_;
        value_type x_copy = x;
        if (index < size() / 2) {
            push_front(front());
            iterator front1 = start_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = start_ + index;
            iterator pos1 = pos;
            ++pos1;
            copy(front2, pos1, front1);
        }
        else {
            push_back(back());
            iterator back1 = finish_;
            --back1;
            iterator back2 = back1;
            --back2;
            pos = start_ + index;
            copy_backward(pos, back2, back1);
        }
        *pos = x_copy;
        return pos;
    }
    template <typename InputIterator>
    iterator __insert_aux(iterator pos, InputIterator first, InputIterator last) {
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
    iterator __insert_aux(iterator pos, size_type n, reference_const x) {
        difference_type index = pos - start_;
        if (index < size() / 2) {
            if (n <= start_.cur_ - start_.first_) {
                iterator new_start = start_ - n;
                MSTL::copy(start_, pos, new_start);
                pos = new_start + index;
                MSTL::fill_n(pos, n, x);
                start_ = new_start;
            }
            else {
                size_type needs =
                    difference_type(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_map_at_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), x);
                }
                iterator new_start = start_ - n;
                MSTL::copy(start_, pos, new_start);
                pos = new_start + index;
                MSTL::fill_n(pos, n, x);
                start_ = new_start;
            }
        }
        else {
            if (finish_.last_ - finish_.cur_ - 1 >= n) {
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(pos, finish_, new_finish);
                MSTL::fill_n(pos, n, x);
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
                MSTL::fill_n(pos, n, x);
                finish_ = new_finish;
                pos = start_ + index;
            }
        }
        return pos;
    }
public:
    explicit deque() : data_alloc_(), map_alloc_(), map_(0), map_size_(1), start_(), finish_() {
        map_ = map_alloc_.allocate(1);
        *map_ = data_alloc_.allocate(buff_size());
        start_.set_node(map_);
        finish_.set_node(map_);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_;
    }
    explicit deque(size_type n, reference_const x = T()) :
        data_alloc_(), map_alloc_(), map_(0), map_size_(0), start_(), finish_() {
        __fill_initialize(n, x);
    }
    explicit deque(const std::initializer_list<T>& _lls) : deque(_lls.begin(), _lls.end()) {}
    template<typename InputIterator>
    deque(InputIterator first, InputIterator last) :
        data_alloc_(), map_alloc_(), map_(0), map_size_(0), start_(), finish_() {
        difference_type n = last - first;
        __create_map_and_nodes(n);
        for (map_pointer cur = start_.node_; cur < finish_.node_; ++cur) {
            MSTL::uninitialized_copy(first, first + buff_size(), *cur);
            first = first + buff_size();
        }
        MSTL::uninitialized_copy(first, last, finish_.first_);
    }
    explicit deque(self& x) : deque(x.begin(), x.end()) {}
    explicit deque(self&& x) : data_alloc_(), map_alloc_(),
		map_(x.map_), map_size_(x.map_size_), start_(x.start_), finish_(x.finish_) {
        x.map_ = 0;
        x.map_size_ = 0;
        x.finish_ = x.start_;
    }
    ~deque() {
        clear();
        MSTL::destroy(map_);
    }

    iterator begin() { return iterator(start_); }
    iterator end() { return iterator(finish_); }
    const_iterator const_begin() const { return const_iterator(start_); }
    const_iterator const_end() const { return const_iterator(finish_); }
    reference front() { return *start_; }
    reference back() { return *(finish_ - 1); }
    reference operator[](const size_t& n) {
        return start_[difference_type(n)];
    }
    void swap(self& rep) {
        std::swap(start_, rep.start_);
        std::swap(finish_, rep.finish_);
        std::swap(map_, rep.map_);
        std::swap(map_size_, rep.map_size_);
    }
    size_type size() const { return finish_ - start_; }
    bool empty() const { return finish_ == start_; }

    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (map_size_ - (finish_.node_ - map_) - 1 < nodes_to_add) __reallocate_map(nodes_to_add, false);
    }
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (size_type(start_.node_ - map_) < nodes_to_add) __reallocate_map(nodes_to_add, true);
    }

    void push_back(reference_const x) {
        if (finish_.cur_ != finish_.last_ - 1) {
            MSTL::construct(finish_.cur_, x);
            ++finish_.cur_;
        }
        else __push_back_aux(x);
    }
    void push_front(const value_type& x) {
        if (start_.cur_ != start_.first_) {
            MSTL::construct(start_.cur_ - 1, x);
            --start_.cur_;
        }
        else __push_front_aux(x);
    }
    void pop_back() {
        if (finish_.cur_ != finish_.first_) {
            --finish_.cur_;
            MSTL::destroy(finish_.cur_);
        }
        else __pop_back_aux();
    }
    void pop_front() {
        if (start_.cur_ != start_.last_ - 1) {
            MSTL::destroy(start_.cur_);
            ++start_.cur_;
        }
        else __pop_front_aux();
    }
    void clear() {
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
    iterator erase(iterator pos) {
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
    iterator erase(iterator first, iterator last) {
        if (first == start_ && last == finish_) {
            clear();
            return finish_;
        }
        difference_type len = last - first;
        difference_type n = first - start_;
        if (n < (size() - len) / 2) {
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
    template <typename InputIterator>
    iterator insert(iterator position, InputIterator first, InputIterator last) {
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
    iterator insert(iterator position, const int& n, reference_const x) {
        if (position == start_) {
            for (int i = 0; i < n; i++) push_front(x);
            return start_;
        }
        else if (position == finish_) {
            for (int i = 0; i < n; i++) push_back(x);
            return finish_ - 1;
        }
        else return insert_aux(position, size_type(n), x);

    }
    iterator insert(iterator position, reference_const x) {
        if (position.cur_ == start_.cur_) {
            push_front(x);
            return start_;
        }
        else if (position.cur_ == finish_.cur_) {
            push_back(x);
            return finish_ - 1;
        }
        else return insert_aux(position, x);
    }
};
template <class T, class Alloc>
inline bool operator ==(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return lh.size() == rh.size() && MSTL::equal(lh.begin(), lh.end(), rh.begin());
}
template <class T, class Alloc>
inline bool operator <(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return MSTL::lexicographical_compare(lh.begin(), lh.end(), rh.begin(), rh.end());
}
template <class T, class Alloc>
inline void swap(deque<T, Alloc>& lh, deque<T, Alloc>& rh) {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__

#endif // MSTL_DEQUE_HPP__

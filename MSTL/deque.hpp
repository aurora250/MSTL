#ifndef MSTL_DEQUE_HPP__
#define MSTL_DEQUE_HPP__
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_CONSTEXPR size_t deque_buf_size(size_t n, size_t sz) noexcept {
    return n != 0 ? n : (sz < size_t(256) ? size_t(256 / sz) : 1);
}

template <typename T, typename Ref = T&, typename Ptr = T*, size_t BufSize = 0>
struct deque_iterator {
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using pointer           = Ptr;
    using reference         = Ref;
    using difference_type   = ptrdiff_t;

    using size_type         = size_t;
    using link_type         = T*;
    using map_pointer       = link_type*;
    using self              = deque_iterator<T, Ref, Ptr, BufSize>;
    using iterator          = deque_iterator<T, T&, T*, BufSize>;
    using const_iterator    = deque_iterator<T, const T&, const T*, BufSize>;

    link_type cur_;
    link_type first_;
    link_type last_;
    map_pointer node_;

    MSTL_CONSTEXPR static size_t buff_size() noexcept {
        return deque_buf_size(BufSize, sizeof(T));
    }

    void change_buff(map_pointer new_node) noexcept {
        node_ = new_node;
        first_ = *new_node;
        last_ = first_ + difference_type(buff_size());
    }

    deque_iterator(link_type cur = nullptr, link_type first = nullptr,
        link_type last = nullptr, map_pointer node = nullptr) noexcept :
        cur_(cur), first_(first), last_(last), node_(node) {}

    deque_iterator(const iterator& x) noexcept :
        cur_(x.cur_), first_(x.first_), last_(x.last_), node_(x.node_) {}

    self& operator =(const self& x) noexcept {
		if(*this == x) return *this;
		this->cur_ = x.cur_;
		this->first_ = x.first_;
		this->last_ = x.last_;
		this->node_ = x.node_;
		return *this;
	}

    deque_iterator(iterator&& x) noexcept :
        cur_(x.cur_), first_(x.first_), last_(x.last_), node_(x.node_) {
        x.cur_ = nullptr; x.first_ = nullptr; 
        x.last_ = nullptr; x.node_ = nullptr;
    }

    ~deque_iterator() noexcept = default;

    MSTL_NODISCARD reference operator *() const noexcept { return *cur_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }

    self& operator ++() noexcept {
        ++cur_;
        if (cur_ == last_) {
            change_buff(node_ + 1);
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
            change_buff(node_ - 1);
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
            change_buff(node_ + node_offset);
            cur_ = first_ + (offset - node_offset * difference_type(buff_size()));
        }
        return *this;
    }
    MSTL_NODISCARD self operator +(difference_type n) const noexcept {
        self temp = *this;
        return temp += n;
    }
    MSTL_NODISCARD friend MSTL_CONSTEXPR self operator +(difference_type n, const self& it) {
        return it + n;
    }

    self& operator -=(difference_type n) noexcept { return *this += -n; }
    MSTL_NODISCARD self operator -(difference_type n) const noexcept {
        self temp = *this;
        return temp -= n;
    }
    difference_type operator -(const self& x) const noexcept {
        return (node_ - x.node_ - 1) * buff_size() + (cur_ - first_) + (x.last_ - x.cur_);
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

template<typename T, typename Alloc = standard_allocator<T>, size_t BufSize = 0>
class deque {
#ifdef MSTL_VERSION_20__	
    static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
    static_assert(is_same_v<T, typename Alloc::value_type>, "allocator type mismatch.");
    static_assert(is_object_v<T>, "deque only contains object types.");

public:
    using value_type        = T;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using self              = deque<T, Alloc, BufSize>;
    using allocator_type    = Alloc;
    using iterator          = deque_iterator<T, T&, T*, BufSize>;
    using const_iterator    = deque_iterator<T, const T&, const T*, BufSize>;
    using reverse_iterator  = MSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = MSTL::reverse_iterator<const_iterator>;

private:
    using map_pointer       = pointer*;
    using map_allocator     = standard_allocator<pointer>;

    MSTL_NO_UNIADS allocator_type data_alloc_;
    MSTL_NO_UNIADS map_allocator map_alloc_;
    map_pointer map_;
    size_type map_size_;
    iterator start_;
    iterator finish_;

    inline void range_check(size_type position) const noexcept {
        MSTL_DEBUG_VERIFY__(position < static_cast<size_type>(MSTL::distance(start_, finish_)), 
            "deque index out of ranges.");
    }
    inline void range_check(iterator position) const noexcept {
        MSTL_DEBUG_VERIFY__(position >= start_ && position <= finish_, "deque index out of ranges.");
    }

    MSTL_CONSTEXPR static size_t buff_size() noexcept {
        return deque_buf_size(BufSize, sizeof(T));
    }

    void create_map_and_nodes(size_type n) {
        size_type node_nums = n / buff_size() + 1;
        map_size_ = MSTL::max(size_type(8), node_nums + 2);
        map_ = map_alloc_.allocate(map_size_);
        map_pointer nstart = map_ + (map_size_ - node_nums) / 2;
        map_pointer nfinish = nstart + node_nums - 1;
        map_pointer cur;
        MSTL_TRY__{
            for (cur = nstart; cur <= nfinish; ++cur) *cur = data_alloc_.allocate(buff_size());
        }
        MSTL_CATCH_UNWIND_THROW_M__(
            for (cur = nstart; cur <= nfinish; ++cur) data_alloc_.deallocate(*cur, buff_size());
        );
        start_.change_buff(nstart);
        finish_.change_buff(nfinish);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_ + n % buff_size();
    }

    void fill_initialize(size_type n, const T& x) {
        create_map_and_nodes(n);
        map_pointer cur;
        for (cur = start_.node_; cur < finish_.node_; ++cur)
            MSTL::uninitialized_fill(*cur, *cur + buff_size(), x);
        MSTL::uninitialized_fill(finish_.first_, finish_.cur_, x);
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
            size_type new_map_size = map_size_ + MSTL::max(map_size_, nodes_to_add) + 2;
            map_pointer new_map = map_alloc_.allocate(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            MSTL::copy(start_.node_, finish_.node_ + 1, new_start);
            map_alloc_.deallocate(map_, map_size_);
            map_ = new_map;
            map_size_ = new_map_size;
        };
        start_.change_buff(new_start);
        finish_.change_buff(new_start + old_num_nodes - 1);
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    iterator range_insert(iterator position, Iterator first, Iterator last) {
        difference_type n = last - first;
        difference_type index = position - start_;
        if (index < size() / 2) {
            if (n <= start_.cur_ - start_.first_) {
                iterator new_start = start_ - n;
                MSTL::copy(start_, position, new_start);
                position = new_start + index;
                MSTL::copy(first, last, position);
                start_ = new_start;
            }
            else {
                size_type needs =
                    difference_type(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_start = start_ - n;
                MSTL::copy(start_, position, new_start);
                position = new_start + index;
                MSTL::copy(first, last, position);
                start_ = new_start;
            }
        }
        else {
            if (finish_.last_ - finish_.cur_ - 1 >= n) {
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(position, finish_, new_finish);
                MSTL::copy(first, last, position);
                finish_ = new_finish;
                position = start_ + index;
            }
            else {
                difference_type left = finish_.last_ - finish_.cur_ - 1;
                size_type needs = difference_type(n - left - 1) / buff_size() + 1;
                reserve_back(needs);
                map_pointer cur = finish_.node_ + 1;
                for (; needs > 0; ++cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(position, finish_, new_finish);
                MSTL::copy(first, last, position);
                finish_ = new_finish;
                position = start_ + index;
            }
        }
        return position;
    }

    iterator range_insert(iterator position, size_type n, const T& x) {
        difference_type index = position - start_;
        if (index < difference_type(size() / 2)) {
            if (n <= size_type(start_.cur_ - start_.first_)) {
                iterator new_start = start_ - n;
                MSTL::copy(start_, position, new_start);
                position = new_start + index;
                MSTL::fill_n(position, n, x);
                start_ = new_start;
            }
            else {
                size_type needs =
                    difference_type(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), x);
                }
                iterator new_start = start_ - n;
                MSTL::copy(start_, position, new_start);
                position = new_start + index;
                MSTL::fill_n(position, n, x);
                start_ = new_start;
            }
        }
        else {
            if (n <= size_type(finish_.last_ - finish_.cur_ - 1)) {
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(position, finish_, new_finish);
                MSTL::fill_n(position, n, x);
                finish_ = new_finish;
                position = start_ + index;
            }
            else {
                difference_type left = finish_.last_ - finish_.cur_ - 1;
                size_type needs = difference_type(n - left - 1) / buff_size() + 1;
                reserve_back(needs);
                map_pointer cur = finish_.node_ + 1;
                for (; needs > 0; ++cur, --needs) {
                    *cur = data_alloc_.allocate(buff_size());
                    MSTL::uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_finish = finish_ + n;
                MSTL::copy_backward(position, finish_, new_finish);
                MSTL::fill_n(position, n, x);
                finish_ = new_finish;
                position = start_ + index;
            }
        }
        return position;
    }

    template <typename... Args>
    iterator emplace_aux(iterator position, Args&&... args)
        noexcept(is_nothrow_move_assignable_v<value_type>) {
        size_type index = MSTL::distance(start_, position);
        if (index < size() / 2) {
            (emplace_front)(front());
            iterator front1 = start_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            position = start_ + index;
            iterator pos1 = position;
            ++pos1;
            MSTL::copy(front2, pos1, front1);
        }
        else {
            push_back(back());
            iterator back1 = finish_;
            --back1;
            iterator back2 = back1;
            --back2;
            position = start_ + index;
            MSTL::copy_backward(position, back2, back1);
        }
        value_type val(MSTL::forward<Args>(args)...);
        *position = MSTL::move(val);
        return position;
    }

public:
    deque() : data_alloc_(), map_alloc_(), map_(nullptr),
        map_size_(0), start_(nullptr), finish_(nullptr) {
        map_ = map_alloc_.allocate(1);
        *map_ = data_alloc_.allocate(buff_size());
        start_.change_buff(map_);
        finish_.change_buff(map_);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_;
    }

    explicit deque(size_type n) :
        data_alloc_(), map_alloc_(), map_(nullptr), map_size_(0), start_(nullptr), finish_(nullptr) {
        (fill_initialize)(n, T());
    }

    deque(size_type n, const T& x) :
        data_alloc_(), map_alloc_(), map_(nullptr), map_size_(0), start_(nullptr), finish_(nullptr) {
        (fill_initialize)(n, x);
    }
    deque(size_type n, T&& x) :
        data_alloc_(), map_alloc_(), map_(nullptr), map_size_(0), start_(nullptr), finish_(nullptr) {
        (fill_initialize)(n, MSTL::forward<T>(x));
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    deque(Iterator first, Iterator last) :
        data_alloc_(), map_alloc_(), map_(nullptr), map_size_(0), start_(nullptr), finish_(nullptr) {
        Exception(MSTL::distance(first, last) >= 0,
            StopIterator("deque iterator-constructor out of ranges."));
        create_map_and_nodes(MSTL::distance(first, last));
        for (map_pointer cur = start_.node_; cur < finish_.node_; ++cur) {
            MSTL::uninitialized_copy(first, first + buff_size(), *cur);
            MSTL::advance(first, buff_size());
        }
        MSTL::uninitialized_copy(first, last, finish_.first_);
    }

    deque(std::initializer_list<T> l)
        : deque(l.begin(), l.end()) {}

    self& operator =(std::initializer_list<T> l) {
        clear();
        insert(end(), l.begin(), l.end());
        return *this;
    }

    deque(const self& x) : deque(x.cbegin(), x.cend()) {}

    self& operator =(const self& x) {
        if (MSTL::addressof(x) == this) return *this;
        clear();
        insert(end(), x.cbegin(), x.cend());
        return *this;
    }

    deque(self&& x) noexcept : data_alloc_(), map_alloc_(),
        map_(nullptr), map_size_(0), start_(nullptr), finish_(nullptr) {
        swap(x);
    }
    self& operator =(self&& x) noexcept {
        if (MSTL::addressof(x) == this) return *this;
        clear();
        swap(x);
        return *this;
    }

    ~deque() {
        clear();
        if(map_) MSTL::destroy(map_);
    }

    MSTL_NODISCARD iterator begin() noexcept { return iterator(start_); }
    MSTL_NODISCARD iterator end() noexcept { return iterator(finish_); }
    MSTL_NODISCARD const_iterator cbegin() const noexcept { return const_iterator(start_); }
    MSTL_NODISCARD const_iterator cend() const noexcept { return const_iterator(finish_); }
    MSTL_NODISCARD reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    MSTL_NODISCARD reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    MSTL_NODISCARD const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    MSTL_NODISCARD const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    MSTL_NODISCARD size_type size() const noexcept { return finish_ - start_; }
    MSTL_NODISCARD size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    MSTL_NODISCARD bool empty() const noexcept { return finish_ == start_; }

    MSTL_NODISCARD allocator_type get_allocator() const noexcept { return allocator_type(); }

    MSTL_NODISCARD reference front() noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "front called on empty deque");
        return *start_;
    }
    MSTL_NODISCARD const_reference front() const noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "front called on empty deque");
        return *start_;
    }
    MSTL_NODISCARD reference back() noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "back called on empty deque");
        return *(finish_ - 1);
    }
    MSTL_NODISCARD const_reference back() const noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "back called on empty deque");
        return *(finish_ - 1);
    }

    void reserve_back(size_type nodes_to_add = 1) {
        if (map_size_ - (finish_.node_ - map_) - 1 < nodes_to_add)
            reallocate_map(nodes_to_add, false);
    }
    void reserve_front(size_type nodes_to_add = 1) {
        if (size_type(start_.node_ - map_) < nodes_to_add)
            reallocate_map(nodes_to_add, true);
    }

    void resize(size_type new_size, const T& x) {
        if (new_size < size()) erase(start_ + new_size, finish_);
        else insert(finish_, new_size - size(), x);
    }
    void resize(size_type new_size) { resize(new_size, T()); }

    template <typename... Args>
    typename iterator emplace(iterator position, Args&& ...args) {
        range_check(position);
        if (position.cur_ == start_.cur_) {
            (emplace_front)(MSTL::forward<Args>(args)...);
            return start_;
        }
        else if (position.cur_ == finish_.cur_) {
            (emplace_back)(MSTL::forward<Args>(args)...);
            return finish_ - 1;
        }
        else
            return (emplace_aux)(position, MSTL::forward<Args>(args)...);
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (finish_.cur_ != finish_.last_ - 1) {
            MSTL::construct(finish_.cur_, MSTL::forward<Args>(args)...);
            ++finish_.cur_;
        }
        else {
            reserve_back();
            *(finish_.node_ + 1) = data_alloc_.allocate(buff_size());
            MSTL::construct(finish_.cur_, MSTL::forward<Args>(args)...);
            finish_.change_buff(finish_.node_ + 1);
            finish_.cur_ = finish_.first_;
        }
    }
    template <typename... Args>
    void emplace_front(Args&&... args) {
        if (start_.cur_ != start_.first_) {
            MSTL::construct(start_.cur_ - 1, MSTL::forward<Args>(args)...);
            --start_.cur_;
        }
        else {
            reserve_front();
            *(start_.node_ - 1) = data_alloc_.allocate(buff_size());
            start_.change_buff(start_.node_ - 1);
            start_.cur_ = start_.last_ - 1;
            MSTL::construct(start_.cur_, MSTL::forward<Args>(args)...);
        }
    }

    void push_back(const T& x) {
        (emplace_back)(x);
    }
    void push_front(const T& x) {
        (emplace_front)(x);
    }
    void push_back(T&& x) {
        (emplace_back)(MSTL::forward<T>(x));
    }
    void push_front(T&& x) {
        (emplace_front)(MSTL::forward<T>(x));
    }

    void pop_back() noexcept {
        if (finish_.cur_ != finish_.first_) {
            --finish_.cur_;
            MSTL::destroy(finish_.cur_);
        }
        else {
            data_alloc_.deallocate(finish_.first_, buff_size());
            finish_.change_buff(finish_.node_ - 1);
            finish_.cur_ = finish_.last_ - 1;
            MSTL::destroy(finish_.cur_);
        }
    }
    void pop_front() noexcept {
        if (start_.cur_ != start_.last_ - 1) {
            MSTL::destroy(start_.cur_);
            ++start_.cur_;
        }
        else {
            MSTL::destroy(start_.cur_);
            data_alloc_.deallocate(start_.first_, buff_size());
            start_.change_buff(start_.node_ + 1);
            start_.cur_ = start_.first_;
        }
    }

    void assign(size_type count, const T& value) {
        clear();
        insert(begin(), count, value);
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    void assign(Iterator first, Iterator last) {
        clear();
        insert(begin(), first, last);
    }

    void assign(std::initializer_list<T> l) {
        assign(l.begin(), l.end());
    }

    iterator insert(iterator position, const T& x) {
        return (emplace)(position, x);
    }
    iterator insert(iterator position, T&& x) {
        return (emplace)(position, MSTL::forward<T>(x));
    }
    iterator insert(iterator position) {
        return (emplace)(position, T());
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    iterator insert(iterator position, Iterator first, Iterator last) {
        range_check(position);
        Exception(MSTL::distance(first, last) >= 0, StopIterator("deque insert out of ranges."));
        if (position == start_) {
            for (--last; first != last; --last)
                (emplace_front)(*last);
            (emplace_front)(*first);
            return start_;
        }
        else if (position == finish_) {
            for (; first != last; ++first)
                (emplace_back)(*first);
            return finish_ - 1;
        }
        else
            return range_insert(position, first, last);
    }
    iterator insert(iterator position, std::initializer_list<T> l) {
        insert(position, l.begin(), l.end());
    }
    iterator insert(iterator position, size_t n, const T& x) {
        range_check(position);
        if (position == start_) {
            for (size_t i = 0; i < n; i++)
                (emplace_front)(x);
            return start_;
        }
        else if (position == finish_) {
            for (size_t i = 0; i < n; i++)
                (emplace_back)(x);
            return finish_ - 1;
        }
        else
            return range_insert(position, size_type(n), x);

    }

    iterator erase(iterator position) noexcept {
        range_check(position);
        iterator next = position;
        ++next;
        difference_type n = position - start_;
        if (n < size() / 2) {
            MSTL::copy_backward(start_, position, next);
            pop_front();
        }
        else {
            MSTL::copy(next, finish_, position);
            pop_back();
        }
        return start_ + n;
    }
    iterator erase(iterator first, iterator last) noexcept {
        Exception(MSTL::distance(first, last) >= 0, StopIterator("deque erase out of ranges."));
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

    void swap(self& x) noexcept {
        MSTL::swap(start_, x.start_);
        MSTL::swap(finish_, x.finish_);
        MSTL::swap(map_, x.map_);
        MSTL::swap(map_size_, x.map_size_);
    }

    MSTL_NODISCARD const_reference at(size_type position) const {
        range_check(position);
        return const_iterator(start_)[position];
    }
    MSTL_NODISCARD reference at(size_type position) {
        return const_cast<reference>(
            static_cast<const self*>(this)->at(position)
            );
    }
    MSTL_NODISCARD const_reference operator [](size_type position) const {
        return this->at(position);
    }
    MSTL_NODISCARD reference operator [](size_type position) {
        return this->at(position);
    }
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Iterator, typename Alloc>
deque(Iterator, Iterator, Alloc = Alloc()) -> deque<iter_val_t<Iterator>, Alloc>;
#endif

template <typename T, typename Alloc>
MSTL_NODISCARD bool operator ==(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return lh.size() == rh.size() && MSTL::equal(lh.begin(), lh.end(), rh.begin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator !=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh == rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator <(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return MSTL::lexicographical_compare(lh.begin(), lh.end(), rh.begin(), rh.end());
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator >(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return rh < lh;
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator <=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh > rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator >=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh < rh);
}
template <typename T, typename Alloc>
void swap(deque<T, Alloc>& lh, deque<T, Alloc>& rh) noexcept {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__
#endif // MSTL_DEQUE_HPP__

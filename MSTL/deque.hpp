#ifndef MSTL_DEQUE_HPP__
#define MSTL_DEQUE_HPP__
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

static constexpr size_t MAX_DEQUE_BUFFER_SIZE = 256;

constexpr size_t deque_buf_size(const size_t n, const size_t sz) noexcept {
    return n != 0 ? n : sz < MAX_DEQUE_BUFFER_SIZE ? MAX_DEQUE_BUFFER_SIZE / sz : 1;
}

template <typename T, typename Alloc, size_t BufSize>
class deque;

template <bool IsConst, typename Deque, size_t BufSize = 0>
struct deque_iterator {
private:
    using container_type	= Deque;
    using iterator			= deque_iterator<false, container_type>;
    using const_iterator	= deque_iterator<true, container_type>;

public:
    using iterator_category = random_access_iterator_tag;
    using value_type		= typename container_type::value_type;
    using reference			= conditional_t<IsConst, typename container_type::const_reference, typename container_type::reference>;
    using pointer			= conditional_t<IsConst, typename container_type::const_pointer, typename container_type::pointer>;
    using difference_type	= typename container_type::difference_type;
    using size_type			= typename container_type::size_type;

    using self              = deque_iterator<IsConst, Deque, BufSize>;

private:
    pointer cur_ = nullptr;
    pointer first_ = nullptr;
    pointer last_ = nullptr;
    pointer* node_ = nullptr;
    const container_type* deq_ = nullptr;

    template <typename, typename, size_t> friend class deque;
    template <bool, typename, size_t> friend struct deque_iterator;

private:
    static constexpr size_t buff_size() noexcept {
        return _MSTL deque_buf_size(BufSize, sizeof(value_type));
    }

    void change_buff(pointer* new_map) noexcept {
        node_ = new_map;
        first_ = *new_map;
        last_ = first_ + static_cast<difference_type>(buff_size());
    }

public:
    deque_iterator() noexcept = default;
    deque_iterator(pointer cur, pointer first, pointer last, pointer* node, const container_type* deq) noexcept
    : cur_(const_cast<pointer>(cur)), first_(const_cast<pointer>(first)),
    last_(const_cast<pointer>(last)), node_(const_cast<pointer*>(node)), deq_(deq) {}

    deque_iterator(const iterator& x) noexcept
    : cur_(const_cast<pointer>(x.cur_)), first_(const_cast<pointer>(x.first_)),
    last_(const_cast<pointer>(x.last_)), node_(const_cast<pointer*>(x.node_)), deq_(x.deq_) {}

    self& operator =(const iterator& x) noexcept {
        if (_MSTL addressof(x) == this) return *this;
        cur_ = const_cast<pointer>(x.cur_);
        first_ = const_cast<pointer>(x.first_);
        last_ = const_cast<pointer>(x.last_);
        node_ = const_cast<pointer*>(x.node_);
        deq_ = x.deq_;
        return *this;
    }

    deque_iterator(iterator&& x) noexcept
    : cur_(const_cast<pointer>(x.cur_)), first_(const_cast<pointer>(x.first_)),
    last_(const_cast<pointer>(x.last_)), node_(const_cast<pointer*>(x.node_)), deq_(x.deq_) {
        x.cur_ = nullptr;
        x.first_ = nullptr;
        x.last_ = nullptr;
        x.node_ = nullptr;
        x.deq_ = nullptr;
    }

    self& operator =(iterator&& x) noexcept {
        if (_MSTL addressof(x) == this) return *this;
        cur_ = const_cast<pointer>(x.cur_);
        first_ = const_cast<pointer>(x.first_);
        last_ = const_cast<pointer>(x.last_);
        node_ = const_cast<pointer*>(x.node_);
        deq_ = x.deq_;
        x.cur_ = nullptr;
        x.first_ = nullptr;
        x.last_ = nullptr;
        x.node_ = nullptr;
        x.deq_ = nullptr;
        return *this;
    }

    deque_iterator(const const_iterator& x) noexcept
    : cur_(const_cast<pointer>(x.cur_)), first_(const_cast<pointer>(x.first_)),
    last_(const_cast<pointer>(x.last_)), node_(const_cast<pointer*>(x.node_)), deq_(x.deq_) {}

    self& operator =(const const_iterator& x) noexcept {
        if (_MSTL addressof(x) == this) return *this;
        cur_ = const_cast<pointer>(x.cur_);
        first_ = const_cast<pointer>(x.first_);
        last_ = const_cast<pointer>(x.last_);
        node_ = const_cast<pointer*>(x.node_);
        deq_ = x.deq_;
        return *this;
    }

    deque_iterator(const_iterator&& x) noexcept
    : cur_(const_cast<pointer>(x.cur_)), first_(const_cast<pointer>(x.first_)),
    last_(const_cast<pointer>(x.last_)), node_(const_cast<pointer*>(x.node_)), deq_(x.deq_) {
        x.cur_ = nullptr;
        x.first_ = nullptr;
        x.last_ = nullptr;
        x.node_ = nullptr;
        x.deq_ = nullptr;
    }
    self& operator =(const_iterator&& x) noexcept {
        if (_MSTL addressof(x) == this) return *this;
        cur_ = const_cast<pointer>(x.cur_);
        first_ = const_cast<pointer>(x.first_);
        last_ = const_cast<pointer>(x.last_);
        node_ = const_cast<pointer*>(x.node_);
        deq_ = x.deq_;
        x.cur_ = nullptr;
        x.first_ = nullptr;
        x.last_ = nullptr;
        x.node_ = nullptr;
        x.deq_ = nullptr;
        return *this;
    }

    ~deque_iterator() noexcept = default;

    MSTL_NODISCARD reference operator *() const noexcept {
        MSTL_DEBUG_VERIFY__(cur_ && deq_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(deque_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
        MSTL_DEBUG_VERIFY__(first_ <= cur_ && cur_ < last_, __MSTL_DEBUG_MESG_OUT_OF_RANGE(deque_iterator, __MSTL_DEBUG_TAG_DEREFERENCE));
        return *cur_;
    }
    MSTL_NODISCARD pointer operator ->() const noexcept {
        return &operator*();
    }

    self& operator ++() noexcept {
        MSTL_DEBUG_VERIFY__(cur_ && deq_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(deque_iterator, __MSTL_DEBUG_TAG_INCREMENT));
        MSTL_DEBUG_VERIFY__(cur_ < last_ ||
            (cur_ + 1 == last_ && node_ + 1 < deq_->map_pair_.value + deq_->map_size_pair_.value && *(node_ + 1) < deq_->finish_.cur_),
             __MSTL_DEBUG_MESG_OUT_OF_RANGE(deque_iterator, __MSTL_DEBUG_TAG_INCREMENT));
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
        MSTL_DEBUG_VERIFY__(cur_ && deq_, __MSTL_DEBUG_MESG_OPERATE_NULLPTR(deque_iterator, __MSTL_DEBUG_TAG_DECREMENT));
        MSTL_DEBUG_VERIFY__(first_ < cur_ || (cur_ == first_ && node_ - 1 == deq_->map_pair_.value),
            __MSTL_DEBUG_MESG_OUT_OF_RANGE(deque_iterator, __MSTL_DEBUG_TAG_DECREMENT));
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

    self& operator +=(const difference_type n) noexcept {
        const difference_type offset = n + (cur_ - first_);
        if (offset >= 0 && offset < static_cast<difference_type>(buff_size()))
            cur_ += n;
        else {
            difference_type node_offset = offset > 0 ?
                offset / static_cast<difference_type>(buff_size())
                : -static_cast<difference_type>((-offset - 1) / buff_size()) - 1;
            change_buff(node_ + node_offset);
            cur_ = first_ + (offset - node_offset * static_cast<difference_type>(buff_size()));
        }
        return *this;
    }
    MSTL_NODISCARD self operator +(const difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }
    MSTL_NODISCARD friend self operator +(const difference_type n, const self& it) {
        return it + n;
    }

    self& operator -=(const difference_type n) noexcept {
        return *this += -n;
    }
    MSTL_NODISCARD self operator -(const difference_type n) const noexcept {
        self temp = *this;
        return temp -= n;
    }
    difference_type operator -(const self& x) const noexcept {
		MSTL_DEBUG_VERIFY__(deq_ == x.deq_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(deque_iterator));
        return (node_ - x.node_ - 1) * buff_size() + (cur_ - first_) + (x.last_ - x.cur_);
    }

    MSTL_NODISCARD reference operator [](const difference_type n) noexcept { return *(*this + n); }

    MSTL_NODISCARD bool operator ==(const self& x) const noexcept {
		MSTL_DEBUG_VERIFY__(deq_ == x.deq_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(deque_iterator));
        return cur_ == x.cur_;
    }
    MSTL_NODISCARD bool operator !=(const self& x) const noexcept { return !(*this == x); }
    MSTL_NODISCARD bool operator <(const self& x) const noexcept {
		MSTL_DEBUG_VERIFY__(deq_ == x.deq_, __MSTL_DEBUG_MESG_CONTAINER_INCOMPATIBLE(deque_iterator));
        return node_ == x.node_ ? cur_ < x.cur_ : node_ < x.node_;
    }
    MSTL_NODISCARD bool operator >(const self& x) const noexcept { return x < *this; }
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
    MSTL_BUILD_TYPE_ALIAS(T)
    using allocator_type            = Alloc;
    using self                      = deque<T, Alloc, BufSize>;

    using iterator                  = deque_iterator<false, self, BufSize>;
    using const_iterator            = deque_iterator<true, self, BufSize>;
    using reverse_iterator          = _MSTL reverse_iterator<iterator>;
    using const_reverse_iterator    = _MSTL reverse_iterator<const_iterator>;

private:
    using map_pointer   = pointer*;
    using map_allocator = typename Alloc::template rebind<pointer>::other;

    iterator start_{};
    iterator finish_{};
    compressed_pair<allocator_type, size_type> map_size_pair_{ default_construct_tag{}, 0 };
    compressed_pair<map_allocator, map_pointer> map_pair_{ default_construct_tag{}, nullptr };

    template <bool, typename, size_t> friend struct deque_iterator;

private:
    void range_check(const size_type position) const noexcept {
        MSTL_DEBUG_VERIFY__(position < static_cast<size_type>(distance(start_, finish_)),
            "deque index out of ranges.");
    }
    void range_check(iterator position) const noexcept {
        MSTL_DEBUG_VERIFY__(position >= start_ && position <= finish_, "deque index out of ranges.");
    }

    MSTL_CONSTEXPR static size_t buff_size() noexcept {
        return deque_buf_size(BufSize, sizeof(T));
    }

    void create_map_and_nodes(const size_type n) {
        size_type node_nums = n / buff_size() + 1;
        map_size_pair_.value = _MSTL max(static_cast<size_type>(8), node_nums + 2);
        map_pair_.value = map_pair_.get_base().allocate(map_size_pair_.value);
        map_pointer nstart = map_pair_.value + (map_size_pair_.value - node_nums) / 2;
        map_pointer nfinish = nstart + node_nums - 1;
        map_pointer cur;
        MSTL_TRY__{
            for (cur = nstart; cur <= nfinish; ++cur) *cur = map_size_pair_.get_base().allocate(buff_size());
        }
        MSTL_CATCH_UNWIND_THROW_M__(
            for (cur = nstart; cur <= nfinish; ++cur) map_size_pair_.get_base().deallocate(*cur, buff_size());
        );
        start_.change_buff(nstart);
        finish_.change_buff(nfinish);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_ + n % buff_size();
        start_.deq_ = this;
        finish_.deq_ = this;
    }

    void fill_initialize(const size_type n, const T& x) {
        create_map_and_nodes(n);
        map_pointer cur = nullptr;
        for (cur = start_.node_; cur < finish_.node_; ++cur)
            _MSTL uninitialized_fill(*cur, *cur + buff_size(), x);
        _MSTL uninitialized_fill(finish_.first_, finish_.cur_, x);
    }

    void reallocate_map(size_type nodes_to_add, const bool add_at_front) {
        size_type old_num_nodes = finish_.node_ - start_.node_ + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;
        map_pointer new_start;
        if (map_size_pair_.value > 2 * new_num_nodes) {
            new_start = map_pair_.value + (map_size_pair_.value - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            if (new_start < start_.node_) _MSTL copy(start_.node_, finish_.node_ + 1, new_start);
            else _MSTL copy_backward(start_.node_, finish_.node_ + 1, new_start + old_num_nodes);
        }
        else {
            size_type new_map_size = map_size_pair_.value + _MSTL max(map_size_pair_.value, nodes_to_add) + 2;
            map_pointer new_map = map_pair_.get_base().allocate(new_map_size);
            new_start = new_map + (new_map_size - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
            _MSTL copy(start_.node_, finish_.node_ + 1, new_start);
            map_pair_.get_base().deallocate(map_pair_.value, map_size_pair_.value);
            map_pair_.value = new_map;
            map_size_pair_.value = new_map_size;
        };
        start_.change_buff(new_start);
        finish_.change_buff(new_start + old_num_nodes - 1);
    }

    template <typename Iterator,
        enable_if_t<is_input_iter_v<Iterator>, int> = 0>
    iterator range_insert(iterator position, Iterator first, Iterator last) {
        difference_type n = last - first;
        difference_type index = position - start_;
        if (index < size() / 2) {
            if (n <= start_.cur_ - start_.first_) {
                iterator new_start = start_ - n;
                _MSTL copy(start_, position, new_start);
                position = new_start + index;
                _MSTL copy(first, last, position);
                start_ = new_start;
            }
            else {
                size_type needs =
                    static_cast<difference_type>(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = map_size_pair_.get_base().allocate(buff_size());
                    _MSTL uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_start = start_ - n;
                _MSTL copy(start_, position, new_start);
                position = new_start + index;
                _MSTL copy(first, last, position);
                start_ = new_start;
            }
        }
        else {
            if (finish_.last_ - finish_.cur_ - 1 >= n) {
                iterator new_finish = finish_ + n;
                _MSTL copy_backward(position, finish_, new_finish);
                _MSTL copy(first, last, position);
                finish_ = new_finish;
                position = start_ + index;
            }
            else {
                const difference_type left = finish_.last_ - finish_.cur_ - 1;
                size_type needs = (n - left - 1) / buff_size() + 1;
                reserve_back(needs);
                map_pointer cur = finish_.node_ + 1;
                for (; needs > 0; ++cur, --needs) {
                    *cur = map_size_pair_.get_base().allocate(buff_size());
                    _MSTL uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_finish = finish_ + n;
                _MSTL copy_backward(position, finish_, new_finish);
                _MSTL copy(first, last, position);
                finish_ = new_finish;
                position = start_ + index;
            }
        }
        return position;
    }

    iterator range_insert(iterator position, size_type n, const T& x) {
        difference_type index = position - start_;
        if (index < static_cast<difference_type>(size() / 2)) {
            if (n <= static_cast<size_type>(start_.cur_ - start_.first_)) {
                iterator new_start = start_ - n;
                _MSTL copy(start_, position, new_start);
                position = new_start + index;
                _MSTL fill_n(position, n, x);
                start_ = new_start;
            }
            else {
                size_type needs =
                    static_cast<difference_type>(n - (start_.cur_ - start_.first_) - 1) / buff_size() + 1;
                reserve_front(needs);
                map_pointer cur = start_.node_ - 1;
                for (; needs > 0; --cur, --needs) {
                    *cur = map_size_pair_.get_base().allocate(buff_size());
                    _MSTL uninitialized_fill_n(*cur, buff_size(), x);
                }
                iterator new_start = start_ - n;
                _MSTL copy(start_, position, new_start);
                position = new_start + index;
                _MSTL fill_n(position, n, x);
                start_ = new_start;
            }
        }
        else {
            if (n <= static_cast<size_type>(finish_.last_ - finish_.cur_ - 1)) {
                iterator new_finish = finish_ + n;
                _MSTL copy_backward(position, finish_, new_finish);
                _MSTL fill_n(position, n, x);
                finish_ = new_finish;
                position = start_ + index;
            }
            else {
                const difference_type left = finish_.last_ - finish_.cur_ - 1;
                size_type needs = static_cast<difference_type>(n - left - 1) / buff_size() + 1;
                reserve_back(needs);
                map_pointer cur = finish_.node_ + 1;
                for (; needs > 0; ++cur, --needs) {
                    *cur = map_size_pair_.get_base().allocate(buff_size());
                    _MSTL uninitialized_fill_n(*cur, buff_size(), *start_);
                }
                iterator new_finish = finish_ + n;
                _MSTL copy_backward(position, finish_, new_finish);
                _MSTL fill_n(position, n, x);
                finish_ = new_finish;
                position = start_ + index;
            }
        }
        return position;
    }

    template <typename... Args>
    iterator emplace_aux(iterator position, Args&&... args)
        noexcept(is_nothrow_move_assignable_v<value_type>) {
        size_type index = _MSTL distance(start_, position);
        if (index < size() / 2) {
            (emplace_front)(front());
            iterator front1 = start_;
            ++front1;
            iterator front2 = front1;
            ++front2;
            position = start_ + index;
            iterator pos1 = position;
            ++pos1;
            _MSTL copy(front2, pos1, front1);
        }
        else {
            push_back(back());
            iterator back1 = finish_;
            --back1;
            iterator back2 = back1;
            --back2;
            position = start_ + index;
            _MSTL copy_backward(position, back2, back1);
        }
        value_type val(_MSTL forward<Args>(args)...);
        *position = _MSTL move(val);
        return position;
    }

public:
    deque() {
        map_pair_.value = map_pair_.get_base().allocate(1);
        *map_pair_.value = map_size_pair_.get_base().allocate(buff_size());
        start_.change_buff(map_pair_.value);
        finish_.change_buff(map_pair_.value);
        start_.cur_ = start_.first_;
        finish_.cur_ = finish_.first_;
        start_.deq_ = this;
        finish_.deq_ = this;
    }

    explicit deque(const size_type n) {
        (fill_initialize)(n, T());
    }

    deque(const size_type n, const T& x) {
        (fill_initialize)(n, x);
    }
    deque(const size_type n, T&& x) {
        (fill_initialize)(n, _MSTL forward<T>(x));
    }

    template <typename Iterator, enable_if_t<is_iter_v<Iterator>, int> = 0>
    deque(Iterator first, Iterator last) {
        Exception(_MSTL distance(first, last) >= 0,
            StopIterator("deque iterator-constructor out of ranges."));
        create_map_and_nodes(_MSTL distance(first, last));
        for (map_pointer cur = start_.node_; cur < finish_.node_; ++cur) {
            _MSTL uninitialized_copy(first, first + buff_size(), *cur);
            _MSTL advance(first, buff_size());
        }
        _MSTL uninitialized_copy(first, last, finish_.first_);
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
        if (_MSTL addressof(x) == this) return *this;
        clear();
        insert(end(), x.cbegin(), x.cend());
        return *this;
    }

    deque(self&& x) noexcept {
        swap(x);
    }
    self& operator =(self&& x) noexcept {
        if (_MSTL addressof(x) == this) return *this;
        clear();
        swap(x);
        return *this;
    }

    ~deque() {
        clear();
        if (map_pair_.value) _MSTL destroy(map_pair_.value);
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
        if (map_size_pair_.value - (finish_.node_ - map_pair_.value) - 1 < nodes_to_add)
            reallocate_map(nodes_to_add, false);
    }
    void reserve_front(size_type nodes_to_add = 1) {
        if (size_type(start_.node_ - map_pair_.value) < nodes_to_add)
            reallocate_map(nodes_to_add, true);
    }

    void resize(size_type new_size, const T& x) {
        if (new_size < size()) erase(start_ + new_size, finish_);
        else insert(finish_, new_size - size(), x);
    }
    void resize(const size_type new_size) { resize(new_size, T()); }

    template <typename... Args>
    iterator emplace(iterator position, Args&& ...args) {
        range_check(position);
        if (position.cur_ == start_.cur_) {
            (emplace_front)(_MSTL forward<Args>(args)...);
            return start_;
        }
        if (position.cur_ == finish_.cur_) {
            (emplace_back)(_MSTL forward<Args>(args)...);
            return finish_ - 1;
        }
        return (emplace_aux)(position, _MSTL forward<Args>(args)...);
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (finish_.cur_ != finish_.last_ - 1) {
            _MSTL construct(finish_.cur_, _MSTL forward<Args>(args)...);
            ++finish_.cur_;
        }
        else {
            reserve_back();
            *(finish_.node_ + 1) = map_size_pair_.get_base().allocate(buff_size());
            _MSTL construct(finish_.cur_, _MSTL forward<Args>(args)...);
            finish_.change_buff(finish_.node_ + 1);
            finish_.cur_ = finish_.first_;
        }
    }
    template <typename... Args>
    void emplace_front(Args&&... args) {
        if (start_.cur_ != start_.first_) {
            _MSTL construct(start_.cur_ - 1, _MSTL forward<Args>(args)...);
            --start_.cur_;
        }
        else {
            reserve_front();
            *(start_.node_ - 1) = map_size_pair_.get_base().allocate(buff_size());
            start_.change_buff(start_.node_ - 1);
            start_.cur_ = start_.last_ - 1;
            _MSTL construct(start_.cur_, _MSTL forward<Args>(args)...);
        }
    }

    void push_back(const T& x) {
        (emplace_back)(x);
    }
    void push_front(const T& x) {
        (emplace_front)(x);
    }
    void push_back(T&& x) {
        (emplace_back)(_MSTL forward<T>(x));
    }
    void push_front(T&& x) {
        (emplace_front)(_MSTL forward<T>(x));
    }

    void pop_back() noexcept {
        if (finish_.cur_ != finish_.first_) {
            --finish_.cur_;
            _MSTL destroy(finish_.cur_);
        }
        else {
            map_size_pair_.get_base().deallocate(finish_.first_, buff_size());
            finish_.change_buff(finish_.node_ - 1);
            finish_.cur_ = finish_.last_ - 1;
            _MSTL destroy(finish_.cur_);
        }
    }
    void pop_front() noexcept {
        if (start_.cur_ != start_.last_ - 1) {
            _MSTL destroy(start_.cur_);
            ++start_.cur_;
        }
        else {
            _MSTL destroy(start_.cur_);
            map_size_pair_.get_base().deallocate(start_.first_, buff_size());
            start_.change_buff(start_.node_ + 1);
            start_.cur_ = start_.first_;
        }
    }

    void assign(const size_type count, const T& value) {
        clear();
        insert(begin(), count, value);
    }

    template <typename Iterator, enable_if_t<
        is_iter_v<Iterator>, int> = 0>
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
        return (emplace)(position, _MSTL forward<T>(x));
    }
    iterator insert(iterator position) {
        return (emplace)(position, T());
    }

    template <typename Iterator, enable_if_t<
        is_input_iter_v<Iterator>, int> = 0>
    iterator insert(iterator position, Iterator first, Iterator last) {
        range_check(position);
        Exception(_MSTL distance(first, last) >= 0, StopIterator("deque insert out of ranges."));
        if (position == start_) {
            for (--last; first != last; --last)
                (emplace_front)(*last);
            (emplace_front)(*first);
            return start_;
        }
        if (position == finish_) {
            for (; first != last; ++first)
                (emplace_back)(*first);
            return finish_ - 1;
        }
        return range_insert(position, first, last);
    }
    iterator insert(iterator position, std::initializer_list<T> l) {
        return insert(position, l.begin(), l.end());
    }
    iterator insert(iterator position, size_t n, const T& x) {
        range_check(position);
        if (position == start_) {
            for (size_t i = 0; i < n; i++)
                (emplace_front)(x);
            return start_;
        }
        if (position == finish_) {
            for (size_t i = 0; i < n; i++)
                (emplace_back)(x);
            return finish_ - 1;
        }
        return range_insert(position, n, x);
    }

    iterator erase(iterator position) noexcept {
        range_check(position);
        iterator next = position;
        ++next;
        difference_type n = position - start_;
        if (n < size() / 2) {
            _MSTL copy_backward(start_, position, next);
            pop_front();
        }
        else {
            _MSTL copy(next, finish_, position);
            pop_back();
        }
        return start_ + n;
    }
    iterator erase(iterator first, iterator last) noexcept {
        Exception(_MSTL distance(first, last) >= 0, StopIterator("deque erase out of ranges."));
        if (first == start_ && last == finish_) {
            clear();
            return finish_;
        }
        difference_type len = last - first;
        difference_type n = first - start_;
        if (n < static_cast<difference_type>((size() - len) / 2)) {
            _MSTL copy_backward(start_, first, last);
            iterator new_start = start_ + len;
            _MSTL destroy(start_, new_start);
            for (map_pointer cur = start_.node_; cur < new_start.node_; ++cur)
                map_size_pair_.get_base().deallocate(*cur, buff_size());
            start_ = new_start;
        }
        else {
            _MSTL copy(last, finish_, first);
            iterator new_finish = finish_ - len;
            _MSTL destroy(new_finish, finish_);
            for (map_pointer cur = new_finish.node_ + 1; cur <= finish_.node_; ++cur)
                map_size_pair_.get_base().deallocate(*cur, buff_size());
            finish_ = new_finish;
        }
        return start_ + n;
    }

    void clear() noexcept {
        map_pointer cur = start_.node_;
        for (++cur; cur < finish_.node_; ++cur) {
            _MSTL destroy(*cur, *cur + buff_size());
            map_size_pair_.get_base().deallocate(*cur, buff_size());
        }
        if (start_.node_ == finish_.node_) _MSTL destroy(start_.cur_, finish_.cur_);
        else {
            _MSTL destroy(finish_.first_, finish_.cur_);
            map_size_pair_.get_base().deallocate(finish_.first_, buff_size());
            _MSTL destroy(start_.cur_, start_.last_);
        }
        finish_ = start_;
    }

    void swap(self& x) noexcept {
        if (_MSTL addressof(x) == this) return;
        _MSTL swap(start_, x.start_);
        _MSTL swap(finish_, x.finish_);
        map_pair_.swap(x.map_pair_);
        map_size_pair_.swap(x.map_size_pair_);
    }

    MSTL_NODISCARD const_reference at(size_type position) const {
        range_check(position);
        return const_iterator(start_)[position];
    }
    MSTL_NODISCARD reference at(const size_type position) {
        return const_cast<reference>(static_cast<const self*>(this)->at(position));
    }
    MSTL_NODISCARD const_reference operator [](const size_type position) const {
        return this->at(position);
    }
    MSTL_NODISCARD reference operator [](const size_type position) {
        return this->at(position);
    }
};
#if MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Iterator, typename Alloc>
deque(Iterator, Iterator, Alloc = Alloc()) -> deque<iter_val_t<Iterator>, Alloc>;
#endif

template <typename T, typename Alloc>
MSTL_NODISCARD bool operator ==(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return lh.size() == rh.size() && _MSTL equal(lh.begin(), lh.end(), rh.begin());
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator !=(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return !(lh == rh);
}
template <typename T, typename Alloc>
MSTL_NODISCARD bool operator <(const deque<T, Alloc>& lh, const deque<T, Alloc>& rh) {
    return _MSTL lexicographical_compare(lh.begin(), lh.end(), rh.begin(), rh.end());
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

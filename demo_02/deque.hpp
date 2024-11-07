#ifndef DEQUE_H
#define DEQUE_H
#include <memory>
#include <iterator>
#include "iterable.h"
#include "algobase.hpp"
#include "alloc.h"

namespace MSTL {
    inline size_t deque_buf_size(size_t n, size_t sz) {
        return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
    }

    template <typename T, typename Ref = T&, typename Ptr = T*, size_t BufSize = 0>
    struct deque_iterator{
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

        link_type cur;
        link_type first;
        link_type last;
        map_pointer node;

        static size_t buff_size() {
            return deque_buf_size(BufSize, sizeof(T));
        }
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buff_size());
        }

        deque_iterator(link_type cur = nullptr, link_type first = nullptr,
            link_type last = nullptr, map_pointer node = nullptr) :
            cur(cur), first(first), last(last), node(node) {}
        deque_iterator(const iterator& x) :
            cur(x.cur), first(x.first), last(x.last), node(x.node) {}
        ~deque_iterator() = default;

        reference operator *()const { return *cur; }
        pointer operator ->()const { return &(operator*()); }
        difference_type operator -(const self& x)const {
            return difference_type((node - x.node - 1) * buff_size() + (cur - first) + (x.last - x.cur));
        }
        self& operator ++() {
            ++cur;
            if (cur == last) {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }
        self operator ++(int) {
            self temp = *this;
            ++*this;
            return temp;
        }
        self& operator --() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }
        self operator --(int) {
            self temp = *this;
            --*this;
            return temp;
        }
        self& operator +=(difference_type n) {
            difference_type all = (cur - first) + n;
            if (n >= 0) {
                if (all >= buff_size()) {
                    difference_type node_num = all / buff_size();
                    difference_type node_left = all - node_num * buff_size();
                    node = set_node(node + node_num);
                    cur = first + node_left;
                }
                else {
                    cur = cur + n;
                }
            }
            else {
                if (all < 0) {
                    difference_type node_num = (-all) / buff_size() + 1;
                    difference_type node_left = (-all) - (node_num - 1) * buff_size();
                    node = set_node(node - node_num);
                    cur = last - node_left - 1;
                }
                else {
                    cur = cur - n;
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
        bool operator ==(const self& x)const { return cur == x.cur; }
        bool operator !=(const self& x)const { return !(*this == x); }
        bool operator <(const self& x)const { return node == x.node ? (cur < x.cur) : (node < x.node); }
    };
    
    template<typename T, typename Alloc = std::allocator<T>, size_t BufSize = 0>
    class deque : private siterable {
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

        static const char* const __type__;

    private:
        typedef pointer*                    map_pointer;
        typedef simple_alloc<value_type>    data_allocator;
        typedef simple_alloc<pointer>       map_allocator;

        data_allocator data_alloc;
        map_allocator map_alloc;
        map_pointer map;
        size_type map_size;
        iterator start;
        iterator finish;

        static size_t buff_size() {
            return deque_buf_size(BufSize, sizeof(T));
        }
        void __create_map_and_nodes(size_type n) {
            size_type node_nums = n / buff_size() + 1;
            map_size = max(size_type(8), node_nums + 2);
            map = map_alloc.allocate(map_size);
            map_pointer nstart = map + (map_size - node_nums) / 2;
            map_pointer nfinish = nstart + node_nums - 1;
            map_pointer cur;
            try {
                for (cur = nstart; cur <= nfinish; ++cur) *cur = data_alloc.allocate(buff_size());
            }
            catch (...) {
                Exception(error_map[__error::MemErr]);
            }
            start.set_node(nstart);
            finish.set_node(nfinish);
            start.cur = start.first;
            finish.cur = finish.first + n % buff_size();
        }
        void __fill_initialize(size_type n, reference_const x) {
            __create_map_and_nodes(n);
            map_pointer cur;
            for (cur = start.node; cur < finish.node; ++cur) 
                std::uninitialized_fill(*cur, *cur + buff_size(), x);
            std::uninitialized_fill(finish.first, finish.cur, x);
        }
        void __reallocate_map(size_type nodes_to_add, bool add_at_front) {
            size_type old_num_nodes = finish.node - start.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;
            map_pointer new_start;
            if (map_size > 2 * new_num_nodes) {
                new_start = map + (map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
                if (new_start < start.node) copy(start.node, finish.node + 1, new_start);
                else copy_backward(start.node, finish.node + 1, new_start + old_num_nodes);
            }
            else {
                size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
                map_pointer new_map = map_alloc.allocate(new_map_size);
                new_start = new_map + (new_map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
                copy(start.node, finish.node + 1, new_start);
                map_alloc.deallocate(map, map_size);
                map = new_map;
                map_size = new_map_size;
            };
            start.set_node(new_start);
            finish.set_node(new_start + old_num_nodes - 1);
        }
        void __push_back_aux(reference_const x) {
            value_type x_copy = x;
            reserve_map_at_back();
            *(finish.node + 1) = data_alloc.allocate(buff_size());
            construct(finish.cur, x_copy);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        }
        void __push_front_aux(reference_const x) {
            value_type x_copy = x;
            reserve_map_at_front();
            *(start.node - 1) = data_alloc.allocate(buff_size());
            start.set_node(start.node - 1);
            start.cur = start.last - 1;
            construct(start.cur, x_copy);
        }
        void __pop_back_aux() {
            data_alloc.deallocate(finish.first, buff_size());
            finish.set_node(finish.node - 1);
            finish.cur = finish.last - 1;
            destroy(finish.cur);
        }
        void __pop_front_aux() {
            destroy(start.cur);
            data_alloc.deallocate(start.first, buff_size());
            start.set_node(start.node + 1);
            start.cur = start.first;
        }
        iterator __insert_aux(iterator pos, reference_const x) {
            difference_type index = pos - start;
            value_type x_copy = x;
            if (index < size() / 2) {
                push_front(front());
                iterator front1 = start;
                ++front1;
                iterator front2 = front1;
                ++front2;
                pos = start + index;
                iterator pos1 = pos;
                ++pos1;
                copy(front2, pos1, front1);
            }
            else {
                push_back(back());
                iterator back1 = finish;
                --back1;
                iterator back2 = back1;
                --back2;
                pos = start + index;
                copy_backward(pos, back2, back1);
            }
            *pos = x_copy;
            return pos;
        }
        template <typename InputIterator>
        iterator __insert_aux(iterator pos, InputIterator first, InputIterator last) {
            difference_type n = last - first;
            difference_type index = pos - start;
            if (index < size() / 2) {
                if (n <= start.cur - start.first) {
                    iterator new_start = start - n;
                    copy(start, pos, new_start);
                    pos = new_start + index;
                    copy(first, last, pos);
                    start = new_start;
                }
                else {
                    size_type needs =
                        difference_type(n - (start.cur - start.first) - 1) / buff_size() + 1;
                    reserve_map_at_front(needs);
                    map_pointer cur = start.node - 1;
                    for (; needs > 0; --cur, --needs) {
                        *cur = data_alloc.allocate(buff_size());
                        std::uninitialized_fill_n(*cur, buff_size(), *start);
                    }
                    iterator new_start = start - n;
                    copy(start, pos, new_start);
                    pos = new_start + index;
                    copy(first, last, pos);
                    start = new_start;
                }
            }
            else {
                if (finish.last - finish.cur - 1 >= n) {
                    iterator new_finish = finish + n;
                    copy_backward(pos, finish, new_finish);
                    copy(first, last, pos);
                    finish = new_finish;
                    pos = start + index;
                }
                else {
                    difference_type left = finish.last - finish.cur - 1;
                    size_type needs = difference_type(n - left - 1) / buff_size() + 1;
                    reserve_map_at_back(needs);
                    map_pointer cur = finish.node + 1;
                    for (; needs > 0; ++cur, --needs) {
                        *cur = data_alloc.allocate(buff_size());
                        std::uninitialized_fill_n(*cur, buff_size(), *start);
                    }
                    iterator new_finish = finish + n;
                    copy_backward(pos, finish, new_finish);
                    copy(first, last, pos);
                    finish = new_finish;
                    pos = start + index;
                }
            }
            return pos;
        }
        iterator __insert_aux(iterator pos, size_type n, reference_const x) {
            difference_type index = pos - start;
            if (index < size() / 2) {
                if (n <= start.cur - start.first) {
                    iterator new_start = start - n;
                    copy(start, pos, new_start);
                    pos = new_start + index;
                    fill_n(pos, n, x);
                    start = new_start;
                }
                else {
                    size_type needs =
                        difference_type(n - (start.cur - start.first) - 1) / buff_size() + 1;
                    reserve_map_at_front(needs);
                    map_pointer cur = start.node - 1;
                    for (; needs > 0; --cur, --needs) {
                        *cur = data_alloc.allocate(buff_size());
                        std::uninitialized_fill_n(*cur, buff_size(), x);
                    }
                    iterator new_start = start - n;
                    copy(start, pos, new_start);
                    pos = new_start + index;
                    fill_n(pos, n, x);
                    start = new_start;
                }
            }
            else {
                if (finish.last - finish.cur - 1 >= n) {
                    iterator new_finish = finish + n;
                    copy_backward(pos, finish, new_finish);
                    fill_n(pos, n, x);
                    finish = new_finish;
                    pos = start + index;
                }
                else {
                    difference_type left = finish.last - finish.cur - 1;
                    size_type needs = difference_type(n - left - 1) / buff_size() + 1;
                    reserve_map_at_back(needs);
                    map_pointer cur = finish.node + 1;
                    for (; needs > 0; ++cur, --needs) {
                        *cur = data_alloc.allocate(buff_size());
                        std::uninitialized_fill_n(*cur, buff_size(), *start);
                    }
                    iterator new_finish = finish + n;
                    copy_backward(pos, finish, new_finish);
                    fill_n(pos, n, x);
                    finish = new_finish;
                    pos = start + index;
                }
            }
            return pos;
        }
    public:
        inline void __show_data_only(std::ostream& _out) const {
            const_iterator _band = const_end();
            --_band;
            _out << '[' << std::flush;
            for (const_iterator iter = const_begin(); iter != const_end(); ++iter) {
                _out << *iter << std::flush;
                if (iter != _band) _out << ", " << std::flush;
            }
            _out << ']' << std::flush;
        }
        inline void __show_size_only(std::ostream& _out) const {
            _out << "size: " << this->map_size << " (map_size) " << std::endl;
        }
        void __det__(std::ostream& _out = std::cout) const {
            split_line(_out);
            _out << "type: " << __type__ << std::endl;
            this->__show_size_only(_out);
            _out << "data: " << std::flush;
            this->__show_data_only(_out);
            _out << std::endl;
            split_line(_out);
        }

        deque() : map(0), map_size(1), start(), finish(), data_alloc(), map_alloc() {
            map = map_alloc.allocate(1);
            *map = data_alloc.allocate(buff_size());
            start.set_node(map);
            finish.set_node(map);
            start.cur = start.first;
            finish.cur = finish.first;
        }
        deque(size_type n, reference_const x = T()) :
              map(0), map_size(0), start(), finish(), data_alloc(), map_alloc() {
            __fill_initialize(n, x); 
        }
        deque(const std::initializer_list<T>& _lls) : deque(_lls.begin(), _lls.end()) {}
        template<typename InputIterator>
        deque(InputIterator first, InputIterator last) :
              map(0), map_size(0), start(), finish(), data_alloc(), map_alloc() {
            difference_type n = last - first;
            __create_map_and_nodes(n);
            for (map_pointer cur = start.node; cur < finish.node; ++cur) {
                std::uninitialized_copy(first, first + buff_size(), *cur);
                first = first + buff_size();
            }
            std::uninitialized_copy(first, last, finish.first);
        }
        deque(self& x) : deque(x.begin(), x.end()) {}
        deque(self&& x) : map(x.map), map_size(x.map_size),
              start(x.start), finish(x.finish), data_alloc(), map_alloc() {
            x.map = 0;
            x.map_size = 0;
            x.finish = x.start;
        }

        iterator begin() { return iterator(start); }
        iterator end() { return iterator(finish); }
        const_iterator const_begin() const { return const_iterator(start); }
        const_iterator const_end() const { return const_iterator(finish); }
        reference front() { return *start; }
        reference back() { return *(finish - 1); }
        reference operator[](const size_t& n) {
            return start[difference_type(n)];
        }
        size_type size() const { return finish - start; }
        bool empty() const { return finish == start; }

        void reserve_map_at_back(size_type nodes_to_add = 1) {
            if (map_size - (finish.node - map) - 1 < nodes_to_add) __reallocate_map(nodes_to_add, false);
        }
        void reserve_map_at_front(size_type nodes_to_add = 1) {
            if (start.node - map < nodes_to_add) __reallocate_map(nodes_to_add, true);
        }

        void push_back(reference_const x) {
            if (finish.cur != finish.last - 1) {
                construct(finish.cur, x);
                ++finish.cur;
            }
            else __push_back_aux(x);
        }
        void push_front(const value_type& x) {
            if (start.cur != start.first) {
                construct(start.cur - 1, x);
                --start.cur;
            }
            else __push_front_aux(x);
        }
        void pop_back() {
            if (finish.cur != finish.first) {
                --finish.cur;
                destroy(finish.cur);
            }
            else __pop_back_aux();
        }
        void pop_front() {
            if (start.cur != start.last - 1) {
                destroy(start.cur);
                ++start.cur;
            }
            else __pop_front_aux();
        }
        void clear() {
            map_pointer cur = start.node;
            for (++cur; cur < finish.node; ++cur) {
                destroy(*cur, *cur + buff_size());
                data_alloc.deallocate(*cur, buff_size());
            }
            if (start.node == finish.node) destroy(start.cur, finish.cur);
            else {
                destroy(finish.first, finish.cur);
                data_alloc.deallocate(finish.first, buff_size());
                destroy(start.cur, start.last);
            }
            finish = start;
        }
        iterator erase(iterator pos) {
            iterator next = pos;
            ++next;
            difference_type n = pos - start;
            if (n < size() / 2) {
                copy_backward(start, pos, next);
                pop_front();
            }
            else {
                copy(next, finish, pos);
                pop_back();
            }
            return start + n;
        }
        iterator erase(iterator first, iterator last) {
            if (first == start && last == finish) {
                clear();
                return finish;
            }
            difference_type len = last - first;
            difference_type n = first - start;
            if (n < (size() - len) / 2) {
                copy_backward(start, first, last);
                iterator new_start = start + len;
                destroy(start, new_start);
                for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                    data_alloc.deallocate(*cur, buff_size());
                start = new_start;
            }
            else {
                copy(last, finish, first);
                iterator new_finish = finish - len;
                destroy(new_finish, finish);
                for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                    data_alloc.deallocate(*cur, buff_size());
                finish = new_finish;
            }
            return start + n;
        }
        template <typename InputIterator>
        iterator insert(iterator position, InputIterator first, InputIterator last) {
            if (position == start) {
                for (--last; first != last; --last) {
                    push_front(*last);
                }
                push_front(*first);
                return start;
            }
            else if (position == finish) {
                for (; first != last; ++first) push_back(*first);
                return finish - 1;
            }
            else return insert_aux(position, first, last);
        }
        iterator insert(iterator position, const int& n, reference_const x) {
            if (position == start) {
                for (int i = 0; i < n; i++) push_front(x);
                return start;
            }
            else if (position == finish) {
                for (int i = 0; i < n; i++) push_back(x);
                return finish - 1;
            }
            else return insert_aux(position, size_type(n), x);
            
        }
        iterator insert(iterator position, reference_const x) {
            if (position.cur == start.cur) {
                push_front(x);
                return start;
            }
            else if (position.cur == finish.cur) {
                push_back(x);
                return finish - 1;
            }
            else return insert_aux(position, x);
        }
    };
    template <typename T, typename Alloc, size_t BufSize>
    const char* const deque<T, Alloc, BufSize>::__type__ = "deque";
}
template <typename T, typename Alloc, size_t BufSize>
std::ostream& operator <<(std::ostream& _out, const MSTL::deque<T, Alloc, BufSize>& _deq) {
    _deq.__show_data_only(_out);
    return _out;
}

#endif // DEQUE_H

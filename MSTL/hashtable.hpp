#ifndef MSTL_HASHTABLE_HPP__
#define MSTL_HASHTABLE_HPP__
#include "basiclib.h"
#include "algo.hpp"
#include "memory.hpp"
#include "vector.hpp"
#include "concepts.hpp"
#include "hash_function.hpp"
#include "mathlib.h"
MSTL_BEGIN_NAMESPACE__
MSTL_CONCEPTS__

template <class T>
struct __hashtable_node {
    __hashtable_node* next_;
    T data_;
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey,
    class Alloc = standard_allocator<__hashtable_node<Value>>>
class hashtable;

template <class Value, class Key, class HashFcn,
    class ExtractKey, class EqualKey, class Alloc>
struct hashtable_const_iterator;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef __hashtable_node<Value> node;
    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur_;
    hashtable* ht_;

    hashtable_iterator(node* n, hashtable* tab) : cur_(n), ht_(tab) {}
    hashtable_iterator() : cur_(nullptr), ht_(nullptr) {}
    hashtable_iterator(const iterator& it) : cur_(it.cur_), ht_(it.ht_) {}
    MSTL_NODISCARD reference operator *() const noexcept { return cur_->data_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }
    iterator& operator ++() {
        const node* old = cur_;
        cur_ = cur_->next_;
        if (!cur_) {
            size_type bucket = ht_->bkt_num(old->data_);
            while (!cur_ && ++bucket < ht_->buckets_.size())
                cur_ = ht_->buckets_[bucket];
        }
        return *this;
    }
    iterator operator ++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
};
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
MSTL_NODISCARD bool operator ==(
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh, 
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh) 
noexcept(noexcept(lh.cur_ == rh.cur_)) {
    return lh.cur_ == rh.cur_;
}
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
MSTL_NODISCARD bool operator !=(
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
    noexcept(noexcept(!(lh == rh))) {
    return !(lh == rh);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct hashtable_const_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef __hashtable_node<Value> node;
    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef const Value& reference;
    typedef const Value* pointer;

    const node* cur_;
    const hashtable* ht_;

    hashtable_const_iterator(const node* n, const hashtable* tab)
        : cur_(n), ht_(tab) {}
    hashtable_const_iterator() noexcept : cur_(nullptr), ht_(nullptr) {}
    hashtable_const_iterator(const iterator& it) : cur_(it.cur_), ht_(it.ht_) {}
    MSTL_NODISCARD reference operator *() const noexcept { return cur_->data_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }
    const_iterator& operator ++() {
        const node* old = cur_;
        cur_ = cur_->next_;
        if (!cur_) {
            size_type bucket = ht_->bkt_num(old->data_);
            while (!cur_ && ++bucket < ht_->buckets_.size())
                cur_ = ht_->buckets_[bucket];
        }
        return *this;
    }
    const_iterator operator ++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
};
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
MSTL_NODISCARD bool operator ==(
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
    noexcept(noexcept(lh.cur_ == rh.cur_)) {
    return lh.cur_ == rh.cur_;
}
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
MSTL_NODISCARD bool operator !=(
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
    noexcept(noexcept(!(lh == rh))) {
    return !(lh == rh);
}

MSTL_NODISCARD inline size_t hashtable_next_prime(size_t n) noexcept {
    const mathui_t* first = PRIME_LIST;
    const mathui_t* last = PRIME_LIST + PRIMER_COUNT__;
    const mathui_t* pos = lower_bound(first, last, n);
    return pos == last ? size_t(*(last - 1)) : size_t(*pos);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef __hashtable_node<value_type> node_type;
    typedef node_type* link_type;
    typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> self;

    friend struct hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend struct hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend bool operator ==(const hashtable&, const hashtable&);

private:
    typedef typename value_type::second_type value_of_key;
    typedef Alloc alloc_type;

private:
    hasher hasher_;
    key_equal equals_;
    ExtractKey get_key_;
    vector<link_type> buckets_;
    size_type size_;
    alloc_type alloc_;

public:
    explicit hashtable(size_type n) 
        : hasher_(HashFcn()), equals_(EqualKey()), get_key_(ExtractKey()), size_(0) {}
    hashtable(size_type n, const HashFcn& hf)
        : hasher_(hf), equals_(EqualKey()), get_key_(ExtractKey()), size_(0) {
        initialize_buckets(n);
    }
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hasher_(hf), equals_(eql), get_key_(ExtractKey()), size_(0) {
        initialize_buckets(n);
    }
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql, const ExtractKey& ext)
        : hasher_(hf), equals_(eql), get_key_(ext), size_(0) {
        initialize_buckets(n);
    }
    hashtable(const self& ht)
        : hasher_(ht.hasher_), equals_(ht.equals_), get_key_(ht.get_key_), size_(0) {
        copy_from(ht);
    }
    self& operator =(const self& ht) {
        if (std::addressof(ht) != this) return *this;
        clear();
        hasher_ = ht.hasher_;
        equals_ = ht.equals_;
        get_key_ = ht.get_key_;
        copy_from(ht);
        return *this;
    }
    explicit hashtable(self&& ht) noexcept(noexcept(swap(ht)))
        : hasher_(ht.hasher_), equals_(ht.equals_), get_key_(ht.get_key_), size_(0) {
        swap(ht);
    }
    self& operator =(self&& ht) noexcept(noexcept(swap(ht))) {
        if (std::addressof(ht) == this) {
            ht.clear();
            return *this;
        }
        clear();
        swap(ht);
        return *this;
    }
    ~hashtable() { clear(); }

    MSTL_NODISCARD hasher hash_func() const noexcept(NothrowCopyConstructible<hasher>) {
        return hasher_;
    }
    MSTL_NODISCARD key_equal key_eql() const noexcept(NothrowCopyConstructible<key_equal>) {
        return equals_;
    }
    MSTL_NODISCARD size_type size() const noexcept { return size_; }
    MSTL_NODISCARD bool empty() const noexcept { return size_ == 0; }

    MSTL_NODISCARD iterator begin() noexcept {
        for (size_type n = 0; n < buckets_.size(); ++n)
            if (buckets_[n])
                return iterator(buckets_[n], this);
        return end();
    }
    MSTL_NODISCARD iterator end() noexcept { return iterator(nullptr, this); }

    MSTL_NODISCARD const_iterator cbegin() const noexcept {
        for (size_type n = 0; n < buckets_.size(); ++n) {
            if (buckets_[n]) 
                return const_iterator(buckets_[n], this);
        }
        return cend();
    }
    MSTL_NODISCARD const_iterator cend() const noexcept {
        return const_iterator(nullptr, this);
    }

    void swap(self& ht) noexcept(noexcept(std::swap(hasher_, ht.hasher_)) 
        && noexcept(std::swap(equals_, ht.equals_)) && noexcept(std::swap(get_key_, ht.get_key_))) {
        if (this == std::addressof(ht)) return;
        std::swap(hasher_, ht.hasher_);
        std::swap(equals_, ht.equals_);
        std::swap(get_key_, ht.get_key_);
        buckets_.swap(ht.buckets_);
        std::swap(size_, ht.size_);
    }

    MSTL_NODISCARD size_type buckets_size() const noexcept { return buckets_.size(); }
    MSTL_NODISCARD size_type max_buckets_size() const noexcept { 
        return PRIME_LIST[PRIMER_COUNT__ - 1];
    }
    MSTL_NODISCARD size_type bucket_count(size_type bucket) const noexcept {
        size_type result = 0;
        for (node_type* cur = buckets_[bucket]; cur; cur = cur->next_) 
            result++;
        return result;
    }
    MSTL_NODISCARD size_type count(const key_type& key) const noexcept {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for (const node_type* cur = buckets_[n]; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) ++result;
        }
        return result;
    }

    pair<iterator, bool> insert_unique(const value_type& x) {
        resize(size_ + 1);
        return insert_unique_noresize(x);
    }
    iterator insert_equal(const value_type& x) {
        resize(size_ + 1);
        return insert_equal_noresize(x);
    }
    pair<iterator, bool> insert_unique(value_type&& x) {
        resize(size_ + 1);
        return insert_unique_noresize(std::forward<value_type>(x));
    }
    iterator insert_equal(value_type&& x) {
        resize(size_ + 1);
        return insert_equal_noresize(std::forward<value_type>(x));
    }
    template <typename T = value_type> 
    pair<iterator, bool> insert_unique_noresize(T&& x) {
        const size_type n = bkt_num(x);
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), get_key_(x))) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                node_type* tmp = new_node(std::forward<T>(x));
                tmp->next_ = buckets_[n];
                buckets_[n] = tmp;
                return pair<iterator, bool>(iterator(tmp, this), false);
            }
        }
        node_type* tmp = new_node(std::forward<T>(x));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }
    
    template <typename T = value_type>
    iterator insert_equal_noresize(T&& x) {
        const size_type n = bkt_num(x);
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), get_key_(x))) {
                node_type* tmp = new_node(std::forward<T>(x));
                tmp->next_ = cur->next_;
                cur->next_ = tmp;
                ++size_;
                return iterator(tmp, this);
            }
        }
        node_type* tmp = new_node(std::forward<T>(x));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return iterator(tmp, this);
    }

    template <class Iterator>
        requires(InputIterator<Iterator>)
    void insert_unique(Iterator f, Iterator l) {
        for (; f != l; ++f) insert_unique(*f);
    }
    template <class Iterator>
        requires(InputIterator<Iterator>)
    void insert_equal(Iterator f, Iterator l) {
        for (; f != l; ++f) insert_equal(*f);
    }
    template <class Iterator>
        requires(ForwardIterator<Iterator>)
    void insert_unique(Iterator f, Iterator l) {
        size_type n = 0;
        MSTL::distance(f, l, n);
        resize(size_ + n);
        for (; n > 0; --n, ++f) insert_unique_noresize(*f);
    }
    template <class Iterator>
        requires(ForwardIterator<Iterator>)
    void insert_equal(Iterator f, Iterator l) {
        size_type n = 0;
        MSTL::distance(f, l, n);
        resize(size_ + n);
        for (; n > 0; --n, ++f) insert_equal_noresize(*f);
    }

    template <typename T = value_type> 
    reference find_or_insert(T&& x) {
        resize(size_ + 1);
        size_type n = bkt_num(x);
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), get_key_(x))) 
                return cur->data_;
        }
        node_type* tmp = new_node(std::forward<T>(x));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return tmp->data_;
    }

    MSTL_NODISCARD iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node_type* first;
        for (first = buckets_[n]; first && !equals_(get_key_(first->data_), key);
            first = first->next_);
        return iterator(first, this);
    }
    MSTL_NODISCARD const_iterator find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        const node_type* first;
        for (first = buckets_[n]; first && !equals_(get_key_(first->data_), key); 
            first = first->next_);
        return const_iterator(first, this);
    }

    template <typename... Args>
    pair<iterator, bool> emplace_unique(Args&&... args) {
        return insert_unique_noresize(std::move(value_type(args...)));
    }
    template <typename... Args>
    iterator emplace_equal(Args&&... args) {
        return insert_equal_noresize(std::move(value_type(args...)));
    }
    template <typename... Args>
    pair<iterator, bool> emplace_unique_pair(Args&&... args) {
        resize(size_ + 1);
        // structured binding
        auto [key, value_args] = MSTL::forward_as_tuple(std::forward<Args>(args)...);
        const size_type n = bkt_num_key(key, buckets_.size());
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                node_type* tmp = new_node(std::forward<decltype(key)>(key),
                    std::forward<decltype(value_args)>(value_args));
                tmp->next_ = buckets_[n];
                buckets_[n] = tmp;
                return pair<iterator, bool>(iterator(tmp, this), false);
            }
        }
        node_type* tmp = new_node(std::forward<decltype(key)>(key),
            std::forward<decltype(value_args)>(value_args));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }
    template <typename... Args>
    iterator emplace_equal_pair(Args&&... args) {
        resize(size_ + 1);
        auto [key, value_args] = MSTL::forward_as_tuple(std::forward<Args>(args)...);
        const size_type n = bkt_num_key(key, buckets_.size());
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) {
                node_type* tmp = new_node(std::forward<decltype(key)>(key),
                    std::forward<decltype(value_args)>(value_args));
                tmp->next_ = cur->next_;
                cur->next_ = tmp;
                ++size_;
                return iterator(tmp, this);
            }
        }
        node_type* tmp = new_node(std::forward<decltype(key)>(key),
            std::forward<decltype(value_args)>(value_args));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return iterator(tmp, this);
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        typedef pair<iterator, iterator> iter_pair;
        const size_type n = bkt_num_key(key);
        for (node_type* first = buckets_[n]; first; first = first->next_) {
            if (equals_(get_key_(first->data_), key)) {
                for (node_type* cur = first->next_; cur; cur = cur->next_) {
                    if (!equals_(get_key_(cur->data_), key))
                        return iter_pair(iterator(first, this), iterator(cur, this));
                }
                for (size_type m = n + 1; m < buckets_.size(); ++m) {
                    if (buckets_[m])
                        return iter_pair(iterator(first, this),
                            iterator(buckets_[m], this));
                }
                return iter_pair(iterator(first, this), end());
            }
        }
        return iter_pair(end(), end());
    }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        typedef pair<const_iterator, const_iterator> citer_pair;
        const size_type n = bkt_num_key(key);
        for (const node_type* first = buckets_[n]; first; first = first->next_) {
            if (equals_(get_key_(first->data_), key)) {
                for (const node_type* cur = first->next_; cur; cur = cur->next_) {
                    if (!equals_(get_key_(cur->data_), key))
                        return citer_pair(const_iterator(first, this), const_iterator(cur, this));
                }
                for (size_type m = n + 1; m < buckets_.size(); ++m) {
                    if (buckets_[m])
                        return citer_pair(const_iterator(first, this), const_iterator(buckets_[m], this));
                }
                return citer_pair(const_iterator(first, this), end());
            }
        }
        return citer_pair(end(), end());
    }

    size_type erase(const key_type& key) noexcept {
        const size_type n = bkt_num_key(key);
        node_type* first = buckets_[n];
        size_type erased = 0;
        if (first) {
            node_type* cur = first;
            node_type* next = cur->next_;
            while (next) {
                if (equals_(get_key_(next->data_), key)) {
                    cur->next_ = next->next_;
                    delete_node(next);
                    next = cur->next_;
                    ++erased;
                    --size_;
                }
                else {
                    cur = next;
                    next = cur->next_;
                }
            }
            if (equals_(get_key_(first->data_), key)) {
                buckets_[n] = first->next_;
                delete_node(first);
                ++erased;
                --size_;
            }
        }
        return erased;
    }
    void erase(const iterator& it) noexcept {
        if (node_type* const p = it.cur_) {
            const size_type n = bkt_num(p->data_);
            node_type* cur = buckets_[n];
            if (cur == p) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                --size_;
            }
            else {
                node_type* next = cur->next_;
                while (next) {
                    if (next == p) {
                        cur->next_ = next->next_;
                        delete_node(next);
                        --size_;
                        break;
                    }
                    else {
                        cur = next;
                        next = cur->next_;
                    }
                }
            }
        }
    }
    void erase(iterator first, iterator last) noexcept { // range optimize
        size_type f_bucket = first.cur_ ? bkt_num(first.cur_->data_) : buckets_.size();
        size_type l_bucket = last.cur_ ? bkt_num(last.cur_->data_) : buckets_.size();
        if (first.cur_ == last.cur_) return;
        else if (f_bucket == l_bucket)
            erase_bucket(f_bucket, first.cur_, last.cur_);
        else {
            erase_bucket(f_bucket, first.cur_, 0);
            for (size_type n = f_bucket + 1; n < l_bucket; ++n)
                erase_bucket(n, 0);
            if (l_bucket != buckets_.size())
                erase_bucket(l_bucket, last.cur_);
        }
    }

    void erase(const const_iterator& it) noexcept {
        erase(iterator(const_cast<node_type*>(it.cur_), const_cast<hashtable*>(it.ht_)));
    }
    void erase(const_iterator first, const_iterator last) noexcept {
        erase(iterator(const_cast<node_type*>(first.cur_), const_cast<hashtable*>(first.ht_)),
            iterator(const_cast<node_type*>(last.cur_), const_cast<hashtable*>(last.ht_)));
    }

    void resize(size_type new_size) {
        const size_type old_size = buckets_.size();
        if (new_size > old_size) {
            const size_type n = next_size(new_size);
            if (n > old_size) {
                vector<node_type*> tmp(n, nullptr);
                MSTL_TRY__{
                    for (size_type bucket = 0; bucket < old_size; ++bucket) {
                        node_type* first = buckets_[bucket];
                        while (first != nullptr) {
                            size_type new_bucket = bkt_num(first->data_, n);
                            buckets_[bucket] = first->next_;
                            first->next_ = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            first = buckets_[bucket];
                        }
                    }
                    buckets_.swap(tmp);
                }
                MSTL_CATCH_UNWIND__{
                    for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                        while (tmp[bucket] != nullptr) {
                            node_type* next = tmp[bucket]->next_;
                            delete_node(tmp[bucket]);
                            tmp[bucket] = next;
                        }
                    }
                    MSTL_EXEC_MEMORY__
                }
            }
        }
    }
    void clear() noexcept {
        for (size_type i = 0; i < buckets_.size(); ++i) {
            node_type* cur = buckets_[i];
            while (cur != nullptr) {
                node_type* next = cur->next_;
                delete_node(cur);
                cur = next; // stack like
            }
            buckets_[i] = 0;
        }
        size_ = 0;
    }

private:
    size_type next_size(size_type n) const noexcept {
        return hashtable_next_prime(n);
    }
    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets_.reserve(n_buckets);
        buckets_.insert(buckets_.end(), n_buckets, nullptr);
        size_ = 0;
    }
    template <typename T = key_type>
    size_type bkt_num_key(T&& key) const noexcept {
        return bkt_num_key(std::forward<T>(key), buckets_.size());
    }
    template <typename T = value_type>
    size_type bkt_num(T&& obj) const noexcept {
        return bkt_num_key(get_key_(std::forward<T>(obj)));
    }
    template <typename T = key_type>
    size_type bkt_num_key(T&& key, size_t n) const noexcept {
        return hasher_(std::forward<T>(key)) % n;
    }
    template <typename T = value_type>
    size_type bkt_num(T&& obj, size_t n) const noexcept {
        return bkt_num_key(get_key_(std::forward<T>(obj)), n);
    }

    template <typename... Args>
    node_type* new_node(Args&&... args) {
        node_type* n = alloc_.allocate();
        n->next_ = 0;
        MSTL_TRY__{
            MSTL::construct(&n->data_, std::forward<decltype(args)>(args)...);
            return n;
        }
        MSTL_CATCH_UNWIND__{
            alloc_.deallocate(n);
            MSTL_EXEC_MEMORY__;
            return nullptr; // never run
        }
    }
    void delete_node(node_type* n) noexcept {
        MSTL::destroy(&n->data_);
        alloc_.deallocate(n);
    }
    void erase_bucket(const size_type n, node_type* first, node_type* last) noexcept {
        node_type* cur = buckets_[n];
        if (cur == first) erase_bucket(n, last);
        else {
            node_type* next;
            for (next = cur->next_; next != first; cur = next, next = cur->next_);
            while (next != nullptr) {
                cur->next_ = next->next_;
                delete_node(next);
                next = cur->next_;
                --size_;
            }
        }
    }
    void erase_bucket(const size_type n, node_type* last) noexcept {
        node_type* cur = buckets_[n];
        while (cur != last) {
            node_type* next = cur->next_;
            delete_node(cur);
            cur = next;
            buckets_[n] = cur;
            --size_;
        }
    }

    void copy_from(const hashtable& ht) {
        buckets_.clear();
        buckets_.reserve(ht.buckets_.size());
        buckets_.insert(buckets_.end(), ht.buckets_.size(), (node_type*)0);
        MSTL_TRY__{
            for (size_type i = 0; i < ht.buckets_.size(); ++i) {
                if (const node_type* cur = ht.buckets_[i]) {
                    node_type* copy = new_node(cur->data_);
                    buckets_[i] = copy;
                    for (node_type* next = cur->next_; next; cur = next, next = cur->next_) {
                        copy->next_ = new_node(next->data_);
                        copy = copy->next_;
                    }
                }
            }
            size_ = ht.size_;
        }
        MSTL_CATCH_UNWIND_THROW_M__(clear());
    }
};
template <typename Value, typename Key, typename HashFcn,
    typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator ==(
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh) noexcept {
    typedef typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::node_type node;
    if (lh.buckets_.size() != rh.buckets_.size()) return false;
    for (size_t n = 0; n < lh.buckets_.size(); n++) {
        node* cur1 = lh.buckets_[n];
        node* cur2 = rh.buckets_[n];
        for (; cur1 && cur2 && cur1->data_ == cur2->data_; cur1 = cur1->next_, cur2 = cur2->next_);
        if (cur1 || cur2) return false;
    }
    return true;
}
template <typename Value, typename Key, typename HashFcn,
    typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator !=(
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
noexcept(noexcept(!(lh == rh))) {
    return !(lh == rh);
}
template <typename Value, typename Key, typename HashFcn,
    typename ExtractKey, typename EqualKey, typename Alloc>
inline void swap(hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& ht1, 
    hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& ht2)
    noexcept(noexcept(ht1.swap(ht2))) {
    ht1.swap(ht2);
}

MSTL_END_NAMESPACE__

#endif // MSTL_HASHTABLE_HPP__

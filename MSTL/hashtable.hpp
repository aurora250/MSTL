#ifndef MSTL_HASHTABLE_HPP__
#define MSTL_HASHTABLE_HPP__
#include "basiclib.h"
#include "algo.hpp"
#include "memory.hpp"
#include "vector.hpp"
#include "concepts.hpp"
#include "hash_function.hpp"
//#ifdef MSTL_DLL_LINK__
//#include <boost/container_hash/extensions.hpp>
//using namespace boost;
//#endif
MSTL_BEGIN_NAMESPACE__
using namespace concepts;

template <class T>
struct __hashtable_node {
    __hashtable_node* next_;
    T data_;
};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey,
    class Alloc = default_standard_alloc<__hashtable_node<Value>>>
class hashtable;

template <class Value, class Key, class HashFcn,
    class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef __hashtable_node<Value> node;
    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node* cur_;
    hashtable* ht_;

    __hashtable_iterator(node* n, hashtable* tab) : cur_(n), ht_(tab) {}
    __hashtable_iterator() : cur_(nullptr), ht_(nullptr) {}
    reference operator *() const { return cur_->data_; }
    pointer operator ->() const { return &(operator*()); }
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
    bool operator ==(const iterator& it) const { return cur_ == it.cur_; }
    bool operator !=(const iterator& it) const { return cur_ != it.cur_; }
};


template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
struct __hashtable_const_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;
    typedef __hashtable_node<Value> node;
    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef const Value& reference;
    typedef const Value* pointer;

    const node* cur_;
    const hashtable* ht_;

    __hashtable_const_iterator(const node* n, const hashtable* tab)
        : cur_(n), ht_(tab) {}
    __hashtable_const_iterator() {}
    __hashtable_const_iterator(const iterator& it) : cur_(it.cur_), ht_(it.ht_) {}
    reference operator *() const { return cur_->data_; }
    pointer operator ->() const { return &(operator*()); }
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
    bool operator ==(const const_iterator& it) const { return cur_ == it.cur_; }
    bool operator !=(const const_iterator& it) const { return cur_ != it.cur_; }
};

static const int NUM_PRIMERS__ = 28;
static const unsigned long PRIME_LIST__[NUM_PRIMERS__] = {
  53,         97,           193,         389,       769,
  1543,       3079,         6151,        12289,     24593,
  49157,      98317,        196613,      393241,    786433,
  1572869,    3145739,      6291469,     12582917,  25165843,
  50331653,   100663319,    201326611,   402653189, 805306457,
  1610612741, 3221225473ul, 4294967291ul
};

inline size_t __next_prime(size_t n) {
    const unsigned long* first = PRIME_LIST__;
    const unsigned long* last = PRIME_LIST__ + NUM_PRIMERS__;
    const unsigned long* pos = lower_bound(first, last, n);
    return pos == last ? (size_t)(*(last - 1)) : (size_t)(*pos);
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
    typedef typename Value::second_type value_of_key;

    template <class... _Args>
    using key_extractor = std::_In_place_key_extract_map<Key, _Args...>;

    hasher hash_funct() const { return hasher_; }
    key_equal key_eq() const { return equals_; }
private:
    hasher hasher_;
    key_equal equals_;
    ExtractKey get_key_;

    typedef __hashtable_node<Value> node;
    typedef Alloc node_allocator;

    vector<node*> buckets_;
    size_type size_;
    node_allocator alloc_;

public:
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;

    friend struct __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend struct __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend bool operator ==(const hashtable&, const hashtable&);

    explicit hashtable(size_type n, const HashFcn& hf, const EqualKey& eql, const ExtractKey& ext)
        : hasher_(hf), equals_(eql), get_key_(ext), size_(0) {
        initialize_buckets(n);
    }
    explicit hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hasher_(hf), equals_(eql), get_key_(ExtractKey()), size_(0) {
        initialize_buckets(n);
    }
    explicit hashtable(const hashtable& ht)
        : hasher_(ht.hasher_), equals_(ht.equals_), get_key_(ht.get_key_), size_(0) {
        copy_from(ht);
    }
    hashtable& operator =(const hashtable& ht) {
        if (&ht != this) {
            clear();
            hasher_ = ht.hasher_;
            equals_ = ht.equals_;
            get_key_ = ht.get_key_;
            copy_from(ht);
        }
        return *this;
    }
    ~hashtable() { clear(); }

    size_type size() const { return size_; }
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return size() == 0; }

    iterator begin() {
        for (size_type n = 0; n < buckets_.size(); ++n)
            if (buckets_[n])
                return iterator(buckets_[n], this);
        return end();
    }
    iterator end() { return iterator(nullptr, this); }

    const_iterator const_begin() const {
        for (size_type n = 0; n < buckets_.size(); ++n) {
            if (buckets_[n]) return const_iterator(buckets_[n], this);
        }
        return const_end();
    }
    const_iterator const_end() const { return const_iterator(nullptr, this); }

    void swap(hashtable& ht) {
        std::swap(hasher_, ht.hasher_);
        std::swap(equals_, ht.equals_);
        std::swap(get_key_, ht.get_key_);
        buckets_.swap(ht.buckets_);
        std::swap(size_, ht.size_);
    }
    size_type bucket_count() const { return buckets_.size(); }
    size_type max_bucket_count() const { return PRIME_LIST__[NUM_PRIMERS__ - 1]; }
    size_type elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for (node* cur = buckets_[bucket]; cur; cur = cur->next_) 
            result++;
        return result;
    }

    pair<iterator, bool> insert_unique(const value_type& obj) {
        resize(size_ + 1);
        return insert_unique_noresize(obj);
    }
    pair<iterator, bool> insert_unique(const Key& key, value_of_key&& val) {
        resize(size_ + 1);
        return insert_unique_noresize(key, std::forward<value_of_key>(val));
    }
    iterator insert_equal(const value_type& obj) {
        resize(size_ + 1);
        return insert_equal_noresize(obj);
    }
    iterator insert_equal(const Key& key, value_of_key&& val) {
        resize(size_ + 1);
        return insert_equal_noresize(key, std::forward<value_of_key>(val));
    }
    pair<iterator, bool> insert_unique_noresize(const value_type& obj) {
        const size_type n = bkt_num(obj);
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), get_key_(obj))) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                node* tmp = new_node(obj);
                tmp->next_ = buckets_[n];
                buckets_[n] = tmp;
                return pair<iterator, bool>(iterator(tmp, this), false);
            }
        }
        node* tmp = new_node(obj);
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }
    pair<iterator, bool> insert_unique_noresize(const Key& key, value_of_key&& val) {
        const size_type n = bkt_num_key(key, buckets_.size());
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                node* tmp = new_node(key, std::forward<value_of_key>(val));
                tmp->next_ = buckets_[n];
                buckets_[n] = tmp;
                return pair<iterator, bool>(iterator(cur, this), false);
            }
        }
        node* tmp = new_node(key, std::forward<value_of_key>(val));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }
    iterator insert_equal_noresize(const value_type& obj) {
        const size_type n = bkt_num(obj);
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), get_key_(obj))) {
                node* tmp = new_node(obj);
                tmp->next_ = cur->next_;
                cur->next_ = tmp;
                ++size_;
                return iterator(tmp, this);
            }
        }
        node* tmp = new_node(obj);
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return iterator(tmp, this);
    }
    iterator insert_equal_noresize(const Key& key, value_of_key&& val) {
        const size_type n = bkt_num_key(key, buckets_.size());
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) {
                node* tmp = new_node(key, std::forward<value_of_key>(val));
                tmp->next_ = cur->next_;
                cur->next_ = tmp;
                ++size_;
                return iterator(tmp, this);
            }
        }
        node* tmp = new_node(key, std::forward<value_of_key>(val));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return iterator(tmp, this);
    }
    template <class InputIterator>
    void insert_unique(InputIterator f, InputIterator l) {
        insert_unique(f, l, (iterator_category)(f));
    }
    template <class InputIterator>
    void insert_equal(InputIterator f, InputIterator l) {
        insert_equal(f, l, (iterator_category)(f));
    }
    template <class InputIterator>
    void insert_unique(InputIterator f, InputIterator l, MSTL_ITERATOR_TRATIS_FROM__ input_iterator_tag) {
        for (; f != l; ++f) insert_unique(*f);
    }
    template <class InputIterator>
    void insert_equal(InputIterator f, InputIterator l, MSTL_ITERATOR_TRATIS_FROM__ input_iterator_tag) {
        for (; f != l; ++f) insert_equal(*f);
    }
    template <class ForwardIterator>
    void insert_unique(ForwardIterator f, ForwardIterator l, MSTL_ITERATOR_TRATIS_FROM__ forward_iterator_tag) {
        size_type n = 0;
        MSTL::distance(f, l, n);
        resize(size_ + n);
        for (; n > 0; --n, ++f) insert_unique_noresize(*f);
    }
    template <class ForwardIterator>
    void insert_equal(ForwardIterator f, ForwardIterator l, MSTL_ITERATOR_TRATIS_FROM__ forward_iterator_tag) {
        size_type n = 0;
        MSTL::distance(f, l, n);
        resize(size_ + n);
        for (; n > 0; --n, ++f) insert_equal_noresize(*f);
    }

    reference find_or_insert(const value_type& obj) {
        resize(size_ + 1);
        size_type n = bkt_num(obj);
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), get_key_(obj))) return cur->data_;
        }
        node* tmp = new_node(obj);
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return tmp->data_;
    }
    reference find_or_insert(const Key& key, value_of_key&& val) {
        resize(size_ + 1);
        size_type n = bkt_num_key(key, buckets_.size());
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) return cur->data_;
        }
        node* tmp = new_node(key, std::forward<value_of_key>(val));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return tmp->data_;
    }
    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first;
        for (first = buckets_[n]; first && !equals_(get_key_(first->data_), key); first = first->next_);
        return iterator(first, this);
    }

    const_iterator find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        const node* first;
        for (first = buckets_[n]; first && !equals_(get_key_(first->data_), key); first = first->next_);
        return const_iterator(first, this);
    }

    template <typename... Args> requires (sizeof...(Args) > 1)
        pair<iterator, bool> emplace_unique(Args&&... args) {
        resize(size_ + 1);
        // using extractor = typename key_extractor<std::_Remove_cvref_t<Args>...>;
        // const auto& key = extractor::_Extract(Args...);

        auto [key, value_args] = std::forward_as_tuple(std::forward<Args>(args)...);
        const size_type n = bkt_num_key(key, buckets_.size());
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                node* tmp = new_node(key, std::forward<value_of_key>(value_args));
                tmp->next_ = buckets_[n];
                buckets_[n] = tmp;
                return pair<iterator, bool>(iterator(tmp, this), false);
            }
        }
        node* tmp = new_node(key, std::forward<decltype(value_args)>(value_args));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }

    template <typename... Args> requires (sizeof...(Args) > 1)
        iterator emplace_equal(Args&&... args) {
        resize(size_ + 1);
        auto [key, value_args] = std::forward_as_tuple(std::forward<Args>(args)...);
        const size_type n = bkt_num_key(key, buckets_.size());
        node* first = buckets_[n];
        for (node* cur = first; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) {
                node* tmp = new_node(key, std::forward<value_of_key>(value_args));
                tmp->next_ = cur->next_;
                cur->next_ = tmp;
                ++size_;
                return iterator(tmp, this);
            }
        }
        node* tmp = new_node(key, std::forward<decltype(value_args)>(value_args));
        tmp->next_ = first;
        buckets_[n] = tmp;
        ++size_;
        return iterator(tmp, this);
    }

    size_type count(const key_type& key) const {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for (const node* cur = buckets_[n]; cur; cur = cur->next_) {
            if (equals_(get_key_(cur->data_), key)) ++result;
        }
        return result;
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        typedef pair<iterator, iterator> iter_pair;
        const size_type n = bkt_num_key(key);
        for (node* first = buckets_[n]; first; first = first->next_) {
            if (equals_(get_key_(first->data_), key)) {
                for (node* cur = first->next_; cur; cur = cur->next_) {
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
        for (const node* first = buckets_[n]; first; first = first->next_) {
            if (equals_(get_key_(first->data_), key)) {
                for (const node* cur = first->next_; cur; cur = cur->next_) {
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

    size_type erase(const key_type& key) {
        const size_type n = bkt_num_key(key);
        node* first = buckets_[n];
        size_type erased = 0;
        if (first) {
            node* cur = first;
            node* next = cur->next_;
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
    void erase(const iterator& it) {
        if (node* const p = it.cur_) {
            const size_type n = bkt_num(p->data_);
            node* cur = buckets_[n];
            if (cur == p) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                --size_;
            }
            else {
                node* next = cur->next_;
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
    void erase(iterator first, iterator last) {
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

    void erase(const const_iterator& it) {
        erase(iterator(const_cast<node*>(it.cur_), const_cast<hashtable*>(it.ht_)));
    }
    void erase(const_iterator first, const_iterator last) {
        erase(iterator(const_cast<node*>(first.cur_), const_cast<hashtable*>(first.ht_)),
            iterator(const_cast<node*>(last.cur_), const_cast<hashtable*>(last.ht_)));
    }

    void resize(size_type num_elements_hint) {
        const size_type old_n = buckets_.size();
        if (num_elements_hint > old_n) {
            const size_type n = next_size(num_elements_hint);
            if (n > old_n) {
                vector<node*> tmp(n, (node*)0);
                MSTL_TRY__{
                    for (size_type bucket = 0; bucket < old_n; ++bucket) {
                        node* first = buckets_[bucket];
                        while (first) {
                            size_type new_bucket = bkt_num(first->data_, n);
                            buckets_[bucket] = first->next_;
                            first->next_ = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            first = buckets_[bucket];
                        }
                    }
                    buckets_.swap(std::move(tmp));
                }
                MSTL_CATCH_UNWIND__{
                    for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                        while (tmp[bucket]) {
                            node* next = tmp[bucket]->next_;
                            delete_node(tmp[bucket]);
                            tmp[bucket] = next;
                        }
                    }
                    throw;
                }
            }
        }
    }
    void clear() {
        for (size_type i = 0; i < buckets_.size(); ++i) {
            node* cur = buckets_[i];
            while (cur != 0) {
                node* next = cur->next_;
                delete_node(cur);
                cur = next;
            }
            buckets_[i] = 0;
        }
        size_ = 0;
    }

private:
    size_type next_size(size_type n) const { return __next_prime(n); }
    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets_.reserve(n_buckets);
        buckets_.insert(buckets_.end(), n_buckets, (node*)0);
        size_ = 0;
    }
    size_type bkt_num_key(const key_type& key) const { return bkt_num_key(key, buckets_.size()); }
    size_type bkt_num(const value_type& obj) const { return bkt_num_key(get_key_(obj)); }
    size_type bkt_num_key(const key_type& key, size_t n) const { return hasher_(key) % n; }
    size_type bkt_num(const value_type& obj, size_t n) const { return bkt_num_key(get_key_(obj), n); }

    node* new_node(const value_type& obj) {
        node* n = alloc_.allocate();
        n->next_ = 0;
        MSTL_TRY__{
          MSTL::construct(&n->data_, obj);
          return n;
        }
        MSTL_CATCH_UNWIND_THROW_U__(alloc_.deallocate(n));
    }
    template <typename... Args> requires (sizeof...(Args) > 1)
    node* new_node(Args&&... args) {
        node* n = alloc_.allocate();
        n->next_ = 0;
        MSTL_TRY__{
            MSTL::construct(&n->data_, std::forward<decltype(args)>(args)...);
            return n;
        }
        MSTL_CATCH_UNWIND_THROW_U__(alloc_.deallocate(n));
    }
    void delete_node(node* n) {
        MSTL::destroy(&n->data_);
        alloc_.deallocate(n);
    }
    void erase_bucket(const size_type n, node* first, node* last) {
        node* cur = buckets_[n];
        if (cur == first) erase_bucket(n, last);
        else {
            node* next;
            for (next = cur->next_; next != first; cur = next, next = cur->next_);
            while (next) {
                cur->next_ = next->next_;
                delete_node(next);
                next = cur->next_;
                --size_;
            }
        }
    }
    void erase_bucket(const size_type n, node* last) {
        node* cur = buckets_[n];
        while (cur != last) {
            node* next = cur->next_;
            delete_node(cur);
            cur = next;
            buckets_[n] = cur;
            --size_;
        }
    }

    void copy_from(const hashtable& ht) {
        buckets_.clear();
        buckets_.reserve(ht.buckets_.size());
        buckets_.insert(buckets_.end(), ht.buckets_.size(), (node*)0);
        MSTL_TRY__{
            for (size_type i = 0; i < ht.buckets_.size(); ++i) {
                if (const node* cur = ht.buckets_[i]) {
                    node* copy = new_node(cur->data_);
                    buckets_[i] = copy;
                    for (node* next = cur->next_; next; cur = next, next = cur->next_) {
                        copy->next_ = new_node(next->data_);
                        copy = copy->next_;
                    }
                }
            }
            size_ = ht.size_;
        }
        MSTL_CATCH_UNWIND_THROW_U__(clear());
    }
};

template <class Val, class Key, class HF, class Extract, class EqKey, class A>
bool operator ==(const hashtable<Val, Key, HF, Extract, EqKey, A>& ht1, 
    const hashtable<Val, Key, HF, Extract, EqKey, A>& ht2) {
    typedef typename hashtable<Val, Key, HF, Extract, EqKey, A>::node node;
    if (ht1.buckets_.size() != ht2.buckets_.size()) return false;
    for (int n = 0; n < ht1.buckets_.size(); ++n) {
        node* cur1 = ht1.buckets_[n];
        node* cur2 = ht2.buckets_[n];
        for (; cur1 && cur2 && cur1->val == cur2->val; cur1 = cur1->next, cur2 = cur2->next);
        if (cur1 || cur2) return false;
    }
    return true;
}
template <class Val, class Key, class HF, class Extract, class EqKey, class A>
inline void swap(hashtable<Val, Key, HF, Extract, EqKey, A>& ht1, 
    hashtable<Val, Key, HF, Extract, EqKey, A>& ht2) {
    ht1.swap(ht2);
}

MSTL_END_NAMESPACE__

#endif // MSTL_HASHTABLE_HPP__

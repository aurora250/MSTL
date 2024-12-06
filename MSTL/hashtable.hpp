#ifndef MSTL_HASHTABLE_HPP__
#define MSTL_HASHTABLE_HPP__
#include "basiclib.h"
#include "algo.hpp"
#include "memory.hpp"
#include "vector.hpp"

MSTL_BEGIN_NAMESPACE__

template <class Key>
struct hash {};
inline size_t __hash_string(const char* s) {
    unsigned long h = 0;
    for (; *s; ++s)
        h = 5 * h + *s;
    return size_t(h);
}

MSTL_TEMPLATE_NULL__ struct hash<char*> {
    size_t operator ()(const char* s) const { return __hash_string(s); }
};
MSTL_TEMPLATE_NULL__ struct hash<const char*> {
    size_t operator ()(const char* s) const { return __hash_string(s); }
};
#define HASH_STRUCT__(OPT) \
    MSTL_TEMPLATE_NULL__ struct hash<OPT> { \
    size_t operator ()(OPT x) const { return x; } \
    };
HASH_STRUCT__(char)
HASH_STRUCT__(unsigned char)
HASH_STRUCT__(signed char)
HASH_STRUCT__(short)
HASH_STRUCT__(unsigned short)
HASH_STRUCT__(int)
HASH_STRUCT__(unsigned int)
HASH_STRUCT__(long)
HASH_STRUCT__(unsigned long)
HASH_STRUCT__(long long)
HASH_STRUCT__(size_t)

template <class Value>
struct __hashtable_node {
    __hashtable_node* next;
    Value val;
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

    node* cur;
    hashtable* ht;

    __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
    __hashtable_iterator() {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++() {
        const node* old = cur;
        cur = cur->next;
        if (!cur) {
            size_type bucket = ht->bkt_num(old->val);
            while (!cur && ++bucket < ht->buckets.size())
                cur = ht->buckets[bucket];
        }
        return *this;
    }
    iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    bool operator==(const iterator& it) const { return cur == it.cur; }
    bool operator!=(const iterator& it) const { return cur != it.cur; }
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

    const node* cur;
    const hashtable* ht;

    __hashtable_const_iterator(const node* n, const hashtable* tab)
        : cur(n), ht(tab) {}
    __hashtable_const_iterator() {}
    __hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) {}
    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }
    const_iterator& operator++() {
        const node* old = cur;
        cur = cur->next;
        if (!cur) {
            size_type bucket = ht->bkt_num(old->val);
            while (!cur && ++bucket < ht->buckets.size())
                cur = ht->buckets[bucket];
        }
        return *this;
    }
    const_iterator operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
    bool operator==(const const_iterator& it) const { return cur == it.cur; }
    bool operator!=(const const_iterator& it) const { return cur != it.cur; }
};

static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
  53,         97,           193,         389,       769,
  1543,       3079,         6151,        12289,     24593,
  49157,      98317,        196613,      393241,    786433,
  1572869,    3145739,      6291469,     12582917,  25165843,
  50331653,   100663319,    201326611,   402653189, 805306457,
  1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long __next_prime(unsigned long n) {
    const unsigned long* first = __stl_prime_list;
    const unsigned long* last = __stl_prime_list + __stl_num_primes;
    const unsigned long* pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
class hashtable {
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    hasher hash_funct() const { return hash; }
    key_equal key_eq() const { return equals; }

private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;

    typedef __hashtable_node<Value> node;
    typedef Alloc node_allocator;

    vector<node*> buckets;
    size_type num_elements;
    node_allocator alloc;

public:
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> iterator;
    typedef __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc> const_iterator;

    friend struct __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend struct __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql, const ExtractKey& ext)
        : hash(hf), equals(eql), get_key(ext), num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
        initialize_buckets(n);
    }

    hashtable(const hashtable& ht)
        : hash(ht.hash), equals(ht.equals), get_key(ht.get_key), num_elements(0) {
        copy_from(ht);
    }

    hashtable& operator =(const hashtable& ht) {
        if (&ht != this) {
            clear();
            hash = ht.hash;
            equals = ht.equals;
            get_key = ht.get_key;
            copy_from(ht);
        }
        return *this;
    }
    ~hashtable() { clear(); }

    size_type size() const { return num_elements; }
    size_type max_size() const { return size_type(-1); }
    bool empty() const { return size() == 0; }

    iterator begin() {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return iterator(buckets[n], this);
        return end();
    }

    iterator end() { return iterator(0, this); }

    const_iterator begin() const {
        for (size_type n = 0; n < buckets.size(); ++n)
            if (buckets[n])
                return const_iterator(buckets[n], this);
        return end();
    }
    const_iterator end() const { return const_iterator(0, this); }

    friend bool operator ==(const hashtable&, const hashtable&);

    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const {
        return __stl_prime_list[__stl_num_primes - 1];
    }

    size_type elems_in_bucket(size_type bucket) const {
        size_type result = 0;
        for (node* cur = buckets[bucket]; cur; cur = cur->next)
            result += 1;
        return result;
    }

    pair<iterator, bool> insert_unique(const value_type& obj) {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }
    iterator insert_equal(const value_type& obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }

    pair<iterator, bool> insert_unique_noresize(const value_type& obj) {
        const size_type n = bkt_num(obj);
        node* first = buckets[n];
        for (node* cur = first; cur; cur = cur->next)
            if (equals(get_key(cur->val), get_key(obj)))
                return pair<iterator, bool>(iterator(cur, this), false);
        node* tmp = new_node(obj);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return pair<iterator, bool>(iterator(tmp, this), true);
    }
    iterator insert_equal_noresize(const value_type& obj) {
        const size_type n = bkt_num(obj);
        node* first = buckets[n];
        for (node* cur = first; cur; cur = cur->next)
            if (equals(get_key(cur->val), get_key(obj))) {
                node* tmp = new_node(obj);
                tmp->next = cur->next;
                cur->next = tmp;
                ++num_elements;
                return iterator(tmp, this);
            }
        node* tmp = new_node(obj);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return iterator(tmp, this);
    }

    template <class InputIterator>
    void insert_unique(InputIterator f, InputIterator l) {
        insert_unique(f, l, iterator_category(f));
    }
    template <class InputIterator>
    void insert_equal(InputIterator f, InputIterator l) {
        insert_equal(f, l, iterator_category(f));
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
        distance(f, l, n);
        resize(num_elements + n);
        for (; n > 0; --n, ++f) insert_unique_noresize(*f);
    }
    template <class ForwardIterator>
    void insert_equal(ForwardIterator f, ForwardIterator l, MSTL_ITERATOR_TRATIS_FROM__ forward_iterator_tag) {
        size_type n = 0;
        distance(f, l, n);
        resize(num_elements + n);
        for (; n > 0; --n, ++f) insert_equal_noresize(*f);
    }

    reference find_or_insert(const value_type& obj) {
        resize(num_elements + 1);
        size_type n = bkt_num(obj);
        node* first = buckets[n];
        for (node* cur = first; cur; cur = cur->next)
            if (equals(get_key(cur->val), get_key(obj))) return cur->val;
        node* tmp = new_node(obj);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return tmp->val;
    }
    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first;
        for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
        return iterator(first, this);
    }

    const_iterator find(const key_type& key) const
    {
        size_type n = bkt_num_key(key);
        const node* first;
        for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next);
        return const_iterator(first, this);
    }

    size_type count(const key_type& key) const
    {
        const size_type n = bkt_num_key(key);
        size_type result = 0;

        for (const node* cur = buckets[n]; cur; cur = cur->next)
            if (equals(get_key(cur->val), key))
                ++result;
        return result;
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        typedef pair<iterator, iterator> iter_pir;
        const size_type n = bkt_num_key(key);
        for (node* first = buckets[n]; first; first = first->next) {
            if (equals(get_key(first->val), key)) {
                for (node* cur = first->next; cur; cur = cur->next)
                    if (!equals(get_key(cur->val), key))
                        return iter_pir(iterator(first, this), iterator(cur, this));
                for (size_type m = n + 1; m < buckets.size(); ++m)
                    if (buckets[m])
                        return iter_pir(iterator(first, this),
                            iterator(buckets[m], this));
                return iter_pir(iterator(first, this), end());
            }
        }
        return iter_pir(end(), end());
    }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        typedef pair<const_iterator, const_iterator> citer_pir;
        const size_type n = bkt_num_key(key);
        for (const node* first = buckets[n]; first; first = first->next) {
            if (equals(get_key(first->val), key)) {
                for (const node* cur = first->next; cur; cur = cur->next)
                    if (!equals(get_key(cur->val), key))
                        return citer_pir(const_iterator(first, this), const_iterator(cur, this));
                for (size_type m = n + 1; m < buckets.size(); ++m)
                    if (buckets[m])
                        return citer_pir(const_iterator(first, this), const_iterator(buckets[m], this));
                return citer_pir(const_iterator(first, this), end());
            }
        }
        return citer_pir(end(), end());
    }

    size_type erase(const key_type& key) {
        const size_type n = bkt_num_key(key);
        node* first = buckets[n];
        size_type erased = 0;
        if (first) {
            node* cur = first;
            node* next = cur->next;
            while (next) {
                if (equals(get_key(next->val), key)) {
                    cur->next = next->next;
                    delete_node(next);
                    next = cur->next;
                    ++erased;
                    --num_elements;
                }
                else {
                    cur = next;
                    next = cur->next;
                }
            }
            if (equals(get_key(first->val), key)) {
                buckets[n] = first->next;
                delete_node(first);
                ++erased;
                --num_elements;
            }
        }
        return erased;
    }
    void erase(const iterator& it) {
        if (node* const p = it.cur) {
            const size_type n = bkt_num(p->val);
            node* cur = buckets[n];
            if (cur == p) {
                buckets[n] = cur->next;
                delete_node(cur);
                --num_elements;
            }
            else {
                node* next = cur->next;
                while (next) {
                    if (next == p) {
                        cur->next = next->next;
                        delete_node(next);
                        --num_elements;
                        break;
                    }
                    else {
                        cur = next;
                        next = cur->next;
                    }
                }
            }
        }
    }
    void erase(iterator first, iterator last) {
        size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();
        size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();
        if (first.cur == last.cur) return;
        else if (f_bucket == l_bucket)
            erase_bucket(f_bucket, first.cur, last.cur);
        else {
            erase_bucket(f_bucket, first.cur, 0);
            for (size_type n = f_bucket + 1; n < l_bucket; ++n)
                erase_bucket(n, 0);
            if (l_bucket != buckets.size())
                erase_bucket(l_bucket, last.cur);
        }
    }

    void erase(const const_iterator& it) {
        erase(iterator(const_cast<node*>(it.cur), const_cast<hashtable*>(it.ht)));
    }
    void erase(const_iterator first, const_iterator last) {
        erase(iterator(const_cast<node*>(first.cur), const_cast<hashtable*>(first.ht)),
            iterator(const_cast<node*>(last.cur), const_cast<hashtable*>(last.ht)));
    }

    void resize(size_type num_elements_hint) {
        const size_type old_n = buckets.size();
        if (num_elements_hint > old_n) {
            const size_type n = next_size(num_elements_hint);
            if (n > old_n) {
                vector<node*> tmp(n, (node*)0);
                MSTL_TRY__{
                    for (size_type bucket = 0; bucket < old_n; ++bucket) {
                        node* first = buckets[bucket];
                        while (first) {
                            size_type new_bucket = bkt_num(first->val, n);
                            buckets[bucket] = first->next;
                            first->next = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            first = buckets[bucket];
                        }
                    }
                    buckets.swap(tmp);
                }
                MSTL_CATCH_UNWIND__{
                    for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                        while (tmp[bucket]) {
                            node* next = tmp[bucket]->next;
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
        for (size_type i = 0; i < buckets.size(); ++i) {
            node* cur = buckets[i];
            while (cur != 0) {
                node* next = cur->next;
                delete_node(cur);
                cur = next;
            }
            buckets[i] = 0;
        }
        num_elements = 0;
    }

private:
    size_type next_size(size_type n) const { return __next_prime(n); }

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*)0);
        num_elements = 0;
    }
    size_type bkt_num_key(const key_type& key) const {
        return bkt_num_key(key, buckets.size());
    }
    size_type bkt_num(const value_type& obj) const {
        return bkt_num_key(get_key(obj));
    }
    size_type bkt_num_key(const key_type& key, size_t n) const {
        return hash(key) % n;
    }
    size_type bkt_num(const value_type& obj, size_t n) const {
        return bkt_num_key(get_key(obj), n);
    }

    node* new_node(const value_type& obj) {
        node* n = alloc.allocate();
        n->next = 0;
        MSTL_TRY__{
          (construct)(&n->val, obj);
          return n;
        }
        MSTL_CATCH_UNWIND_THROW_U__(alloc.deallocate(n));
    }

    void delete_node(node* n) {
        destroy(&n->val);
        alloc.deallocate(n);
    }

    void erase_bucket(const size_type n, node* first, node* last) {
        node* cur = buckets[n];
        if (cur == first) erase_bucket(n, last);
        else {
            node* next;
            for (next = cur->next; next != first; cur = next, next = cur->next);
            while (next) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                --num_elements;
            }
        }
    }
    void erase_bucket(const size_type n, node* last) {
        node* cur = buckets[n];
        while (cur != last) {
            node* next = cur->next;
            delete_node(cur);
            cur = next;
            buckets[n] = cur;
            --num_elements;
        }
    }
    void copy_from(const hashtable& ht) {
        buckets.clear();
        buckets.reserve(ht.buckets.size());
        buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
        MSTL_TRY__{
          for (size_type i = 0; i < ht.buckets.size(); ++i) {
            if (const node* cur = ht.buckets[i]) {
              node* copy = new_node(cur->val);
              buckets[i] = copy;

              for (node* next = cur->next; next; cur = next, next = cur->next) {
                copy->next = new_node(next->val);
                copy = copy->next;
              }
            }
          }
          num_elements = ht.num_elements;
        }
        MSTL_CATCH_UNWIND_THROW_U__(clear());
    }
};

template <class V, class K, class HF, class Ex, class Eq, class A>
bool operator ==(const hashtable<V, K, HF, Ex, Eq, A>& ht1,
    const hashtable<V, K, HF, Ex, Eq, A>& ht2) {
    typedef typename hashtable<V, K, HF, Ex, Eq, A>::node node;
    if (ht1.buckets.size() != ht2.buckets.size()) return false;
    for (int n = 0; n < ht1.buckets.size(); ++n) {
        node* cur1 = ht1.buckets[n];
        node* cur2 = ht2.buckets[n];
        for (; cur1 && cur2 && cur1->val == cur2->val; cur1 = cur1->next, cur2 = cur2->next);
        if (cur1 || cur2) return false;
    }
    return true;
}

MSTL_END_NAMESPACE__

#endif // MSTL_HASHTABLE_HPP__

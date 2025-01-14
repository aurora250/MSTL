#ifndef MSTL_HASH_SET_HPP__
#define MSTL_HASH_SET_HPP__
#include "basiclib.h"
#include "hashtable.hpp"
#include "functor.hpp"
MSTL_BEGIN_NAMESPACE__

template <class Value, class HashFcn = hash<Value>,  class EqualKey = equal_to<Value>, 
    class Alloc = default_standard_alloc<__hashtable_node<pair<const Value, Value>>>>
    requires(HashFunction<HashFcn, Value>&& BinaryFunction<EqualKey>)
class hash_set {
private:
    typedef hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef hash_set<Value, HashFcn, EqualKey, Alloc> self;
    typedef typename ht::key_type key_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;
    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::const_pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::const_reference reference;
    typedef typename ht::const_reference const_reference;
    typedef typename ht::const_iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

    hash_set() : rep(100, hasher(), key_equal()) {}
    explicit hash_set(size_type n) : rep(n, hasher(), key_equal()) {}
    explicit hash_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    explicit hash_set(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class InputIterator>
    hash_set(InputIterator f, InputIterator l, size_type n, const hasher& hf, 
        const key_equal& eql) : rep(n, hf, eql) {
        rep.insert_unique(f, l);
    }

    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(hash_set& hs) { rep.swap(hs.rep); }
    iterator begin() const { return rep.begin(); }
    iterator end() const { return rep.end(); }
    friend bool operator ==(const hash_set&, const hash_set&);

    pair<iterator, bool> insert(const value_type& obj) {
        pair<typename ht::iterator, bool> p = rep.insert_unique(obj);
        return pair<iterator, bool>(p.first, p.second);
    }
    template <class InputIterator>
    void insert(InputIterator f, InputIterator l) { rep.insert_unique(f, l); }
    pair<iterator, bool> insert_noresize(const value_type& obj) {
        pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
        return pair<iterator, bool>(p.first, p.second);
    }

    iterator find(const key_type& key) const { return rep.find(key); }
    size_type count(const key_type& key) const { return rep.count(key); }
    pair<iterator, iterator> equal_range(const key_type& key) const { return rep.equal_range(key); }
    size_type erase(const key_type& key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear(); }
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }
};

template <class Value, class HashFcn, class EqualKey, class Alloc>
inline bool operator ==(const hash_set<Value, HashFcn, EqualKey, Alloc>& hs1,
    const hash_set<Value, HashFcn, EqualKey, Alloc>& hs2) {
    return hs1.rep == hs2.rep;
}
template <class Val, class HashFcn, class EqualKey, class Alloc>
inline void swap(hash_set<Val, HashFcn, EqualKey, Alloc>& hs1, hash_set<Val, HashFcn, EqualKey, Alloc>& hs2) {
    hs1.swap(hs2);
}


template <class Value, class HashFcn = hash<Value>, class EqualKey = equal_to<Value>,
    class Alloc = default_standard_alloc<pair<const Value, Value>>>
    requires(HashFunction<HashFcn, Value>&& BinaryFunction<EqualKey>)
class hash_multiset {
private:
    typedef hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef typename ht::key_type key_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;
    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::const_pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::const_reference reference;
    typedef typename ht::const_reference const_reference;
    typedef typename ht::const_iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

    hash_multiset() : rep(100, hasher(), key_equal()) {}
    explicit hash_multiset(size_type n) : rep(n, hasher(), key_equal()) {}
    explicit hash_multiset(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    explicit hash_multiset(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    hash_multiset(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multiset(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multiset(InputIterator f, InputIterator l, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multiset(InputIterator f, InputIterator l, size_type n, const hasher& hf,
        const key_equal& eql) : rep(n, hf, eql) {
        rep.insert_equal(f, l);
    }

    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(hash_multiset& hs) { rep.swap(hs.rep); }
    iterator begin() const { return rep.begin(); }
    iterator end() const { return rep.end(); }
    friend bool operator ==(const hash_multiset&, const hash_multiset&);

    iterator insert(const value_type& obj) { return rep.insert_equal(obj); }
    template <class InputIterator>
    void insert(InputIterator f, InputIterator l) { rep.insert_equal(f, l); }
    iterator insert_noresize(const value_type& obj) { return rep.insert_equal_noresize(obj); }
    iterator find(const key_type& key) const { return rep.find(key); }
    size_type count(const key_type& key) const { return rep.count(key); }
    pair<iterator, iterator> equal_range(const key_type& key) const { return rep.equal_range(key); }
    size_type erase(const key_type& key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear(); }
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }
};
template <class Val, class HashFcn, class EqualKey, class Alloc>
inline bool operator ==(const hash_multiset<Val, HashFcn, EqualKey, Alloc>& hs1,
    const hash_multiset<Val, HashFcn, EqualKey, Alloc>& hs2) {
    return hs1.rep == hs2.rep;
}
template <class Val, class HashFcn, class EqualKey, class Alloc>
inline void swap(hash_multiset<Val, HashFcn, EqualKey, Alloc>& hs1,
    hash_multiset<Val, HashFcn, EqualKey, Alloc>& hs2) {
    hs1.swap(hs2);
}

MSTL_END_NAMESPACE__
#endif // MSTL_HASH_SET_HPP__

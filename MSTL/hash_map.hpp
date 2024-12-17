#ifndef MSTL_HASH_MAP_HPP__
#define MSTL_HASH_MAP_HPP__
#include "hashtable.hpp"
#include "functor.hpp"
#include "container.h"
#include "check_type.h"

MSTL_BEGIN_NAMESPACE__

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = default_standard_alloc<__hashtable_node<pair<const Key, T>>>>
class hash_map {
private:
    typedef hashtable<pair<const Key, T>, Key, HashFcn, select1st<pair<const Key, T> >, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef hash_map<Key, T, HashFcn, EqualKey, Alloc> self;
    typedef typename ht::key_type key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;
    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::reference reference;
    typedef typename ht::const_reference const_reference;
    typedef typename ht::iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    static const char* const __type__;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

    hash_map() : rep(100, hasher(), key_equal()) {}
    explicit hash_map(size_type n) : rep(n, hasher(), key_equal()) {}
    hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    hash_map(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    hash_map(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class InputIterator>
    hash_map(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class InputIterator>
    hash_map(InputIterator f, InputIterator l, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class InputIterator>
    hash_map(InputIterator f, InputIterator l, size_type n, const hasher& hf,
        const key_equal& eql) : rep(n, hf, eql) {
        rep.insert_unique(f, l);
    }

    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(hash_map& hs) { rep.swap(hs.rep); }
    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }
    const_iterator const_begin() const { return rep.const_begin(); }
    const_iterator const_end() const { return rep.const_end(); }
    friend bool operator ==(const hash_map&, const hash_map&);

    pair<iterator, bool> insert(const value_type& obj) { return rep.insert_unique(obj); }
    pair<iterator, bool> insert(const Key& k, const T& v) { return rep.insert_unique(value_type(k, v)); }
    pair<iterator, bool> insert(Key&& k, T&& v) { 
        return rep.insert_unique(value_type(std::move(k), std::move(v))); 
    }
    template<typename... Args>
    void emplace(Args&&... args) {
        rep.emplace_unique(std::forward<Args>(args)...);
    }

    template <class InputIterator>
    void insert(InputIterator f, InputIterator l) { rep.insert_unique(f, l); }
    pair<iterator, bool> insert_noresize(const value_type& obj) { return rep.insert_unique_noresize(obj); }

    iterator find(const key_type& key) { return rep.find(key); }
    const_iterator find(const key_type& key) const { return rep.find(key); }
    T& operator[](const key_type& key) {
        return rep.find_or_insert(value_type(key, T())).second;
    }
    T& operator[](key_type&& key) {
        return rep.find_or_insert(value_type(std::forward<key_type>(key), T())).second;
    }

    size_type count(const key_type& key) const { return rep.count(key); }
    pair<iterator, iterator> equal_range(const key_type& key) { return rep.equal_range(key); }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return rep.equal_range(key);
    }
    size_type erase(const key_type& key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear(); }

    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }
};
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
inline bool operator ==(const hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm1,
    const hash_map<Key, T, HashFcn, EqualKey, Alloc>& hm2) {
    return hm1.rep == hm2.rep;
}
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
const char* const hash_map<Key, T, HashFcn, EqualKey, Alloc>::__type__ = "hash_map";

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = default_standard_alloc<__hashtable_node<pair<const Key, T>>>>
using unordered_map = hash_map<Key, T, HashFcn, EqualKey, Alloc>;


template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = default_standard_alloc<__hashtable_node<pair<const Key, T>>>>
class hash_multimap {
private:
    typedef hashtable<pair<const Key, T>, Key, HashFcn, select1st<pair<const Key, T>>, EqualKey, Alloc> ht;
    ht rep;
public:
    typedef hash_multimap<Key, T, HashFcn, EqualKey, Alloc> self;
    typedef typename ht::key_type key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;
    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::reference reference;
    typedef typename ht::const_reference const_reference;
    typedef typename ht::iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    hasher hash_funct() const { return rep.hash_funct(); }
    key_equal key_eq() const { return rep.key_eq(); }

    static const char* const __type__;

    hash_multimap() : rep(100, hasher(), key_equal()) {}
    explicit hash_multimap(size_type n) : rep(n, hasher(), key_equal()) {}
    explicit hash_multimap(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    explicit hash_multimap(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    hash_multimap(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multimap(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multimap(InputIterator f, InputIterator l, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multimap(InputIterator f, InputIterator l, size_type n, const hasher& hf,
        const key_equal& eql) : rep(n, hf, eql) {
        rep.insert_equal(f, l);
    }

    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(hash_multimap& hs) { rep.swap(hs.rep); }
    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }
    const_iterator const_begin() const { return rep.const_begin(); }
    const_iterator const_end() const { return rep.const_end(); }
    friend bool operator ==(const hash_multimap&, const hash_multimap&);

    template<typename... Args>
    void emplace(Args&&... args) {
        //rep.emplace_equal(std::forward<Args>(args)...);
    }
    iterator insert(value_type&& obj) { return rep.insert_equal(std::move(obj)); }
    iterator insert(const value_type& obj) { return rep.insert_equal(obj); }
    iterator insert(Key&& k, T&& v) { return rep.insert_equal(value_type(std::forward<Key>(k), std::forward<T>(v))); }
    iterator insert(const Key& k, const T& v) { return rep.insert_equal(value_type(k, v)); }
    template <class InputIterator>
    void insert(InputIterator f, InputIterator l) { rep.insert_equal(f, l); }
    iterator insert_noresize(const value_type& obj) {
        return rep.insert_equal_noresize(obj);
    }
    iterator find(const key_type& key) { return rep.find(key); }
    const_iterator find(const key_type& key) const { return rep.find(key); }
    size_type count(const key_type& key) const { return rep.count(key); }
    pair<iterator, iterator> equal_range(const key_type& key) { return rep.equal_range(key); }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return rep.equal_range(key);
    }
    size_type erase(const key_type& key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void erase(iterator f, iterator l) { rep.erase(f, l); }
    void clear() { rep.clear(); }
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }
};
template <class Key, class T, class HF, class EqKey, class Alloc>
inline bool operator ==(const hash_multimap<Key, T, HF, EqKey, Alloc>& hm1, 
    const hash_multimap<Key, T, HF, EqKey, Alloc>& hm2) {
    return hm1.rep == hm2.rep;
}
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
const char* const hash_multimap<Key, T, HashFcn, EqualKey, Alloc>::__type__ = "hash_multimap";

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = default_standard_alloc<__hashtable_node<pair<const Key, T>>>>
using unordered_multimap = hash_multimap<Key, T, HashFcn, EqualKey, Alloc>;

MSTL_END_NAMESPACE__

#endif // MSTL_HASH_MAP_HPP__

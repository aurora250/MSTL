#ifndef MSTL_HASH_MAP_HPP__
#define MSTL_HASH_MAP_HPP__
#include "hashtable.hpp"
#include "functor.hpp"
#include "concepts.hpp"
MSTL_BEGIN_NAMESPACE__

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = standard_allocator<__hashtable_node<pair<const Key, T>>>>
    requires(is_hash_v<HashFcn, Key>)
class hash_map {
private:
    typedef hashtable<pair<const Key, T>, Key, HashFcn, 
        select1st<pair<const Key, T>>, EqualKey, Alloc> ht;

    ht rep;

    friend bool operator ==(const hash_map&, const hash_map&) noexcept;
public:
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
    typedef hash_map<Key, T, HashFcn, EqualKey, Alloc> self;

    hash_map() : rep(10, hasher(), key_equal()) {}

    explicit hash_map(size_type n) : rep(n) {}
    hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    hash_map(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}

    hash_map(const self& ht) : rep(ht.rep) {}
    self& operator =(const self& x) {
        rep = x.rep;
        return *this;
    }
    hash_map(self&& x) noexcept(noexcept(rep.swap(x.rep))) : rep(std::forward<ht>(x.rep)) {}
    self& operator =(self&& x) noexcept(noexcept(rep.swap(x.rep))) {
        rep = std::forward<self>(x.rep);
        return *this;
    }

    template <class Iterator>
        requires(input_iterator<Iterator>)
    hash_map(Iterator f, Iterator l) : rep(10, hasher(), key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class Iterator>
        requires(input_iterator<Iterator>)
    hash_map(Iterator f, Iterator l, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class Iterator>
        requires(input_iterator<Iterator>)
    hash_map(Iterator f, Iterator l, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_unique(f, l);
    }
    template <class Iterator>
        requires(input_iterator<Iterator>)
    hash_map(Iterator f, Iterator l, size_type n, const hasher& hf, const key_equal& eql) 
        : rep(n, hf, eql) {
        rep.insert_unique(f, l);
    }
    hash_map(std::initializer_list<value_type> l) 
        : hash_map(l.begin(), l.end()) {}
    hash_map(std::initializer_list<value_type> l, size_type n) 
        : hash_map(l.begin(), l.end(), n) {}
    hash_map(std::initializer_list<value_type> l, size_type n, const hasher& hf)
        : hash_map(l.begin(), l.end(), n, hf) {}
    hash_map(std::initializer_list<value_type> l, size_type n, const hasher& hf, const key_equal& eql)
        : hash_map(l.begin(), l.end(), n, hf, eql) {}

    MSTL_NODISCARD size_type size() const noexcept { return rep.size(); }
    MSTL_NODISCARD bool empty() const noexcept { return rep.empty(); }

    MSTL_NODISCARD iterator begin() noexcept { return rep.begin(); }
    MSTL_NODISCARD iterator end() noexcept { return rep.end(); }
    MSTL_NODISCARD const_iterator cbegin() const noexcept { return rep.cbegin(); }
    MSTL_NODISCARD const_iterator cend() const noexcept { return rep.cend(); }

    hasher hash_funct() const noexcept(noexcept(rep.hash_func())) { return rep.hash_func(); }
    key_equal key_eq() const noexcept(noexcept(rep.key_eql())) { return rep.key_eql(); }

    pair<iterator, bool> insert(const value_type& obj) {
        return rep.insert_unique(obj);
    }
    pair<iterator, bool> insert(value_type&& obj) { 
        return rep.insert_unique(MSTL::forward<value_type>(obj));
    }
    template<typename Key, typename... Args>
    void emplace(Key&& key, Args&&... args) {
        data_type data(MSTL::forward<Args>(args)...);
        value_type val(key, MSTL::move(data));
        rep.insert_unique(MSTL::forward<value_type>(val));
    }

    template <class Iterator>
        requires(input_iterator<Iterator>)
    void insert(Iterator f, Iterator l) { rep.insert_unique(f, l); }
    template <typename T = value_type>
    pair<iterator, bool> insert_noresize(T&& x) { 
        return rep.insert_unique_noresize(MSTL::forward<T>(x));
    }

    MSTL_NODISCARD iterator find(const key_type& key) { return rep.find(key); }
    MSTL_NODISCARD const_iterator find(const key_type& key) const { return rep.find(key); }
    MSTL_NODISCARD T& operator[](const key_type& key) {
        return rep.find_or_insert(std::move(value_type(key, T()))).second;
    }
    MSTL_NODISCARD T& at(const key_type& key) {
        auto iter = rep.find(key);
        Exception(iter != end(), StopIterator());
        return iter->second;
    }
    MSTL_NODISCARD const T& at(const key_type& key) const {
        auto iter = rep.find(key);
        Exception(iter != cend(), StopIterator());
        return iter->second;
    }

    pair<iterator, iterator> equal_range(const key_type& key) { return rep.equal_range(key); }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return rep.equal_range(key);
    }
    size_type erase(const key_type& key) noexcept { return rep.erase(key); }
    void erase(iterator it) noexcept { rep.erase(it); }
    void erase(iterator f, iterator l) noexcept { rep.erase(f, l); }
    void clear() noexcept { rep.clear(); }

    void resize(size_type hint) { rep.resize(hint); }

    MSTL_NODISCARD size_type count(const key_type& key) const noexcept { return rep.count(key); }
    MSTL_NODISCARD size_type bucket_size() const noexcept { return rep.buckets_size(); }
    MSTL_NODISCARD size_type max_bucket_count() const noexcept { return rep.max_buckets_size(); }
    MSTL_NODISCARD size_type bucket_count(size_type n) const noexcept { return rep.bucket_count(n); }
    void swap(hash_map& hs) noexcept(noexcept(rep.swap(hs.rep))) { rep.swap(hs.rep); }
};
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
MSTL_NODISCARD bool operator ==(
    const hash_map<Key, T, HashFcn, EqualKey, Alloc>& lh,
    const hash_map<Key, T, HashFcn, EqualKey, Alloc>& rh) noexcept {
    return lh.rep == rh.rep;
}
template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
void swap(const hash_map<Key, T, HashFcn, EqualKey, Alloc>& lh,
    const hash_map<Key, T, HashFcn, EqualKey, Alloc>& rh) noexcept(noexcept(lh.swap(rh))) {
    lh.swap(rh);
}

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = standard_allocator<__hashtable_node<pair<const Key, T>>>>
    requires(is_hash_v<HashFcn, Key>)
using unordered_map = hash_map<Key, T, HashFcn, EqualKey, Alloc>;


template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = standard_allocator<__hashtable_node<pair<const Key, T>>>>
    requires(is_hash_v<HashFcn, Key>)
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

    hasher hash_funct() const { return rep.hash_func(); }
    key_equal key_eq() const { return rep.key_eql(); }

    hash_multimap() : rep(10) {}
    explicit hash_multimap(size_type n) : rep(n) {}
    explicit hash_multimap(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    hash_multimap(InputIterator f, InputIterator l) : rep(10) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    hash_multimap(InputIterator f, InputIterator l, size_type n) : rep(n) {
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
    const_iterator cbegin() const { return rep.cbegin(); }
    const_iterator cend() const { return rep.cend(); }
    friend bool operator ==(const hash_multimap&, const hash_multimap&);


    template<typename Key, typename... Args>
    void emplace(Key&& key, Args&&... args) {
        data_type data(MSTL::forward<Args>(args)...);
        value_type val(key, MSTL::move(data));
        rep.insert_equal(MSTL::forward<value_type>(val));
    }
    iterator insert(value_type&& obj) { return rep.insert_equal(std::move(obj)); }
    iterator insert(const value_type& obj) { return rep.insert_equal(obj); }
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
    size_type bucket_count() const { return rep.buckets_size(); }
    size_type max_bucket_count() const { return rep.max_buckets_size(); }
    size_type elems_in_bucket(size_type n) const { return rep.bucket_count(n); }
};
template <class Key, class T, class HF, class EqKey, class Alloc>
inline bool operator ==(const hash_multimap<Key, T, HF, EqKey, Alloc>& hm1, 
    const hash_multimap<Key, T, HF, EqKey, Alloc>& hm2) noexcept {
    return hm1.rep == hm2.rep;
}

template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>,
    class Alloc = standard_allocator<__hashtable_node<pair<const Key, T>>>>
    requires(is_hash_v<HashFcn, Key>)
using unordered_multimap = hash_multimap<Key, T, HashFcn, EqualKey, Alloc>;

MSTL_END_NAMESPACE__

#endif // MSTL_HASH_MAP_HPP__

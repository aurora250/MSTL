#ifndef MSTL_HASH_SET_HPP__
#define MSTL_HASH_SET_HPP__
#include "hashtable.hpp"
#include "functor.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename Value, typename HashFcn = hash<Value>, typename EqualKey = equal_to<Value>,
    typename Alloc = standard_allocator<__hashtable_node<Value>>>
class unordered_set {
#ifdef MSTL_VERSION_20__
    static_assert(is_hash_v<HashFcn, Value>, "unordered set requires valid hash function.");
    static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
    static_assert(is_same_v<__hashtable_node<Value>, typename Alloc::value_type>,
        "allocator type mismatch.");
    static_assert(is_object_v<Value>, "unordered set only contains object types.");

private:
    using base_type         = hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Alloc>;
public:
    using key_type          = typename base_type::key_type;
    using value_type        = typename base_type::value_type;
    using hasher            = typename base_type::hasher;
    using key_equal         = typename base_type::key_equal;
    using size_type         = typename base_type::size_type;
    using difference_type   = typename base_type::difference_type;
    using pointer           = typename base_type::const_pointer;
    using const_pointer     = typename base_type::const_pointer;
    using reference         = typename base_type::const_reference;
    using const_reference   = typename base_type::const_reference;
    using iterator          = typename base_type::const_iterator;
    using const_iterator    = typename base_type::const_iterator;
    using allocator_type    = typename base_type::allocator_type;
    using self              = unordered_set<Value, HashFcn, EqualKey, Alloc>;

private:
    base_type ht_;

    friend bool operator ==(const unordered_set&, const unordered_set&) noexcept;

public:
    unordered_set() : ht_(100, hasher(), key_equal()) {}
    explicit unordered_set(size_type n) : ht_(n) {}

    unordered_set(size_type n, const hasher& hf) : ht_(n, hf, key_equal()) {}
    unordered_set(size_type n, const hasher& hf, const key_equal& eql) : ht_(n, hf, eql) {}

    unordered_set(const self& ht) : ht_(ht.ht_) {}
    self& operator =(const self& x) {
        ht_ = x.ht_;
        return *this;
    }

    unordered_set(self&& x) noexcept(noexcept(ht_.swap(x.ht_))) : ht_(MSTL::forward<base_type>(x.ht_)) {}
    self& operator =(self&& x) noexcept(noexcept(ht_.swap(x.ht_))) {
        ht_ = MSTL::forward<self>(x.ht_);
        return *this;
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    unordered_set(Iterator first, Iterator last) : ht_(100, hasher(), key_equal()) {
        ht_.insert_unique(first, last);
    }
    template <typename Iterator>
        requires(input_iterator<Iterator>)
    unordered_set(Iterator first, Iterator list, size_type n) : ht_(n, hasher(), key_equal()) {
        ht_.insert_unique(first, list);
    }
    template <typename Iterator>
        requires(input_iterator<Iterator>)
    unordered_set(Iterator first, Iterator last, size_type n, const hasher& hf) : ht_(n, hf, key_equal()) {
        ht_.insert_unique(first, last);
    }
    template <typename Iterator>
        requires(input_iterator<Iterator>)
    unordered_set(Iterator first, Iterator last, size_type n, const hasher& hf, const key_equal& eql)
        : ht_(n, hf, eql) {
        ht_.insert_unique(first, last);
    }

    unordered_set(std::initializer_list<value_type> l)
        : unordered_set(l.begin(), l.end()) {
    }
    unordered_set(std::initializer_list<value_type> l, size_type n)
        : unordered_set(l.begin(), l.end(), n) {
    }
    unordered_set(std::initializer_list<value_type> l, size_type n, const hasher& hf)
        : unordered_set(l.begin(), l.end(), n, hf) {
    }
    unordered_set(std::initializer_list<value_type> l, size_type n, const hasher& hf, const key_equal& eql)
        : unordered_set(l.begin(), l.end(), n, hf, eql) {
    }

    MSTL_NODISCARD iterator begin() noexcept { return ht_.begin(); }
    MSTL_NODISCARD iterator end() noexcept { return ht_.end(); }
    MSTL_NODISCARD const_iterator cbegin() const noexcept { return ht_.cbegin(); }
    MSTL_NODISCARD const_iterator cend() const noexcept { return ht_.cend(); }

    MSTL_NODISCARD size_type size() const noexcept { return ht_.size(); }
    MSTL_NODISCARD size_type max_size() const noexcept { return ht_.max_size(); }
    MSTL_NODISCARD bool empty() const noexcept { return ht_.empty(); }

    MSTL_NODISCARD size_type count(const key_type& key) const noexcept(noexcept(ht_.count(key))) {
        return ht_.count(key);
    }
    MSTL_NODISCARD size_type bucket_count() const noexcept { return ht_.bucket_count(); }
    MSTL_NODISCARD size_type max_bucket_count() const noexcept { return ht_.max_bucket_count(); }
    MSTL_NODISCARD size_type bucket_size(size_type n) const noexcept { return ht_.bucket_size(n); }

    MSTL_NODISCARD allocator_type get_allocator() const noexcept { return allocator_type(); }

    MSTL_NODISCARD hasher hash_funct() const noexcept(noexcept(ht_.hash_func())) { return ht_.hash_func(); }
    MSTL_NODISCARD key_equal key_eq() const noexcept(noexcept(ht_.key_eql())) { return ht_.key_eql(); }

    MSTL_NODISCARD float load_factor() const noexcept { return ht_.load_factor(); }
    MSTL_NODISCARD float max_load_factor() const noexcept { return ht_.max_load_factor(); }
    void max_load_factor(float new_max) noexcept { ht_.max_load_factor(new_max); }

    void rehash(size_type new_size) { ht_.rehash(new_size); }
    void reserve(size_type max_count) { ht_.reserve(max_count); }

    template <typename... Args>
    pair<iterator, bool> emplace(Args&&... args) {
        return ht_.emplace_unique(MSTL::forward<Args>(args)...);
    }

    pair<iterator, bool> insert(const value_type& obj) {
        return ht_.insert_unique(obj);
    }
    pair<iterator, bool> insert(value_type&& obj) {
        return ht_.insert_unique(MSTL::forward<value_type>(obj));
    }
    template <typename Iterator>
        requires(input_iterator<Iterator>)
    void insert(Iterator first, Iterator last) { ht_.insert_unique(first, last); }

    size_type erase(const key_type& key) noexcept { return ht_.erase(key); }
    void erase(iterator it) noexcept { ht_.erase(it); }
    void erase(iterator f, iterator l) noexcept { ht_.erase(f, l); }
    void clear() noexcept { ht_.clear(); }

    void swap(self& x) noexcept(noexcept(ht_.swap(x.ht_))) { ht_.swap(x.ht_); }

    MSTL_NODISCARD iterator find(const key_type& key) { return ht_.find(key); }
    MSTL_NODISCARD const_iterator find(const key_type& key) const { return ht_.find(key); }

    pair<iterator, iterator> equal_range(const key_type& key) { return ht_.equal_range(key); }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return ht_.equal_range(key);
    }
};
template <class Value, class HashFcn, class EqualKey, class Alloc>
inline MSTL_NODISCARD bool operator ==(
    const unordered_set<Value, HashFcn, EqualKey, Alloc>& hs1,
    const unordered_set<Value, HashFcn, EqualKey, Alloc>& hs2) noexcept {
    return hs1.ht_ == hs2.ht_;
}
template <class Value, class HashFcn, class EqualKey, class Alloc>
inline MSTL_NODISCARD void swap(
    unordered_set<Value, HashFcn, EqualKey, Alloc>& hs1,
    unordered_set<Value, HashFcn, EqualKey, Alloc>& hs2) {
    hs1.swap(hs2);
}


template <class Value, class HashFcn = hash<Value>, class EqualKey = equal_to<Value>,
    class Alloc = standard_allocator<pair<const Value, Value>>>
    requires(is_hash_v<HashFcn, Value>)
class unordered_multiset {
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

    hasher hash_funct() const { return rep.hash_func(); }
    key_equal key_eq() const { return rep.key_eql(); }

    unordered_multiset() : rep(100, hasher(), key_equal()) {}
    explicit unordered_multiset(size_type n) : rep(n, hasher(), key_equal()) {}
    explicit unordered_multiset(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    explicit unordered_multiset(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}
    template <class InputIterator>
    unordered_multiset(InputIterator f, InputIterator l) : rep(100, hasher(), key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    unordered_multiset(InputIterator f, InputIterator l, size_type n) : rep(n, hasher(), key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    unordered_multiset(InputIterator f, InputIterator l, size_type n, const hasher& hf) : rep(n, hf, key_equal()) {
        rep.insert_equal(f, l);
    }
    template <class InputIterator>
    unordered_multiset(InputIterator f, InputIterator l, size_type n, const hasher& hf,
        const key_equal& eql) : rep(n, hf, eql) {
        rep.insert_equal(f, l);
    }

    size_type size() const { return rep.size(); }
    size_type max_size() const { return rep.max_size(); }
    bool empty() const { return rep.empty(); }
    void swap(unordered_multiset& hs) { rep.swap(hs.rep); }
    iterator begin() const { return rep.begin(); }
    iterator end() const { return rep.end(); }
    friend bool operator ==(const unordered_multiset&, const unordered_multiset&);

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
    void resize(size_type hint) { rep.rehash(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.bucket_size(n); }
};
template <class Val, class HashFcn, class EqualKey, class Alloc>
inline bool operator ==(const unordered_multiset<Val, HashFcn, EqualKey, Alloc>& hs1,
    const unordered_multiset<Val, HashFcn, EqualKey, Alloc>& hs2) {
    return hs1.rep == hs2.rep;
}
template <class Val, class HashFcn, class EqualKey, class Alloc>
inline void swap(unordered_multiset<Val, HashFcn, EqualKey, Alloc>& hs1,
    unordered_multiset<Val, HashFcn, EqualKey, Alloc>& hs2) {
    hs1.swap(hs2);
}

MSTL_END_NAMESPACE__
#endif // MSTL_HASH_SET_HPP__

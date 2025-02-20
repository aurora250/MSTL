#ifndef MSTL_HASHTABLE_HPP__
#define MSTL_HASHTABLE_HPP__
#include "algo.hpp"
#include "functor.hpp"
#include "vector.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename T>
struct __hashtable_node {
    __hashtable_node* next_;
    T data_;
};

template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey,
    typename Alloc = standard_allocator<__hashtable_node<Value>>>
class hashtable;

template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
struct hashtable_const_iterator;

template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
struct hashtable_iterator {
    using hashtable         = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using iterator          = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using const_iterator    = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using node              = __hashtable_node<Value>;
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Value;
    using difference_type   = ptrdiff_t;
    using size_type         = size_t;
    using reference         = Value&;
    using pointer           = Value*;

    node* cur_;
    hashtable* ht_;

    hashtable_iterator() : cur_(nullptr), ht_(nullptr) {}
    hashtable_iterator(node* n, hashtable* tab) : cur_(n), ht_(tab) {}
    hashtable_iterator(const iterator& it) : cur_(it.cur_), ht_(it.ht_) {}

    MSTL_NODISCARD reference operator *() const noexcept { return cur_->data_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }

    iterator& operator ++() {
        const node* old = cur_;
        cur_ = cur_->next_;
        if (cur_ == nullptr) {
            size_t bucket = ht_->bkt_num(old->data_, ht_->buckets_.size());
            while (cur_ == nullptr && ++bucket < ht_->buckets_.size())
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
template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator ==(
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh, 
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh) 
noexcept(noexcept(lh.cur_ == rh.cur_)) {
    return lh.cur_ == rh.cur_;
}
template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator !=(
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
    noexcept(noexcept(!(lh == rh))) {
    return !(lh == rh);
}

template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
struct hashtable_const_iterator {
    using hashtable         = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using iterator          = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using const_iterator    = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using node              = __hashtable_node<Value>;
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Value;
    using difference_type   = ptrdiff_t;
    using size_type         = size_t;
    using reference         = const Value&;
    using pointer           = const Value*;

    const node* cur_;
    const hashtable* ht_;

    hashtable_const_iterator() noexcept : cur_(nullptr), ht_(nullptr) {}
    hashtable_const_iterator(const node* n, const hashtable* tab)
        : cur_(n), ht_(tab) {}
    hashtable_const_iterator(const iterator& it) : cur_(it.cur_), ht_(it.ht_) {}

    MSTL_NODISCARD reference operator *() const noexcept { return cur_->data_; }
    MSTL_NODISCARD pointer operator ->() const noexcept { return &(operator*()); }

    const_iterator& operator ++() {
        const node* old = cur_;
        cur_ = cur_->next_;
        if (cur_ == nullptr) {
            size_t bucket = ht_->bkt_num(old->data_, ht_->buckets_.size());
            while (cur_ == nullptr && ++bucket < ht_->buckets_.size()) {
                cur_ = ht_->buckets_[bucket];
            }
        }
        return *this;
    }
    const_iterator operator ++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }
};
template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator ==(
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
    noexcept(noexcept(lh.cur_ == rh.cur_)) {
    return lh.cur_ == rh.cur_;
}
template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator !=(
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh)
    noexcept(noexcept(lh.cur_ != rh.cur_)) {
    return lh.cur_ != rh.cur_;
}

#ifdef MSTL_DATA_BUS_WIDTH_64__

static MSTL_CONSTEXPR uint16_t HASH_PRIMER_COUNT__ = 99;

static MSTL_CONSTEXPR size_t HASH_PRIME_LIST[HASH_PRIMER_COUNT__] = {
    101,                    173,                        263,                        397,
    599,                    907,                        1361,                       2053, 
    3083,                   4637,                       6959,                       10453, 
    15683,                  23531,                      35311,                      52967, 
    79451,                  119179,                     178781,                     268189, 
    402299,                 603457,                     905189,                     1357787,
    2036687,                3055043,                    4582577,                    6873871, 
    10310819,               15466229,                   23199347,                   34799021, 
    52198537,               78297827,                   117446801,                  176170229,
    264255353,              396383041,                  594574583,                  891861923, 
    1337792887,             2006689337,                 3010034021u,                4515051137ull, 
    6772576709ull,          10158865069ull,             15238297621ull,             22857446471ull,
    34286169707ull,         51429254599ull,             77143881917ull,             115715822899ull,
    173573734363ull,        260360601547ull,            390540902329ull,            585811353559ull, 
    878717030339ull,        1318075545511ull,           1977113318311ull,           2965669977497ull,
    4448504966249ull,       6672757449409ull,           10009136174239ull,          15013704261371ull,
    22520556392057ull,      33780834588157ull,          50671251882247ull,          76006877823377ull,
    114010316735089ull,     171015475102649ull,         256523212653977ull,         384784818980971ull,
    577177228471507ull,     865765842707309ull,         1298648764060979ull,        1947973146091477ull, 
    2921959719137273ull,    4382939578705967ull,        6574409368058969ull,        9861614052088471ull, 
    14792421078132871ull,   22188631617199337ull,       33282947425799017ull,       49924421138698549ull,
    74886631708047827ull,   112329947562071807ull,      168494921343107851ull,      252742382014661767ull,
    379113573021992729ull,  568670359532989111ull,      853005539299483657ull,      1279508308949225477ull,
    1919262463423838231ull, 2878893695135757317ull,     4318340542703636011ull,     6477510814055453699ull,
    9716266221083181299ull, 14574399331624771603ull,    18446744073709551557ull
};

MSTL_NODISCARD inline size_t hashtable_next_prime(size_t n) {
    const size_t* first = HASH_PRIME_LIST;
    const size_t* last = HASH_PRIME_LIST + HASH_PRIMER_COUNT__;
    const size_t* pos = MSTL::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

#else

static MSTL_CONSTEXPR uint16_t HASH_PRIMER_COUNT__ = 28;

static MSTL_CONSTEXPR uint32_t HASH_PRIME_LIST[HASH_PRIMER_COUNT__] = {
    53,         97,           193,         389,       769,
    1543,       3079,         6151,        12289,     24593,
    49157,      98317,        196613,      393241,    786433,
    1572869,    3145739,      6291469,     12582917,  25165843,
    50331653,   100663319,    201326611,   402653189, 805306457,
    1610612741, 3221225473u,  4294967291u
};

MSTL_NODISCARD inline size_t hashtable_next_prime(size_t n) {
    const uint32_t* first = HASH_PRIME_LIST;
    const uint32_t* last = HASH_PRIME_LIST + HASH_PRIMER_COUNT__;
    const uint32_t* pos = MSTL::lower_bound(first, last, n);
    return pos == last ? size_t(*(last - 1)) : size_t(*pos);
}

#endif

template <typename Value, typename Key, typename HashFcn, typename ExtractKey, typename EqualKey, typename Alloc>
class hashtable {
public:
    using key_type          = Key;
    using value_type        = Value;
    using hasher            = HashFcn;
    using key_equal         = EqualKey;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using node_type         = __hashtable_node<value_type>;
    using link_type         = node_type*;
    using allocator_type    = Alloc;
    using iterator          = hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using const_iterator    = hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    using self              = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    friend struct hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
    friend struct hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    template <typename Value1, typename Key1, typename HashFcn1,
        typename ExtractKey1, typename EqualKey1, typename Alloc1>
    friend bool operator ==(const hashtable<Value1, Key1, HashFcn1, ExtractKey1, EqualKey1, Alloc1>&,
        const hashtable<Value1, Key1, HashFcn1, ExtractKey1, EqualKey1, Alloc1>&) noexcept;

private:
    vector<link_type> buckets_;
    size_type size_;
    float factor_;
    MSTL_NO_UNIADS hasher hasher_;
    MSTL_NO_UNIADS key_equal equals_;
    MSTL_NO_UNIADS ExtractKey extracter_;
    MSTL_NO_UNIADS allocator_type alloc_;

    inline size_type next_size(size_type n) const noexcept {
        return hashtable_next_prime(n);
    }

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets_.reserve(n_buckets);
        buckets_.insert(buckets_.end(), n_buckets, nullptr);
        size_ = 0;
    }

    size_type bkt_num_key(const key_type& key, size_t n) const noexcept(is_nothrow_hashable_v<key_type>) {
        return hasher_(key) % n;
    }
    size_type bkt_num(const value_type& obj, size_t n) const noexcept(is_nothrow_hashable_v<key_type>) {
        return bkt_num_key(extracter_(obj), n);
    }

    template <typename... Args>
    node_type* new_node(Args&&... args) {
        node_type* n = alloc_.allocate();
        n->next_ = nullptr;
        MSTL_TRY__{
            MSTL::construct(&n->data_, MSTL::forward<Args>(args)...);
        }
        MSTL_CATCH_UNWIND_THROW_U__(delete_node(n));
        return n;
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
        buckets_.insert(buckets_.end(), ht.buckets_.size(), nullptr);
        MSTL_TRY__{
            for (size_type i = 0; i < ht.buckets_.size(); ++i) {
                if (const node_type* cur = ht.buckets_[i]) {
                    node_type* copy = (new_node)(cur->data_);
                    buckets_[i] = copy;
                    for (node_type* next = cur->next_; next != nullptr; cur = next, next = cur->next_) {
                        copy->next_ = (new_node)(next->data_);
                        copy = copy->next_;
                    }
                }
            }
            size_ = ht.size_;
            factor_ = ht.factor_;
        }
        MSTL_CATCH_UNWIND_THROW_M__(clear());
    }

    pair<iterator, bool> insert_unique_noresize(node_type* x) {
        const size_type n = bkt_num(x->data_, buckets_.size());
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur != nullptr; cur = cur->next_) {
            if (equals_(extracter_(cur->data_), extracter_(x->data_))) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                x->next_ = buckets_[n];
                buckets_[n] = x;
                return pair<iterator, bool>(iterator(x, this), false);
            }
        }
        x->next_ = first;
        buckets_[n] = x;
        ++size_;
        return pair<iterator, bool>(iterator(x, this), true);
    }

    iterator insert_equal_noresize(node_type* x) {
        const size_type n = bkt_num(x->data_, buckets_.size());
        node_type* first = buckets_[n];
        for (node_type* cur = first; cur != nullptr; cur = cur->next_) {
            if (equals_(extracter_(cur->data_), extracter_(x->data_))) {
                while (cur->next_ != nullptr) {
                    cur = cur->next_;
                }
                cur->next_ = x;
                x->next_ = nullptr;
                ++size_;
                return iterator(x, this);
            }
        }
        x->next_ = first;
        buckets_[n] = x;
        ++size_;
        return iterator(x, this);
    }

public:
    explicit hashtable(size_type n) 
        : hasher_(HashFcn()), equals_(EqualKey()), extracter_(ExtractKey()), size_(0), factor_(1.0f) {}

    hashtable(size_type n, const HashFcn& hf)
        : hasher_(hf), equals_(EqualKey()), extracter_(ExtractKey()), size_(0), factor_(1.0f) {
        initialize_buckets(n);
    }
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hasher_(hf), equals_(eql), extracter_(ExtractKey()), size_(0), factor_(1.0f) {
        initialize_buckets(n);
    }
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql, const ExtractKey& ext)
        : hasher_(hf), equals_(eql), extracter_(ext), size_(0), factor_(1.0f) {
        initialize_buckets(n);
    }

    hashtable(const self& ht)
        : hasher_(ht.hasher_), equals_(ht.equals_), extracter_(ht.extracter_), size_(0), factor_(1.0f) {
        copy_from(ht);
    }
    self& operator =(const self& ht) {
        if (MSTL::addressof(ht) == this) return *this;
        clear();
        hasher_ = ht.hasher_;
        equals_ = ht.equals_;
        extracter_ = ht.extracter_;
        copy_from(ht);
        return *this;
    }

    explicit hashtable(self&& ht) noexcept(noexcept(swap(ht)))
        : hasher_(ht.hasher_), equals_(ht.equals_), extracter_(ht.extracter_), size_(0), factor_(1.0f) {
        swap(ht);
    }
    self& operator =(self&& ht) noexcept(noexcept(swap(ht))) {
        if (MSTL::addressof(ht) == this) return *this;
        clear();
        swap(ht);
        return *this;
    }

    ~hashtable() { clear(); }

    MSTL_NODISCARD iterator begin() noexcept {
        for (size_type n = 0; n < buckets_.size(); ++n)
            if (buckets_[n] != nullptr)
                return iterator(buckets_[n], this);
        return end();
    }
    MSTL_NODISCARD iterator end() noexcept { return iterator(nullptr, this); }

    MSTL_NODISCARD const_iterator cbegin() const noexcept {
        for (size_type n = 0; n < buckets_.size(); ++n) {
            if (buckets_[n] != nullptr) 
                return const_iterator(buckets_[n], this);
        }
        return cend();
    }
    MSTL_NODISCARD const_iterator cend() const noexcept {
        return const_iterator(nullptr, this);
    }

    MSTL_NODISCARD size_type size() const noexcept { return size_; }
    MSTL_NODISCARD size_type max_size() const noexcept { return static_cast<size_type>(-1); }
    MSTL_NODISCARD bool empty() const noexcept { return size_ == 0; }
    MSTL_NODISCARD size_type bucket_count() const noexcept { return buckets_.size(); }
    MSTL_NODISCARD size_type max_bucket_count() const noexcept {
        return HASH_PRIME_LIST[HASH_PRIMER_COUNT__ - 1];
    }
    MSTL_NODISCARD size_type bucket(const key_type& key) const noexcept(is_nothrow_hashable_v<key_type>) {
        return bkt_num_key(key);
    }
    MSTL_NODISCARD size_type bucket_size(size_type bucket) const noexcept {
        size_type result = 0;
        for (node_type* cur = buckets_[bucket]; cur != nullptr; cur = cur->next_)
            result++;
        return result;
    }

    MSTL_NODISCARD allocator_type get_allocator() const noexcept { return allocator_type(); }

    MSTL_NODISCARD hasher hash_func() const noexcept(is_nothrow_copy_constructible_v<hasher>) {
        return hasher_;
    }
    MSTL_NODISCARD key_equal key_eql() const noexcept(is_nothrow_copy_constructible_v<key_equal>) {
        return equals_;
    }
    MSTL_NODISCARD float load_factor() const noexcept {
        return static_cast<float>(size()) / static_cast<float>(bucket_count());
    }
    MSTL_NODISCARD float max_load_factor() const noexcept {
        return factor_;
    }
    void max_load_factor(float new_max) noexcept {
        MSTL_DEBUG_VERIFY__(!std::isnan(new_max) && new_max > 0, "hashtable load factor invalid.");
        factor_ = new_max;
    }

    void rehash(size_type new_size) {
        const size_type old_size = buckets_.size();
        if (new_size <= old_size) return;
        const size_type n = next_size(new_size);
        if (n <= old_size) return;
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

    void reserve(size_type max_count) {
        rehash(static_cast<size_type>(MSTL::ceil(static_cast<float>(max_count) / factor_, 0)));
    }

    template <typename... Args>
    pair<iterator, bool> emplace_unique(Args&&... args) {
        rehash(size_ + 1);
        node_type* node = (new_node)(MSTL::forward<Args>(args)...);
        return (insert_unique_noresize)(node);
    }
    template <typename... Args>
    iterator emplace_equal(Args&&... args) {
        rehash(size_ + 1);
        node_type* node = (new_node)(MSTL::forward<Args>(args)...);
        return (insert_equal_noresize)(node);
    }

    pair<iterator, bool> insert_unique(const value_type& x) {
        return (emplace_unique)(x);
    }
    pair<iterator, bool> insert_unique(value_type&& x) {
        return (emplace_unique)(MSTL::move(x));
    }
    iterator insert_equal(const value_type& x) {
        return (emplace_equal)(x);
    }
    iterator insert_equal(value_type&& x) {
        return (emplace_equal)(MSTL::move(x));
    }

    template <typename Iterator, enable_if_t<
        is_iter_v<Iterator>, int> = 0>
    void insert_unique(Iterator first, Iterator last) {
        if constexpr (is_fwd_iter_v<Iterator>) {
            size_type n = MSTL::distance(first, last);
            rehash(size_ + n);
            for (; n > 0; --n, ++first) {
                node_type* tmp = (new_node)(*first);
                insert_unique_noresize(tmp);
            }
        }
        else {
            for (; first != last; ++first)
                insert_unique(*first);
        }
    }

    void insert_unique(std::initializer_list<value_type> l) {
        insert_unique(l.begin(), l.end());
    }

    template <typename Iterator, enable_if_t<
        is_iter_v<Iterator>, int> = 0>
    void insert_equal(Iterator first, Iterator last) {
        if constexpr (is_fwd_iter_v<Iterator>) {
            size_type n = 0;
            MSTL::distance(first, last, n);
            rehash(size_ + n);
            for (; n > 0; --n, ++first) {
                node_type* tmp = (new_node)(*first);
                insert_equal_noresize(tmp);
            }
        }
        else {
            for (; first != last; ++first)
                insert_equal(*first);
        }
    }

    void insert_equal(std::initializer_list<value_type> l) {
        insert_equal(l.begin(), l.end());
    }

    size_type erase(const key_type& key) noexcept(is_nothrow_hashable_v<key_type>) {
        const size_type n = bkt_num_key(key, buckets_.size());
        node_type* first = buckets_[n];
        size_type erased = 0;
        if (first != nullptr) {
            node_type* cur = first;
            node_type* next = cur->next_;
            while (next != nullptr) {
                if (equals_(extracter_(next->data_), key)) {
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
            if (equals_(extracter_(first->data_), key)) {
                buckets_[n] = first->next_;
                delete_node(first);
                ++erased;
                --size_;
            }
        }
        return erased;
    }
    void erase(const iterator& it) noexcept(is_nothrow_hashable_v<key_type>) {
        node_type* const p = it.cur_;
        if (p != nullptr) {
            const size_type n = bkt_num(p->data_, buckets_.size());
            node_type* cur = buckets_[n];
            if (cur == p) {
                buckets_[n] = cur->next_;
                delete_node(cur);
                --size_;
            }
            else {
                node_type* next = cur->next_;
                while (next != nullptr) {
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
    void erase(iterator first, iterator last) noexcept(is_nothrow_hashable_v<key_type>) {
        size_type f_bucket = first.cur_ ? first.hash_val_ : buckets_.size();
        size_type l_bucket = last.cur_ ? last.hash_val_ : buckets_.size();
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

    void erase(const const_iterator& it) noexcept(is_nothrow_hashable_v<key_type>) {
        erase(iterator(const_cast<node_type*>(it.cur_), const_cast<hashtable*>(it.ht_)));
    }
    void erase(const_iterator first, const_iterator last) noexcept(is_nothrow_hashable_v<key_type>) {
        erase(iterator(const_cast<node_type*>(first.cur_), const_cast<hashtable*>(first.ht_)),
            iterator(const_cast<node_type*>(last.cur_), const_cast<hashtable*>(last.ht_)));
    }

    void clear() noexcept {
        for (size_type i = 0; i < buckets_.size(); ++i) {
            node_type* cur = buckets_[i];
            while (cur != nullptr) {
                node_type* next = cur->next_;
                delete_node(cur);
                cur = next;
            }
            buckets_[i] = 0;
        }
        size_ = 0;
    }

    void swap(self& ht) noexcept(is_nothrow_swappable_v<HashFcn> && is_nothrow_swappable_v<EqualKey>) {
        if (MSTL::addressof(ht) == this) return;
        MSTL::swap(hasher_, ht.hasher_);
        MSTL::swap(equals_, ht.equals_);
        MSTL::swap(extracter_, ht.extracter_);
        buckets_.swap(ht.buckets_);
        MSTL::swap(size_, ht.size_);
        MSTL::swap(factor_, ht.factor_);
    }

    MSTL_NODISCARD iterator find(const key_type& key) noexcept(is_nothrow_hashable_v<key_type>) {
        size_type n = bkt_num_key(key, buckets_.size());
        node_type* first;
        for (first = buckets_[n]; first != nullptr && !equals_(extracter_(first->data_), key);
            first = first->next_);
        return iterator(first, this);
    }

    MSTL_NODISCARD const_iterator find(const key_type& key) const noexcept(is_nothrow_hashable_v<key_type>) {
        size_type n = bkt_num_key(key, buckets_.size());
        const node_type* first;
        for (first = buckets_[n]; first != nullptr && !equals_(extracter_(first->data_), key);
            first = first->next_);
        return const_iterator(first, this);
    }

    MSTL_NODISCARD size_type count(const key_type& key) const noexcept(is_nothrow_hashable_v<key_type>) {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for (const node_type* cur = buckets_[n]; cur != nullptr; cur = cur->next_) {
            if (equals_(extracter_(cur->data_), key)) ++result;
        }
        return result;
    }
    MSTL_NODISCARD bool contains(const key_type& key) const noexcept(is_nothrow_hashable_v<key_type>) {
        return find(key) != cend();
    }

    MSTL_NODISCARD pair<iterator, iterator> equal_range(const key_type& key) {
        typedef pair<iterator, iterator> iter_pair;
        const size_type n = bkt_num_key(key);
        for (node_type* first = buckets_[n]; first != nullptr; first = first->next_) {
            if (equals_(extracter_(first->data_), key)) {
                for (node_type* cur = first->next_; cur != nullptr; cur = cur->next_) {
                    if (!equals_(extracter_(cur->data_), key))
                        return iter_pair(iterator(first, this), iterator(cur, this));
                }
                for (size_type m = n + 1; m < buckets_.size(); ++m) {
                    if (buckets_[m] != nullptr)
                        return iter_pair(iterator(first, this),
                            iterator(buckets_[m], this));
                }
                return iter_pair(iterator(first, this), end());
            }
        }
        return iter_pair(end(), end());
    }

    MSTL_NODISCARD pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        typedef pair<const_iterator, const_iterator> citer_pair;
        const size_type n = bkt_num_key(key);
        for (const node_type* first = buckets_[n]; first != nullptr; first = first->next_) {
            if (equals_(extracter_(first->data_), key)) {
                for (const node_type* cur = first->next_; cur != nullptr; cur = cur->next_) {
                    if (!equals_(extracter_(cur->data_), key))
                        return citer_pair(const_iterator(first, this), const_iterator(cur, this));
                }
                for (size_type m = n + 1; m < buckets_.size(); ++m) {
                    if (buckets_[m] != nullptr)
                        return citer_pair(const_iterator(first, this), const_iterator(buckets_[m], this));
                }
                return citer_pair(const_iterator(first, this), end());
            }
        }
        return citer_pair(end(), end());
    }

    // merge 
    // extract
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
        for (; cur1 != nullptr && cur2 != nullptr && cur1->data_ == cur2->data_; 
            cur1 = cur1->next_, cur2 = cur2->next_);
        if (cur1 != nullptr || cur2 != nullptr) return false;
    }
    return true;
}
template <typename Value, typename Key, typename HashFcn,
    typename ExtractKey, typename EqualKey, typename Alloc>
MSTL_NODISCARD bool operator !=(
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& lh,
    const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>& rh) noexcept {
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

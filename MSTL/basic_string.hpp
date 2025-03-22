#ifndef MSTL_BASIC_STRING_H__
#define MSTL_BASIC_STRING_H__
#include "string_view.hpp"
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename CharT, typename Ref = CharT&, typename Ptr = CharT*>
class basic_string_iterator {
public:
#ifdef MSTL_VERSION_20__
    using iterator_category = contiguous_iterator_tag;
#else
    using iterator_category = random_access_iterator_tag;
#endif // MSTL_VERSION_20__
    using value_type        = CharT;
    using pointer           = Ptr;
    using reference         = Ref;
    using difference_type   = ptrdiff_t;
    using self              = basic_string_iterator<CharT, Ref, Ptr>;

private:
    pointer ptr_ = nullptr;

public:
    MSTL_CONSTEXPR20 basic_string_iterator() noexcept = default;
    MSTL_CONSTEXPR20 basic_string_iterator(pointer p) noexcept : ptr_(p) {}
    MSTL_CONSTEXPR20 basic_string_iterator(const self&) noexcept = default;
    MSTL_CONSTEXPR20 ~basic_string_iterator() = default;

    MSTL_NODISCARD MSTL_CONSTEXPR20 reference operator *()  const noexcept { return *ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 pointer operator ->() const noexcept { return &(operator*()); }

    MSTL_CONSTEXPR20 self& operator ++() noexcept { ++ptr_;
        return *this;
    }
    MSTL_CONSTEXPR20 self  operator ++(int) noexcept {
        self tmp = *this;
        ++ptr_;
        return tmp;
    }

    MSTL_CONSTEXPR20 self& operator --() noexcept {
        --ptr_; 
        return *this;
    }
    MSTL_CONSTEXPR20 self  operator --(int) noexcept {
        self tmp = *this;
        --ptr_;
        return tmp;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 self operator +(const difference_type n) const noexcept { return self(ptr_ + n); }
    MSTL_NODISCARD friend MSTL_CONSTEXPR20 self operator +(const difference_type n, const self& it) noexcept {
        return it + n;
    }

    MSTL_CONSTEXPR20 self& operator +=(difference_type n) noexcept {
        ptr_ += n;
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 self operator -(difference_type n) const noexcept { return self(ptr_ - n); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 difference_type operator -(const self& rhs) const noexcept { return ptr_ - rhs.ptr_; }

    MSTL_CONSTEXPR20 self& operator -=(difference_type n) noexcept {
        ptr_ -= n;
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 reference operator [](difference_type n) const noexcept { return ptr_[n]; }

    MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(const self& rhs) const noexcept { return ptr_ == rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(const self& rhs) const noexcept { return ptr_ != rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <(const self& rhs) const noexcept { return ptr_ < rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >(const self& rhs) const noexcept { return ptr_ > rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <=(const self& rhs) const noexcept { return ptr_ <= rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >=(const self& rhs) const noexcept { return ptr_ >= rhs.ptr_; }
};

template <typename CharT, typename Traits = char_traits<CharT>, typename Alloc = standard_allocator<CharT>>
class basic_string {
#ifdef MSTL_VERSION_20__	
    static_assert(is_allocator_v<Alloc>, "Alloc type is not a standard allocator type.");
#endif
    static_assert(is_same_v<CharT, typename Alloc::value_type>, "allocator type mismatch.");
    static_assert(is_same_v<CharT, typename Traits::char_type>, "trait type mismatch.");
    static_assert(!is_array_v<CharT> && is_trivial_v<CharT> && is_standard_layout_v<CharT>, 
        "basic string only contains non-array trivial standard-layout types.");

public:
    MSTL_BUILD_TYPE_ALIAS(CharT)

    using traits_type   = Traits;
    using view_type     = basic_string_view<CharT, Traits>;

    using iterator                  = basic_string_iterator<CharT, reference, pointer>;
    using const_iterator            = basic_string_iterator<CharT, const_reference, const_pointer>;
    using reverse_iterator          = _MSTL reverse_iterator<iterator>;
    using const_reverse_iterator    = _MSTL reverse_iterator<const_iterator>;

    using self              = basic_string<CharT, Traits, Alloc>;
    using allocator_type    = Alloc;

    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    pointer data_ = nullptr;
    size_type size_ = 0;
    compressed_pair<allocator_type, size_type> alloc_pair_{ default_construct_tag{}, 0 };

    MSTL_CONSTEXPR20 void range_check(const size_type n) const noexcept {
        MSTL_DEBUG_VERIFY__(n < size_, "basic_string index out of ranges.");
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type clamp_size(const size_type position, const size_type size) const noexcept {
        return _MSTL min(size, size_ - position);
    }

    MSTL_CONSTEXPR20 void set_empty() noexcept {
        data_ = nullptr;
        size_ = 0;
        alloc_pair_.value = 0;
    }

    template <typename Iterator, enable_if_t<
        is_iter_v<Iterator> && !is_fwd_iter_v<Iterator>, int> = 0>
    MSTL_CONSTEXPR20 void construct_from_iter(Iterator first, Iterator last) {
        size_type n = _MSTL distance(first, last);
        const size_type init_size = _MSTL max(MEMORY_ALIGN_THRESHHOLD, n + 1);
        MSTL_TRY__{
            data_ = alloc_pair_.get_base().allocate(init_size);
            size_ = n;
            alloc_pair_.value = init_size;
        }
        MSTL_CATCH_UNWIND_THROW_M__(destroy_buffer())
        for (; n > 0; --n, ++first) append(*first);
    }

    template <typename Iterator, enable_if_t<is_fwd_iter_v<Iterator>, int> = 0>
    MSTL_CONSTEXPR20 void construct_from_iter(Iterator first, Iterator last) {
        const size_type n = _MSTL distance(first, last);
        const size_type init_size = _MSTL max(MEMORY_ALIGN_THRESHHOLD, n + 1);
        MSTL_TRY__{
            data_ = alloc_pair_.get_base().allocate(init_size);
            size_ = n;
            alloc_pair_.value = init_size;
            _MSTL uninitialized_copy(first, last, data_);
        }
        MSTL_CATCH_UNWIND_THROW_M__(destroy_buffer())
    }

    MSTL_CONSTEXPR20 void construct_from_ptr(const_pointer str, size_type position, size_type n) {
        const size_type init_size = _MSTL max(MEMORY_ALIGN_THRESHHOLD, n + 1);
        data_ = alloc_pair_.get_base().allocate(init_size);
        traits_type::copy(data_, str + position, n);
        traits_type::assign(data_ + n, 1, initialize<CharT>());
        size_ = n;
        alloc_pair_.value = init_size;
    }

    MSTL_CONSTEXPR20 void destroy_buffer() noexcept {
        if (data_ == nullptr) return;
        alloc_pair_.get_base().deallocate(data_, alloc_pair_.value);
        set_empty();
    }

    MSTL_CONSTEXPR20 basic_string& replace_fill(iterator first, size_type n1, const size_type n2, const value_type chr) {
        if (static_cast<size_type>(end() - first) < n1)
            n1 = cend() - first;
        if (n1 < n2) {
            const size_type diff = n2 - n1;
            MSTL_DEBUG_VERIFY__(size_ + diff < max_size(), "basic_string index out of range.");
            if (size_ > alloc_pair_.value - diff)
                reallocate(diff);
            pointer raw_ptr = &*first;
            traits_type::move(raw_ptr + n2, raw_ptr + n1, end() - (first + n1));
            traits_type::assign(raw_ptr, n2, chr);
            size_ += diff;
        }
        else {
            pointer raw_ptr = &*first;
            traits_type::move(raw_ptr + n2, raw_ptr + n1, end() - (first + n1));
            traits_type::assign(raw_ptr, n2, chr);
            size_ -= n1 - n2;
        }
        return *this;
    }

    template <typename Iterator, enable_if_t<
        is_iter_v<Iterator> && is_same_v<iter_val_t<Iterator>, value_type>, int> = 0>
    MSTL_CONSTEXPR20 basic_string& replace_copy(iterator first1, iterator last1, Iterator first2, Iterator last2) {
        size_type len1 = _MSTL distance(first1, last1);
        size_type len2 = _MSTL distance(first2, last2);
        if (len1 < len2) {
            const size_type diff = len2 - len1;
            MSTL_DEBUG_VERIFY__(size_ + diff < max_size(), "basic_string replace_copy index out of range.");
            if (size_ > alloc_pair_.value - diff)
                reallocate(diff);
            pointer raw_ptr = &*first1;
            traits_type::move(raw_ptr + len2, raw_ptr + len1, end() - (first1 + len1));
            traits_type::copy(raw_ptr, &*first2, len2);
            size_ += diff;
        }
        else {
            pointer raw_ptr = &*first1;
            traits_type::move(raw_ptr + len2, raw_ptr + len1, end() - (first1 + len1));
            traits_type::copy(raw_ptr, &*first2, len2);
            size_ -= len1 - len2;
        }
        return *this;
    }
    template <typename Iterator>
    MSTL_CONSTEXPR20 basic_string& replace_copy(iterator first1, const size_type n1, Iterator first2, const size_type n2) {
        return replace_copy(first1, first1 + n1, first2, _MSTL next(first2, n2));
    }

    MSTL_CONSTEXPR20 void reallocate(size_type n) {
        const size_t new_cap = _MSTL max(alloc_pair_.value + n, alloc_pair_.value + (alloc_pair_.value >> 1));
        pointer new_buffer = alloc_pair_.get_base().allocate(new_cap);
        traits_type::move(new_buffer, data_, size_);
        alloc_pair_.get_base().deallocate(data_, alloc_pair_.value);
        data_ = new_buffer;
        alloc_pair_.value = new_cap;
    }

    MSTL_CONSTEXPR20 iterator reallocate_and_fill(iterator position, size_type n, value_type chr) {
        const difference_type diff = (&*position) - data_;
        const size_t old_cap = alloc_pair_.value;
        const size_t new_cap = _MSTL max(old_cap + n, old_cap + (old_cap >> 1));
        pointer new_buffer = alloc_pair_.get_base().allocate(new_cap);
        pointer end1 = traits_type::move(new_buffer, data_, diff) + diff;
        pointer end2 = traits_type::assign(end1, n, chr) + n;
        traits_type::move(end2, data_ + diff, size_ - diff);
        alloc_pair_.get_base().deallocate(data_, old_cap);
        data_ = new_buffer;
        size_ += n;
        alloc_pair_.value = new_cap;
        return data_ + diff;
    }
    MSTL_CONSTEXPR20 iterator reallocate_and_copy(iterator position, const_iterator first, const_iterator last) {
        const difference_type diff = position - data_;
        const size_type old_cap = alloc_pair_.value;
        const size_type n = _MSTL distance(first, last);
        const size_t new_cap = _MSTL max(old_cap + n, old_cap + (old_cap >> 1));
        pointer new_buffer = alloc_pair_.get_base().allocate(new_cap);
        pointer end1 = traits_type::move(new_buffer, data_, diff) + diff;
        pointer end2 = _MSTL uninitialized_copy_n(first, n, end1) + n;
        traits_type::move(end2, data_ + diff, size_ - diff);
        alloc_pair_.get_base().deallocate(data_, old_cap);
        data_ = new_buffer;
        size_ += n;
        alloc_pair_.value = new_cap;
        return data_ + diff;
    }

public:
    MSTL_CONSTEXPR20 basic_string() {
        MSTL_TRY__{
            data_ = alloc_pair_.get_base().allocate(static_cast<size_type>(MEMORY_ALIGN_THRESHHOLD));
            size_ = 0;
            alloc_pair_.value = 0;
        }
        MSTL_CATCH_UNWIND__{ set_empty(); }
    }

    MSTL_CONSTEXPR20 basic_string(size_type n, value_type chr) {
        const size_type init_size = _MSTL max(MEMORY_ALIGN_THRESHHOLD, n + 1);
        data_ = alloc_pair_.get_base().allocate(init_size);
        traits_type::assign(data_, n, chr);
        size_ = n;
        alloc_pair_.value = init_size;
    }
    MSTL_CONSTEXPR20 self& operator =(value_type chr) {
        if (alloc_pair_.value < 1) {
            pointer new_buffer = alloc_pair_.get_base().allocate(2);
            alloc_pair_.get_base().deallocate(data_);
            data_ = new_buffer;
            alloc_pair_.value = 2;
        }
        *data_ = chr;
        size_ = 1;
        return *this;
    }

    MSTL_CONSTEXPR20 basic_string(const self& str) {
        construct_from_ptr(str.data_, 0, str.size_);
    }
    MSTL_CONSTEXPR20 self& operator =(const self& str) {
        if (_MSTL addressof(str) == this) return *this;
        construct_from_ptr(str.data_, 0, str.size_);
        return *this;
    }

    MSTL_CONSTEXPR20 basic_string(self&& str) noexcept
        : data_(_MSTL move(str.data_)), size_(str.size_),
        alloc_pair_(default_construct_tag{}, str.alloc_pair_.value) {
        str.set_empty();
    }
    MSTL_CONSTEXPR20 self& operator =(self&& str) noexcept {
        if (_MSTL addressof(str) == this) return *this;
        clear();
        swap(str);
        return *this;
    }

    MSTL_CONSTEXPR20 basic_string(view_type str) {
        construct_from_ptr(str.data(), 0, str.size());
    }
    MSTL_CONSTEXPR20 basic_string(view_type str, const size_type n) {
        construct_from_ptr(str.data(), 0, n);
    }
    MSTL_CONSTEXPR20 self& operator =(view_type str) {
        const size_type len = str.size();
        if (alloc_pair_.value < len) {
            pointer new_buffer = alloc_pair_.get_base().allocate(len + 1);
            alloc_pair_.get_base().deallocate(data_);
            data_ = new_buffer;
            alloc_pair_.value = len + 1;
        }
        traits_type::copy(data_, str.data(), len);
        size_ = len;
        return *this;
    }

    MSTL_CONSTEXPR20 basic_string(const self& str, size_type position) {
        construct_from_ptr(str.data_, position, str.size_ - position);
    }
    MSTL_CONSTEXPR20 basic_string(const self& str, size_type position, size_type n) {
        construct_from_ptr(str.data_, position, n);
    }

    MSTL_CONSTEXPR20 basic_string(const_pointer str) {
        construct_from_ptr(str, 0, traits_type::length(str));
    }
    MSTL_CONSTEXPR20 basic_string(const_pointer str, const size_type n) {
        construct_from_ptr(str, 0, n);
    }
    MSTL_CONSTEXPR20 self& operator =(const_pointer str) {
        const size_type len = traits_type::length(str);
        if (alloc_pair_.value < len) {
            pointer new_buffer = alloc_pair_.get_base().allocate(len + 1);
            alloc_pair_.get_base().deallocate(data_);
            data_ = new_buffer;
            alloc_pair_.value = len + 1;
        }
        traits_type::copy(data_, str, len);
        size_ = len;
        return *this;
    }

    template <typename Iterator>
    MSTL_CONSTEXPR20 basic_string(Iterator first, Iterator last) {
        construct_from_iter(first, last);
    }

    MSTL_CONSTEXPR20 basic_string(std::initializer_list<value_type> l) : basic_string(l.begin(), l.end()) {}
    MSTL_CONSTEXPR20 self& operator =(std::initializer_list<value_type> l) {
        clear();
        insert(begin(), l.begin(), l.end());
        return *this;
    }

    MSTL_CONSTEXPR20 ~basic_string() { destroy_buffer(); }

    MSTL_NODISCARD MSTL_CONSTEXPR20 iterator begin() noexcept { return data_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 iterator end() noexcept { return data_ + size_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_iterator cbegin() const noexcept { return data_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_iterator cend() const noexcept { return data_ + size_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type size() const noexcept { return size_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type max_size() const noexcept { return npos; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type capacity() const noexcept { return alloc_pair_.value; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type length()   const noexcept { return size_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 bool empty() const noexcept { return size_ == 0; }

    MSTL_NODISCARD MSTL_CONSTEXPR20 allocator_type get_allocator() const noexcept { return allocator_type(); }

    MSTL_CONSTEXPR20 void reserve(size_type n) {
        MSTL_DEBUG_VERIFY__(n < max_size(), "basic_string reserve index out of range.");
        if (alloc_pair_.value >= n) return;
        pointer new_buffer = alloc_pair_.get_base().allocate(n);
        traits_type::move(new_buffer, data_, size_);
        data_ = new_buffer;
        alloc_pair_.value = n;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 reference operator [](const size_type n) {
        MSTL_DEBUG_VERIFY__(n <= size_, "basic_string [] index out of range.");
        return *(data_ + n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference operator [](const size_type n) const {
        MSTL_DEBUG_VERIFY__(n <= size_, "basic_string [] index out of range.");
        return *(data_ + n);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 reference at(const size_type n) {
        range_check(n);
        return (*this)[n];
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference at(const size_type n) const {
        range_check(n);
        return (*this)[n];
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 reference front() {
        MSTL_DEBUG_VERIFY__(!empty(), "front called on empty basic_string");
        return *data_;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference front() const {
        MSTL_DEBUG_VERIFY__(!empty(), "front called on empty basic_string");
        return *data_;
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 reference back() {
        MSTL_DEBUG_VERIFY__(!empty(), "back called on empty basic_string");
        return *(data_ + size_ - 1);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_reference back() const {
        MSTL_DEBUG_VERIFY__(!empty(), "back called on empty basic_string");
        return *(data_ + size_ - 1);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 const_pointer data() const noexcept { return data_; }
    MSTL_NODISCARD MSTL_CONSTEXPR20 const_pointer c_str() const noexcept { return data_; }

    MSTL_CONSTEXPR20 iterator insert(iterator position, value_type chr) {
        if (size_ == alloc_pair_.value)
            return reallocate_and_fill(position, 1, chr);
        traits_type::move(position + 1, position, end() - position);
        ++size_;
        *position = chr;
        return position;
    }

    MSTL_CONSTEXPR20 iterator insert(iterator position, size_type n, value_type chr) {
        if (n == 0) return position;
        if (alloc_pair_.value - size_ < n)
            return reallocate_and_fill(position, n, chr);
        if (position == end()) {
            traits_type::assign(end(), n, chr);
            size_ += n;
            return position;
        }
        traits_type::move(position + n, position, n);
        traits_type::assign(position, n, chr);
        size_ += n;
        return position;
    }

    template <typename Iterator>
    MSTL_CONSTEXPR20 iterator insert(iterator position, Iterator first, Iterator last) {
        const size_type len = _MSTL distance(first, last);
        if (len == 0) return position;

        if (alloc_pair_.value - size_ < len)
            return reallocate_and_copy(position, first, last);
        if (position == end()) {
            _MSTL uninitialized_copy(first, last, end());
            size_ += len;
            return position;
        }
        traits_type::move(position + len, position, len);
        _MSTL uninitialized_copy(first, last, position);
        size_ += len;
        return position;
    }

    MSTL_CONSTEXPR20 void push_back(value_type chr) { append(1, chr); }
    MSTL_CONSTEXPR20 void pop_back() noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "pop_back called on empty basic_string");
        --size_;
    }

    MSTL_CONSTEXPR20 self& append(size_type n, value_type chr) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "basic_string append iterator out of ranges.");
        if (alloc_pair_.value - size_ < n)
            reallocate(n);
        traits_type::assign(data_ + size_, n, chr);
        size_ += n;
        return *this;
    }
    MSTL_CONSTEXPR20 self& append(value_type chr) { return append(1, chr); }

    MSTL_CONSTEXPR20 self& append(const self& str, size_type position, size_type n) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "basic_string append iterator out of ranges.");
        if (n == 0) return *this;
        if (alloc_pair_.value - size_ < n) reallocate(n);
        traits_type::copy(data_ + size_, str.data_ + position, n);
        size_ += n;
        return *this;
    }
    MSTL_CONSTEXPR20 self& append(const self& str) { return append(str, 0, str.size_); }
    MSTL_CONSTEXPR20 self& append(const self& str, size_type position) {
        return append(str, position, str.size_ - position);
    }

    MSTL_CONSTEXPR20 self& append(self&& str, size_type position, size_type n) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "basic_string append iterator out of ranges.");
        if (n == 0) return *this;
        if (alloc_pair_.value - size_ < n) reallocate(n);
        traits_type::move(data_ + size_, str.data_ + position, n);
        size_ += n;
        str.destroy_buffer();
        return *this;
    }
    MSTL_CONSTEXPR20 self& append(self&& str) { return append(_MSTL move(str), 0, str.size_); }
    MSTL_CONSTEXPR20 self& append(self&& str, size_type position) {
        return append(_MSTL move(str), position, str.size_ - position);
    }

    MSTL_CONSTEXPR20 self& append(view_type str, size_type n) {
        return append(str.data(), n);
    }

    MSTL_CONSTEXPR20 self& append(view_type str) { return append(str.data(), str.size()); }

    MSTL_CONSTEXPR20 self& append(const_pointer str, size_type n) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "basic_string append iterator out of ranges.");
        if (alloc_pair_.value - size_ < n) reallocate(n);
        traits_type::copy(data_ + size_, str, n);
        size_ += n;
        return *this;
    }
    MSTL_CONSTEXPR20 self& append(const_pointer str) { return append(str, traits_type::length(str)); }

    template <typename Iterator, enable_if_t<is_iter_v<Iterator>, int> = 0>
    MSTL_CONSTEXPR20 self& append(Iterator first, Iterator last) {
        const size_type n = _MSTL distance(first, last);
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "basic_string append iterator out of ranges.");
        if (alloc_pair_.value - size_ < n)
            reallocate(n);
        _MSTL uninitialized_copy_n(first, n, data_ + size_);
        size_ += n;
        return *this;
    }

    MSTL_CONSTEXPR20 self& append(std::initializer_list<value_type> l) {
        return append(l.begin(), l.end());
    }

    MSTL_CONSTEXPR20 self& operator +=(const self& str) { return append(str); }
    MSTL_CONSTEXPR20 self& operator +=(self&& str) { return append(_MSTL move(str)); }
    MSTL_CONSTEXPR20 self& operator +=(value_type chr) { return append(chr); }
    MSTL_CONSTEXPR20 self& operator +=(const_pointer str) { return append(str, str + traits_type::length(str)); }
    MSTL_CONSTEXPR20 self& operator +=(std::initializer_list<value_type> lls) { return append(lls); }
    MSTL_CONSTEXPR20 self& operator +=(view_type view) { return append(view); }

    MSTL_CONSTEXPR20 iterator erase(iterator position) {
        MSTL_DEBUG_VERIFY__(position != end(), "");
        traits_type::move(position, position + 1, end() - position - 1);
        --size_;
        return position;
    }
    MSTL_CONSTEXPR20 iterator erase(iterator first, iterator last) {
        if (first == begin() && last == end()) {
            clear();
            return end();
        }
        const size_type n = end() - last;
        traits_type::move(first, last, n);
        size_ -= (last - first);
        return first;
    }

    MSTL_CONSTEXPR20 void resize(size_type count, value_type chr) {
        if (count < size_)
            erase(data_ + count, data_ + size_);
        else
            append(count - size_, chr);
    }
    MSTL_CONSTEXPR20 void resize(const size_type count) { resize(count, value_type()); }

    MSTL_CONSTEXPR20 void clear() noexcept {
        if (data_ != nullptr) data_[0] = value_type();
        size_ = 0;
    }


    MSTL_NODISCARD MSTL_CONSTEXPR20 self substr(const size_type off = 0, size_type count = npos) const {
        range_check(off);
        count = clamp_size(off, count);
        return self(data_ + off, count);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 int compare(const self& str) const noexcept {
        return (char_traits_compare<Traits>)(data_, size_, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 int compare(const size_type off, const size_type n, const self& str) const {
        return substr(off, n).compare(str);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 int compare(const size_type off, const size_type n, const self& str,
        const size_type roff, const size_type count) const {
        return substr(off, n).compare(str.substr(roff, count));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 int compare(const CharT* const str) const noexcept {
        return compare(self(str));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 int compare(const size_type off, const size_type n, const CharT* const str) const {
        return substr(off, n).compare(self(str));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 int compare(const size_type off, const size_type n,
        const CharT* const str, const size_type count) const {
        return substr(off, n).compare(self(str, count));
    }

    MSTL_CONSTEXPR20 self& replace(const size_type position, const size_type n, const self& str) {
        range_check(position);
        return replace_copy(data_ + position, n, str.data_, str.size_);
    }
    MSTL_CONSTEXPR20 self& replace(iterator first, iterator last, const self& str) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "basic_string replace iterator out of ranges.");
        return replace_copy(first, last - first, str.data_, str.size_);
    }

    MSTL_CONSTEXPR20 self& replace(const size_type position, const size_type n, const_pointer str) {
        range_check(position);
        return replace_copy(data_ + position, n, str, traits_type::length(str));
    }
    MSTL_CONSTEXPR20 self& replace(iterator first, iterator last, const_pointer str) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "basic_string replace iterator out of ranges.");
        return replace_copy(first, last - first, str, traits_type::length(str));
    }

    MSTL_CONSTEXPR20 self& replace(const size_type position, const size_type n1, const_pointer str, const size_type n2) {
        range_check(position);
        return replace_copy(data_ + position, n1, str, n2);
    }
    MSTL_CONSTEXPR20 self& replace(iterator first, iterator last, const_pointer str, const size_type n) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "basic_string replace iterator out of ranges.");
        return replace_copy(first, last - first, str, n);
    }

    MSTL_CONSTEXPR20 self& replace(const size_type position, const size_type n1, const size_type n2, const value_type chr) {
        range_check(position);
        return replace_fill(data_ + position, n1, n2, chr);
    }
    MSTL_CONSTEXPR20 self& replace(iterator first, iterator last, const size_type n, const value_type chr) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "basic_string replace iterator out of ranges.");
        return replace_fill(first, static_cast<size_type>(last - first), n, chr);
    }

    MSTL_CONSTEXPR20 self& replace(const size_type position1, const size_type n1, const self& str,
        const size_type position2, const size_type n2 = npos) {
        range_check(position1);
        range_check(position2);
        return replace_copy(data_ + position1, n1, str.data_ + position2, n2);
    }

    template <typename Iterator>
    MSTL_CONSTEXPR20 self& replace(iterator first, iterator last, Iterator first2, Iterator last2) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "basic_string replace iterator out of ranges.");
        return replace_copy(first, last, first2, last2);
    }

    MSTL_CONSTEXPR20 void reverse() noexcept {
        if (size() < 2) return;
        for (iterator first = begin(), last = end(); first < last; )
            _MSTL iter_swap(first++, --last);
    }

    MSTL_CONSTEXPR20 void swap(self& x) noexcept {
        if (_MSTL addressof(x) == this) return;
        _MSTL swap(data_, x.data_);
        _MSTL swap(size_, x.size_);
        alloc_pair_.swap(x.alloc_pair_);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find(const self& str, const size_type n = 0) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, n, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find(const CharT chr, const size_type n = 0) const noexcept {
        return (char_traits_find_char<Traits>)(data_, size_, n, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find(const CharT* const str,
        const size_type off, const size_type count) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, off, str, count);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type rfind(const self& str, const size_type off = npos) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type rfind(const CharT chr, const size_type n = npos) const noexcept {
        return (char_traits_rfind_char<Traits>)(data_, size_, n, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type rfind(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type rfind(const CharT* const str, const size_type off = npos) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_of(const self& str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_of(const CharT chr, const size_type off = 0) const noexcept {
        return (char_traits_find_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_of(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_of(const self& str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_of(const CharT chr, const size_type off = npos) const noexcept {
        return (char_traits_rfind_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_of(const CharT* const str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_not_of(const self& str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_not_of(const CharT chr, const size_type off = 0) const noexcept {
        return (char_traits_find_not_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_not_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_first_not_of(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_not_of(const self& str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_not_of(const CharT chr, const size_type off = npos) const noexcept {
        return (char_traits_rfind_not_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_not_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR20 size_type find_last_not_of(const CharT* const str,
        const size_type off = npos) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_CONSTEXPR20 size_type count(value_type chr, size_type position = 0) const noexcept {
        size_type n = 0;
        for (size_type idx = position; idx < size_; ++idx) {
            if (*(data_ + idx) == chr) ++n;
        }
        return n;
    }

    friend std::istream& operator >>(std::istream& _in, self& _str) {
        auto* buf = new value_type[4096];
        _in >> buf;
        self tmp(buf);
        _str = _MSTL move(tmp);
        delete[] buf;
        return _in;
    }

    MSTL_CONSTEXPR20 bool equal_to(const self& str) const noexcept {
        return (char_traits_equal<Traits>)(data_, size_, str.data_, str.size_);
    }
    MSTL_CONSTEXPR20 bool equal_to(const CharT* str) const noexcept {
        return (char_traits_equal<Traits>)(data_, size_, str, Traits::length(str));
    }

    // starts_with / ends_with
};
#ifdef MSTL_SUPPORT_DEDUCTION_GUIDES__
template <typename Iterator, typename Alloc = standard_allocator<iter_val_t<Iterator>>>
basic_string(Iterator, Iterator, Alloc = Alloc())
-> basic_string<iter_val_t<Iterator>, char_traits<iter_val_t<Iterator>>, Alloc>;

template <typename CharT, typename Traits, typename Alloc = standard_allocator<CharT>>
explicit basic_string(basic_string_view<CharT, Traits>, const Alloc & = Alloc())
-> basic_string<CharT, Traits, Alloc>;

template <typename CharT, typename Traits, typename Alloc = standard_allocator<CharT>>
basic_string(basic_string_view<CharT, Traits>, alloc_size_t<Alloc>, alloc_size_t<Alloc>, const Alloc & = Alloc())
-> basic_string<CharT, Traits, Alloc>;
#endif

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, const basic_string<CharT, Traits, Alloc>& rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(rh);
    return tmp;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    const CharT* lh, const basic_string<CharT, Traits, Alloc>& rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(rh);
    return tmp;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, const CharT* rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(rh);
    return tmp;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    CharT lh, const basic_string<CharT, Traits, Alloc>& rh) {
    basic_string<CharT, Traits, Alloc> tmp(1, lh);
    tmp.append(rh);
    return tmp;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, CharT rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(1, rh);
    return tmp;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, const basic_string<CharT, Traits, Alloc>& rh) {
    return _MSTL move(lh.append(rh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, basic_string<CharT, Traits, Alloc>&& rh) {
    return _MSTL move(rh.append(lh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, basic_string<CharT, Traits, Alloc>&& rh) {
    MSTL_DEBUG_VERIFY__(_MSTL addressof(lh) != _MSTL addressof(rh),
        "cannot concatenate the same moved string to itself.");
    basic_string<CharT, Traits, Alloc> tmp(_MSTL move(lh));
    tmp.append(rh);
    return tmp;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    const CharT* lh, basic_string<CharT, Traits, Alloc>&& rh) {
    return _MSTL move(rh.append(lh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, const CharT* rh) {
    return _MSTL move(lh.append(rh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    CharT lh, basic_string<CharT, Traits, Alloc>&& rh) {
    return _MSTL move(rh.append(1, lh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, CharT rh) {
    return _MSTL move(lh.append(1, rh));
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return lh.equal_to(rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return rh.equal_to(lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator ==(
    const basic_string<CharT, Traits, Alloc>& lh, 
    const CharT* const rh) noexcept {
    return lh.equal_to(rh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator !=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return !(lh == rh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return lh.compare(rh) < 0;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return 0 < rh.compare(lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return lh.compare(rh) < 0;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return rh < lh;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <=(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator <=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return !(lh > rh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(rh < lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >=(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(rh < lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR20 bool operator >=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return !(rh < lh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR20 void swap(basic_string<CharT, Traits, Alloc>& lh,
    basic_string<CharT, Traits, Alloc>& rh) noexcept {
    lh.swap(rh);
}

template <typename CharT, typename Traits, typename Alloc>
void show_data_only(const basic_string<CharT, Traits, Alloc>&, std::ostream&);
template <typename CharT, typename Traits, typename Alloc>
std::ostream& operator <<(std::ostream&, const basic_string<CharT, Traits, Alloc>&);

MSTL_END_NAMESPACE__
#endif // MSTL_BASIC_STRING_H__

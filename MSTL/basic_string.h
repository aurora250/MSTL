#ifndef MSTL_BASIC_STRING_H__
#define MSTL_BASIC_STRING_H__
#include "string_view.hpp"
#include "memory.hpp"
MSTL_BEGIN_NAMESPACE__

template <typename CharT, typename Ref = CharT&, typename Ptr = CharT*>
class basic_string_iterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = CharT;
    using pointer           = Ptr;
    using reference         = Ref;
    using difference_type   = ptrdiff_t;
    using self              = basic_string_iterator<CharT, Ref, Ptr>;

private:
    pointer ptr_ = nullptr;

public:
    MSTL_CONSTEXPR basic_string_iterator() noexcept = default;
    MSTL_CONSTEXPR basic_string_iterator(pointer p) noexcept : ptr_(p) {}
    MSTL_CONSTEXPR basic_string_iterator(const self&) noexcept = default;

    MSTL_NODISCARD MSTL_CONSTEXPR reference operator *()  const noexcept { return *ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR pointer operator ->() const noexcept { return &(operator*()); }

    MSTL_CONSTEXPR self& operator ++() noexcept { ++ptr_;
        return *this;
    }
    MSTL_CONSTEXPR self  operator ++(int) noexcept {
        self tmp = *this;
        ++ptr_;
        return tmp;
    }

    MSTL_CONSTEXPR self& operator --() noexcept {
        --ptr_; 
        return *this;
    }
    MSTL_CONSTEXPR self  operator --(int) noexcept {
        self tmp = *this;
        --ptr_;
        return tmp;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR self operator +(difference_type n) const noexcept { return self(ptr_ + n); }
    friend MSTL_NODISCARD MSTL_CONSTEXPR self operator +(typename difference_type n, const self& it) noexcept {
        return it + n;
    }

    MSTL_CONSTEXPR self& operator +=(difference_type n) noexcept {
        ptr_ += n;
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR self operator -(difference_type n) const noexcept { return self(ptr_ - n); }
    MSTL_NODISCARD MSTL_CONSTEXPR difference_type operator -(const self& rhs) const noexcept { return ptr_ - rhs.ptr_; }

    MSTL_CONSTEXPR self& operator -=(difference_type n) noexcept {
        ptr_ -= n;
        return *this;
    }

    MSTL_NODISCARD MSTL_CONSTEXPR reference operator [](difference_type n) const noexcept { return ptr_[n]; }

    MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(const self& rhs) const noexcept { return ptr_ == rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(const self& rhs) const noexcept { return ptr_ != rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(const self& rhs) const noexcept { return ptr_ < rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(const self& rhs) const noexcept { return ptr_ > rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(const self& rhs) const noexcept { return ptr_ <= rhs.ptr_; }
    MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(const self& rhs) const noexcept { return ptr_ >= rhs.ptr_; }
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
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using traits_type       = Traits;
    using allocator_type    = Alloc;
    using value_type        = CharT;
    using pointer           = CharT*;
    using const_pointer     = const CharT*;
    using reference         = CharT&;
    using const_reference   = const CharT&;
    using iterator          = basic_string_iterator<CharT, CharT&, CharT*>;
    using const_iterator    = basic_string_iterator<CharT, const CharT&, const CharT*>;
    using reverse_iterator  = MSTL::reverse_iterator<iterator>;
    using const_reverse_iterator = MSTL::reverse_iterator<const_iterator>;
    using self              = basic_string<CharT, Traits, Alloc>;

public:
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    pointer data_ = nullptr;
    size_type size_ = 0;
    size_type alloc_size_ = 0;
    MSTL_NO_UNIADS allocator_type alloc_{};

    static constexpr uint32_t STRING_BUFFER_THRESHHOLD = 32u;

    inline MSTL_CONSTEXPR void range_check(const size_type n) const noexcept {
        MSTL_DEBUG_VERIFY__(n < size_, "basic string index out of ranges.");
    }

    MSTL_CONSTEXPR size_type clamp_size(const size_type position, const size_type size) const noexcept {
        return MSTL::min(size, size_ - position);
    }

    MSTL_CONSTEXPR void set_empty() noexcept {
        data_ = nullptr;
        size_ = 0;
        alloc_size_ = 0;
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    MSTL_CONSTEXPR void construct_from_iter(Iterator first, Iterator last) {
        size_type n = MSTL::distance(first, last);
        const size_type init_size = MSTL::max(static_cast<size_type>(STRING_BUFFER_THRESHHOLD), n + 1);
        MSTL_TRY__{
            data_ = alloc_.allocate(init_size);
            size_ = n;
            alloc_size_ = init_size;
        }
        MSTL_CATCH_UNWIND_THROW_M__(set_empty())
        for (; n > 0; --n, ++first) append(*first);
    }

    template <typename Iterator>
        requires(forward_iterator<Iterator>)
    MSTL_CONSTEXPR void construct_from_iter(Iterator first, Iterator last) {
        const size_type n = MSTL::distance(first, last);
        const size_type init_size = MSTL::max(static_cast<size_type>(STRING_BUFFER_THRESHHOLD), n + 1);
        MSTL_TRY__{
            data_ = alloc_.allocate(init_size);
            size_ = n;
            alloc_size_ = init_size;
            MSTL::uninitialized_copy(first, last, data_);
        }
        MSTL_CATCH_UNWIND_THROW_M__(set_empty())
    }

    MSTL_CONSTEXPR void construct_from_ptr(const_pointer str, size_type position, size_type n) {
        const size_type init_size = MSTL::max(static_cast<size_type>(STRING_BUFFER_THRESHHOLD), n + 1);
        data_ = alloc_.allocate(init_size);
        traits_type::copy(data_, str + position, n);
        size_ = n;
        alloc_size_ = init_size;
    }

    MSTL_CONSTEXPR void destroy_buffer() noexcept {
        if (data_ == nullptr) return;
        alloc_.deallocate(data_, alloc_size_);
        set_empty();
    }

    MSTL_CONSTEXPR basic_string& replace_fill(iterator first, size_type n1, size_type n2, value_type chr) {
        if (static_cast<size_type>(end() - first) < n1)
            n1 = cend() - first;
        if (n1 < n2) {
            const size_type diff = n2 - n1;
            MSTL_DEBUG_VERIFY__(size_ + diff < max_size(), "");
            if (size_ > alloc_size_ - diff)
                reallocate(diff);
            pointer raw_ptr = &*first;
            traits_type::move(raw_ptr + n2, raw_ptr + n1, end() - (first + n1));
            traits_type::fill(raw_ptr, chr, n2);
            size_ += diff;
        }
        else {
            pointer raw_ptr = &*first;
            traits_type::move(raw_ptr + n2, raw_ptr + n1, end() - (first + n1));
            traits_type::fill(raw_ptr, chr, n2);
            size_ -= (n1 - n2);
        }
        return *this;
    }

    template <typename Iterator>
        requires(input_iterator<Iterator>&& is_same_v<decltype(&*MSTL::declval<Iterator>()), pointer>)
    MSTL_CONSTEXPR basic_string& replace_copy(iterator first, iterator last, Iterator first2, Iterator last2) {
        size_type len1 = last - first;
        size_type len2 = last2 - first2;
        if (len1 < len2) {
            const size_type diff = len2 - len1;
            MSTL_DEBUG_VERIFY__(size_ + diff < max_size(), "");
            if (size_ > alloc_size_ - diff)
                reallocate(diff);
            pointer raw_ptr = &*first;
            traits_type::move(raw_ptr + len2, raw_ptr + len1, end() - (first + len1));
            traits_type::copy(raw_ptr, &*first2, len2);
            size_ += diff;
        }
        else {
            pointer raw_ptr = &*first;
            traits_type::move(raw_ptr + len2, raw_ptr + len1, end() - (first + len1));
            traits_type::copy(raw_ptr, &*first2, len2);
            size_ -= (len1 - len2);
        }
        return *this;
    }
    MSTL_CONSTEXPR basic_string& replace_copy(iterator first, size_type n1, const_pointer str, size_type n2) {
        return replace_copy(first, first + n1, str, str + n2);
    }

    MSTL_CONSTEXPR void reallocate(size_type n) {
        const size_t new_cap = MSTL::max(alloc_size_ + n, alloc_size_ + (alloc_size_ >> 1));
        pointer new_buffer = alloc_.allocate(new_cap);
        traits_type::move(new_buffer, data_, size_);
        alloc_.deallocate(data_);
        data_ = new_buffer;
        alloc_size_ = new_cap;
    }

    MSTL_CONSTEXPR iterator reallocate_and_fill(iterator position, size_type n, value_type chr) {
        const difference_type diff = (&*position) - data_;
        const size_t old_cap = alloc_size_;
        const size_t new_cap = MSTL::max(old_cap + n, old_cap + (old_cap >> 1));
        pointer new_buffer = alloc_.allocate(new_cap);
        pointer end1 = traits_type::move(new_buffer, data_, diff) + diff;
        pointer end2 = traits_type::fill(end1, chr, n) + n;
        traits_type::move(end2, data_ + diff, size_ - diff);
        alloc_.deallocate(data_, old_cap);
        data_ = new_buffer;
        size_ += n;
        alloc_size_ = new_cap;
        return data_ + diff;
    }
    MSTL_CONSTEXPR iterator reallocate_and_copy(iterator position, const_iterator first, const_iterator last) {
        const difference_type diff = position - data_;
        const size_type old_cap = alloc_size_;
        const size_type n = MSTL::distance(first, last);
        const size_t new_cap = MSTL::max(old_cap + n, old_cap + (old_cap >> 1));
        pointer new_buffer = alloc_.allocate(new_cap);
        pointer end1 = traits_type::move(new_buffer, data_, diff) + diff;
        pointer end2 = MSTL::uninitialized_copy_n(first, n, end1) + n;
        traits_type::move(end2, data_ + diff, size_ - diff);
        alloc_.deallocate(data_, old_cap);
        data_ = new_buffer;
        size_ += n;
        alloc_size_ = new_cap;
        return data_ + diff;
    }

public:
    MSTL_CONSTEXPR basic_string() {
        MSTL_TRY__{
            data_ = alloc_.allocate(static_cast<size_type>(STRING_BUFFER_THRESHHOLD));
            size_ = 0;
            alloc_size_ = 0;
        }
        MSTL_CATCH_UNWIND__{ set_empty(); }
    }

    MSTL_CONSTEXPR basic_string(size_type n, value_type chr) : data_(nullptr), size_(0), alloc_size_(0) {
        const size_type init_size = MSTL::max(static_cast<size_type>(STRING_BUFFER_THRESHHOLD), n + 1);
        data_ = alloc_.allocate(init_size);
        traits_type::fill(data_, chr, n);
        size_ = n;
        alloc_size_ = init_size;
    }
    MSTL_CONSTEXPR self& operator =(value_type chr) {
        if (alloc_size_ < 1) {
            pointer new_buffer = alloc_.allocate(2);
            alloc_.deallocate(data_);
            data_ = new_buffer;
            alloc_size_ = 2;
        }
        *data_ = chr;
        size_ = 1;
        return *this;
    }

    MSTL_CONSTEXPR basic_string(const self& str) : data_(nullptr), size_(0), alloc_size_(0) {
        construct_from_ptr(str.data_, 0, str.size_);
    }
    MSTL_CONSTEXPR self& operator =(const self& str) {
        if (MSTL::addressof(str) == this) return *this;
        construct_from_ptr(str.data_, 0, str.size_);
        return *this;
    }

    MSTL_CONSTEXPR basic_string(self&& str) noexcept
        : data_(MSTL::move(str.data_)), size_(str.size_), alloc_size_(str.alloc_size_) {
        str.set_empty();
    }
    MSTL_CONSTEXPR self& operator =(self&& str) noexcept {
        if (MSTL::addressof(str) == this) return *this;
        destroy_buffer();
        swap(str);
        return *this;
    }

    MSTL_CONSTEXPR basic_string(const self& str, size_type position) : data_(nullptr), size_(0), alloc_size_(0) {
        construct_from_ptr(str.data_, position, str.size_ - position);
    }
    MSTL_CONSTEXPR basic_string(const self& str, size_type position, size_type n)
        : data_(nullptr), size_(0), alloc_size_(0) {
        construct_from_ptr(str.data_, position, n);
    }

    MSTL_CONSTEXPR basic_string(const_pointer str) : data_(nullptr), size_(0), alloc_size_(0) {
        construct_from_ptr(str, 0, traits_type::length(str));
    }
    MSTL_CONSTEXPR basic_string(const_pointer str, size_type n) : data_(nullptr), size_(0), alloc_size_(0) {
        construct_from_ptr(str, 0, n);
    }
    MSTL_CONSTEXPR self& operator =(const_pointer str) {
        const size_type len = traits_type::length(str);
        if (alloc_size_ < len) {
            pointer new_buffer = alloc_.allocate(len + 1);
            alloc_.deallocate(data_);
            data_ = new_buffer;
            alloc_size_ = len + 1;
        }
        traits_type::copy(data_, str, len);
        size_ = len;
        return *this;
    }

    template <typename Iterator>
    MSTL_CONSTEXPR basic_string(Iterator first, Iterator last) : data_(nullptr), size_(0), alloc_size_(0) {
        construct_from_iter(first, last);
    }

    MSTL_CONSTEXPR basic_string(std::initializer_list<value_type> l) : basic_string(l.begin(), l.end()) {}
    MSTL_CONSTEXPR self& operator =(std::initializer_list<value_type> l) {
        clear();
        insert(begin(), l.begin(), l.end());
        return *this;
    }

    ~basic_string() { destroy_buffer(); }

    MSTL_CONSTEXPR iterator begin() noexcept { return data_; }
    MSTL_CONSTEXPR iterator end() noexcept { return data_ + size_; }
    MSTL_CONSTEXPR const_iterator cbegin() noexcept { return data_; }
    MSTL_CONSTEXPR const_iterator cend() noexcept { return data_ + size_; }
    MSTL_CONSTEXPR reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    MSTL_CONSTEXPR reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    MSTL_CONSTEXPR const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    MSTL_CONSTEXPR const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    MSTL_CONSTEXPR size_type size() const noexcept { return size_; }
    MSTL_CONSTEXPR size_type max_size() const noexcept { return npos; }
    MSTL_CONSTEXPR size_type capacity() const noexcept { return alloc_size_; }
    MSTL_CONSTEXPR size_type length()   const noexcept { return size_; }
    MSTL_CONSTEXPR bool empty() const noexcept { return size_ == 0; }

    MSTL_CONSTEXPR allocator_type get_allocator() const noexcept { return allocator_type(); }

    MSTL_CONSTEXPR void reserve(size_type n) {
        MSTL_DEBUG_VERIFY__(n < max_size(), "");
        if (alloc_size_ >= n) return;
        pointer new_buffer = alloc_.allocate(n);
        traits_type::move(new_buffer, data_, size_);
        data_ = new_buffer;
        alloc_size_ = n;
    }

    MSTL_CONSTEXPR reference operator [](const size_type n) {
        if (n == size_) *(data_ + n) = value_type();
        return *(data_ + n);
    }
    MSTL_CONSTEXPR const_reference operator [](const size_type n) const {
        if (n == size_) *(data_ + n) = value_type();
        return *(data_ + n);
    }

    MSTL_CONSTEXPR reference at(const size_type n) {
        range_check(n);
        return (*this)[n];
    }
    MSTL_CONSTEXPR const_reference at(const size_type n) const {
        range_check(n);
        return (*this)[n];
    }

    MSTL_CONSTEXPR reference front() {
        MSTL_DEBUG_VERIFY__(!empty(), "");
        return *data_;
    }
    MSTL_CONSTEXPR const_reference front() const {
        MSTL_DEBUG_VERIFY__(!empty(), "");
        return *data_;
    }
    MSTL_CONSTEXPR reference back() {
        MSTL_DEBUG_VERIFY__(!empty(), "");
        return *((data_ + size_) - 1);
    }
    MSTL_CONSTEXPR const_reference back() const {
        MSTL_DEBUG_VERIFY__(!empty(), "");
        return *((data_ + size_) - 1);
    }

    MSTL_CONSTEXPR const_pointer data() const noexcept { return data_; }
    MSTL_CONSTEXPR const_pointer c_str() const noexcept { return data_; }

    MSTL_CONSTEXPR iterator insert(iterator position, value_type chr) {
        if (size_ == alloc_size_)
            return reallocate_and_fill(position, 1, chr);
        traits_type::move(position + 1, position, end() - position);
        ++size_;
        *position = chr;
        return position;
    }

    MSTL_CONSTEXPR iterator insert(iterator position, size_type n, value_type chr) {
        if (n == 0) return position;
        if (alloc_size_ - size_ < n)
            return reallocate_and_fill(position, n, chr);
        if (position == end()) {
            traits_type::fill(end(), chr, n);
            size_ += n;
            return position;
        }
        traits_type::move(position + n, position, n);
        traits_type::fill(position, chr, n);
        size_ += n;
        return position;
    }

    template <typename Iterator>
    MSTL_CONSTEXPR iterator insert(iterator position, Iterator first, Iterator last) {
        const size_type len = MSTL::distance(first, last);
        if (len == 0) return position;

        if (alloc_size_ - size_ < len)
            return reallocate_and_copy(position, first, last);
        if (position == end()) {
            MSTL::uninitialized_copy(first, last, end());
            size_ += len;
            return position;
        }
        traits_type::move(position + len, position, len);
        MSTL::uninitialized_copy(first, last, position);
        size_ += len;
        return position;
    }

    MSTL_CONSTEXPR void push_back(value_type chr) { append(1, chr); }
    MSTL_CONSTEXPR void pop_back() noexcept {
        MSTL_DEBUG_VERIFY__(!empty(), "");
        --size_;
    }

    MSTL_CONSTEXPR self& append(size_type n, value_type chr) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "");
        if (alloc_size_ - size_ < n)
            reallocate(n);
        traits_type::fill(data_ + size_, chr, n);
        size_ += n;
        return *this;
    }

    MSTL_CONSTEXPR self& append(const self& str, size_type position, size_type n) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "");
        if (n == 0) return *this;
        if (alloc_size_ - size_ < n) reallocate(n);
        traits_type::copy(data_ + size_, str.data_ + position, n);
        size_ += n;
        return *this;
    }
    MSTL_CONSTEXPR self& append(const self& str) { return append(str, 0, str.size_); }
    MSTL_CONSTEXPR self& append(const self& str, size_type position) { return append(str, position, str.size_ - position); }

    MSTL_CONSTEXPR self& append(const_pointer str, size_type n) {
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "");
        if (alloc_size_ - size_ < n) reallocate(n);
        traits_type::copy(data_ + size_, str, n);
        size_ += n;
        return *this;
    }
    MSTL_CONSTEXPR self& append(const_pointer str) { return append(str, traits_type::length(str)); }

    template <typename Iterator>
        requires(input_iterator<Iterator>)
    MSTL_CONSTEXPR self& append(Iterator first, Iterator last) {
        const size_type n = MSTL::distance(first, last);
        MSTL_DEBUG_VERIFY__(size_ + n < max_size(), "");
        if (alloc_size_ - size_ < n)
            reallocate(n);
        MSTL::uninitialized_copy_n(first, n, data_ + size_);
        size_ += n;
        return *this;
    }

    MSTL_CONSTEXPR self& append(std::initializer_list<value_type> l) {
        return append(l.begin(), l.end());
    }

    MSTL_CONSTEXPR self& operator +=(const self& str) { return append(str); }
    MSTL_CONSTEXPR self& operator +=(value_type chr) { return append(1, chr); }
    MSTL_CONSTEXPR self& operator +=(const_pointer str) { return append(str, str + traits_type::length(str)); }
    MSTL_CONSTEXPR self& operator +=(std::initializer_list<value_type> l) { return append(l); }

    MSTL_CONSTEXPR iterator erase(iterator position) {
        MSTL_DEBUG_VERIFY__(position != end(), "");
        traits_type::move(position, position + 1, end() - position - 1);
        --size_;
        return position;
    }
    MSTL_CONSTEXPR iterator erase(iterator first, iterator last) {
        if (first == begin() && last == end()) {
            clear();
            return end();
        }
        const size_type n = end() - last;
        traits_type::move(first, last, n);
        size_ -= (last - first);
        return first;
    }

    MSTL_CONSTEXPR void resize(size_type count, value_type chr) {
        if (count < size_)
            erase(data_ + count, data_ + size_);
        else
            append(count - size_, chr);
    }
    MSTL_CONSTEXPR void resize(size_type count) { resize(count, value_type()); }

    MSTL_CONSTEXPR void clear() noexcept { size_ = 0; }


    MSTL_NODISCARD MSTL_CONSTEXPR self substr(const size_type off = 0, size_type count = npos) const {
        range_check(off);
        count = clamp_size(off, count);
        return self(data_ + off, count);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR int compare(const self& str) const noexcept {
        return (char_traits_compare<Traits>)(data_, size_, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR int compare(const size_type off, const size_type n, const self& str) const {
        return substr(off, n).compare(str);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR int compare(const size_type off, const size_type n, const self& str,
        const size_type roff, const size_type count) const {
        return substr(off, n).compare(str.substr(roff, count));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR int compare(const CharT* const str) const noexcept {
        return compare(self(str));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR int compare(const size_type off, const size_type n, const CharT* const str) const {
        return substr(off, n).compare(self(str));
    }
    MSTL_NODISCARD MSTL_CONSTEXPR int compare(const size_type off, const size_type n,
        const CharT* const str, const size_type count) const {
        return substr(off, n).compare(self(str, count));
    }

    MSTL_CONSTEXPR self& replace(size_type position, size_type n, const self& str) {
        range_check(position);
        return replace_copy(data_ + position, n, str.data_, str.size_);
    }
    MSTL_CONSTEXPR self& replace(iterator first, iterator last, const self& str) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "");
        return replace_copy(first, last - first, str.data_, str.size_);
    }

    MSTL_CONSTEXPR self& replace(size_type position, size_type n, const_pointer str) {
        range_check(position);
        return replace_copy(data_ + position, n, str, traits_type::length(str));
    }
    MSTL_CONSTEXPR self& replace(iterator first, iterator last, const_pointer str) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "");
        return replace_copy(first, last - first, str, traits_type::length(str));
    }

    MSTL_CONSTEXPR self& replace(size_type position, size_type n1, const_pointer str, size_type n2) {
        range_check(position);
        return replace_copy(data_ + position, n1, str, n2);
    }
    MSTL_CONSTEXPR self& replace(iterator first, iterator last, const_pointer str, size_type n) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "");
        return replace_copy(first, last - first, str, n);
    }

    MSTL_CONSTEXPR self& replace(size_type position, size_type n1, size_type n2, value_type chr) {
        range_check(position);
        return replace_fill(data_ + position, n1, n2, chr);
    }
    MSTL_CONSTEXPR self& replace(iterator first, iterator last, size_type n, value_type chr) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "");
        return replace_fill(first, static_cast<size_type>(last - first), n, chr);
    }

    MSTL_CONSTEXPR self& replace(size_type position1, size_type n1, const basic_string& str,
        size_type position2, size_type n2 = npos) {
        range_check(position1);
        range_check(position2);
        return replace_copy(data_ + position1, n1, str.data_ + position2, n2);
    }

    template <typename Iterator>
    MSTL_CONSTEXPR self& replace(iterator first, iterator last, Iterator first2, Iterator last2) {
        MSTL_DEBUG_VERIFY__(begin() <= first && last <= end() && first <= last, "");
        return replace_copy(first, last, first2, last2);
    }

    MSTL_CONSTEXPR void reverse() noexcept {
        if (size() < 2) return;
        for (iterator first = begin(), last = end(); first < last; ) 
            MSTL::iter_swap(first++, --last);
    }

    MSTL_CONSTEXPR void swap(self& x) noexcept {
        if (MSTL::addressof(x) == this) return;
        MSTL::swap(data_, x.data_);
        MSTL::swap(size_, x.size_);
        MSTL::swap(alloc_size_, x.alloc_size_);
    }

    MSTL_NODISCARD MSTL_CONSTEXPR size_type find(const self& str, const size_type n = 0) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, n, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find(const CharT chr, const size_type n = 0) const noexcept {
        return (char_traits_find_char<Traits>)(data_, size_, n, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find(const CharT* const str,
        const size_type off, const size_type count) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, off, str, count);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR size_type rfind(const self& str, const size_type off = npos) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type rfind(const CharT chr, const size_type n = npos) const noexcept {
        return (char_traits_rfind_char<Traits>)(data_, size_, n, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type rfind(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type rfind(const CharT* const str, const size_type off = npos) const noexcept {
        return (char_traits_rfind<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_of(const self& str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_of(const CharT chr, const size_type off = 0) const noexcept {
        return (char_traits_find_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_of(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_of(const self& str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_of(const CharT chr, const size_type off = npos) const noexcept {
        return (char_traits_rfind_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_of(const CharT* const str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_not_of(const self& str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_not_of(const CharT chr, const size_type off = 0) const noexcept {
        return (char_traits_find_not_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_not_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_first_not_of(const CharT* const str, const size_type off = 0) const noexcept {
        return (char_traits_find_first_not_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_not_of(const self& str, const size_type off = npos) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str.data_, str.size_);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_not_of(const CharT chr, const size_type off = npos) const noexcept {
        return (char_traits_rfind_not_char<Traits>)(data_, size_, off, chr);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_not_of(const CharT* const str, const size_type off,
        const size_type n) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str, n);
    }
    MSTL_NODISCARD MSTL_CONSTEXPR size_type find_last_not_of(const CharT* const str,
        const size_type off = npos) const noexcept {
        return (char_traits_find_last_not_of<Traits>)(data_, size_, off, str, Traits::length(str));
    }

    MSTL_CONSTEXPR size_type count(value_type chr, size_type position = 0) const noexcept {
        size_type n = 0;
        for (size_type idx = position; idx < size_; ++idx) {
            if (*(data_ + idx) == chr) ++n;
        }
        return n;
    }

    friend std::istream& operator >>(std::istream& _in, self& _str) {
        value_type* buf = new value_type[4096];
        _in >> buf;
        self tmp(buf);
        _str = MSTL::move(tmp);
        delete[] buf;
        return _in;
    }

    friend std::ostream& operator <<(std::ostream& _out, const self& _str) {
        for (size_type i = 0; i < _str.size_; ++i)
            _out << *(_str.data_ + i);
        return _out;
    }

    MSTL_CONSTEXPR bool equal_to(const self& str) const noexcept {
        return (char_traits_equal<Traits>)(data_, size_, str.data_, str.size_);
    }
    MSTL_CONSTEXPR bool equal_to(const CharT* str) const noexcept {
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
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, const basic_string<CharT, Traits, Alloc>& rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(rh);
    return tmp;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    const CharT* lh, const basic_string<CharT, Traits, Alloc>& rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(rh);
    return tmp;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, const CharT* rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(rh);
    return tmp;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    CharT lh, const basic_string<CharT, Traits, Alloc>& rh) {
    basic_string<CharT, Traits, Alloc> tmp(1, lh);
    tmp.append(rh);
    return tmp;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, CharT rh) {
    basic_string<CharT, Traits, Alloc> tmp(lh);
    tmp.append(1, rh);
    return tmp;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, const basic_string<CharT, Traits, Alloc>& rh) {
    return MSTL::move(lh.append(rh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    const basic_string<CharT, Traits, Alloc>& lh, basic_string<CharT, Traits, Alloc>&& rh) {
    return MSTL::move(rh.append(lh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, basic_string<CharT, Traits, Alloc>&& rh) {
    MSTL_DEBUG_VERIFY__(MSTL::addressof(lh) != MSTL::addressof(rh),
        "cannot concatenate the same moved string to itself.");
    basic_string<CharT, Traits, Alloc> tmp(MSTL::move(lh));
    tmp.append(rh);
    return tmp;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    const CharT* lh, basic_string<CharT, Traits, Alloc>&& rh) {
    return MSTL::move(rh.append(lh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, const CharT* rh) {
    return MSTL::move(lh.append(rh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    CharT lh, basic_string<CharT, Traits, Alloc>&& rh) {
    return MSTL::move(rh.append(1, lh));
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR basic_string<CharT, Traits, Alloc> operator +(
    basic_string<CharT, Traits, Alloc>&& lh, CharT rh) {
    return MSTL::move(lh.append(1, rh));
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return lh.equal_to(rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return rh.equal_to(lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator ==(
    const basic_string<CharT, Traits, Alloc>& lh, 
    const CharT* const rh) noexcept {
    return lh.equal_to(rh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh == rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator !=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return !(lh == rh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return lh.compare(rh) < 0;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return 0 < rh.compare(lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return lh.compare(rh) < 0;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return rh < lh;
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return rh < lh;
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(lh > rh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator <=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return !(lh > rh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(rh < lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const CharT* const lh,
    const basic_string<CharT, Traits, Alloc>& rh) noexcept {
    return !(rh < lh);
}
template <typename CharT, typename Traits, typename Alloc>
MSTL_NODISCARD MSTL_CONSTEXPR bool operator >=(
    const basic_string<CharT, Traits, Alloc>& lh,
    const CharT* const rh) noexcept {
    return !(rh < lh);
}

template <typename CharT, typename Traits, typename Alloc>
MSTL_CONSTEXPR void swap(basic_string<CharT, Traits, Alloc>& lh,
    basic_string<CharT, Traits, Alloc>& rh) noexcept {
    lh.swap(rh);
}

MSTL_END_NAMESPACE__
#endif // MSTL_BASIC_STRING_H__

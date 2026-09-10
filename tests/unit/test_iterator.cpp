#include <NeForce/core/iterator/insert_iterator.hpp>
#include <NeForce/core/iterator/iterator_traits.hpp>
#include <NeForce/core/iterator/move_iterator.hpp>
#include <NeForce/core/iterator/normal_iterator.hpp>
#include <NeForce/core/iterator/reverse_iterator.hpp>
#include <NeForce/core/container/deque.hpp>
#include <NeForce/core/container/vector.hpp>
#include <NeForce/core/memory/shared_ptr.hpp>
#include <NeForce/core/memory/standard_allocator.hpp>
#include <NeForce/core/memory/unique_ptr.hpp>
#include <NeForce/core/string/string.hpp>
#include <NeForce/core/utility/pair.hpp>
#include <gtest/gtest.h>
using namespace neforce;

namespace {
    template <typename T>
    struct counting_iterator {
        using iterator_category = bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        T* ptr = nullptr;

        reference operator*() const { return *ptr; }
        pointer operator->() const { return ptr; }
        counting_iterator& operator++() {
            ++ptr;
            return *this;
        }
        counting_iterator operator++(int) {
            counting_iterator tmp = *this;
            ++ptr;
            return tmp;
        }
        counting_iterator& operator--() {
            --ptr;
            return *this;
        }
        counting_iterator operator--(int) {
            counting_iterator tmp = *this;
            --ptr;
            return tmp;
        }
        counting_iterator& operator+=(difference_type n) {
            ptr += n;
            return *this;
        }
        counting_iterator operator+(difference_type n) const { return counting_iterator{ptr + n}; }
        counting_iterator operator-(difference_type n) const { return counting_iterator{ptr - n}; }
        difference_type operator-(const counting_iterator& other) const { return ptr - other.ptr; }
        reference operator[](difference_type n) const { return ptr[n]; }
        bool operator==(const counting_iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const counting_iterator& other) const { return ptr != other.ptr; }
        bool operator<(const counting_iterator& other) const { return ptr < other.ptr; }
    };

    template <typename Pair>
    struct pair_iterator {
        using iterator_category = forward_iterator_tag;
        using value_type = Pair;
        using difference_type = ptrdiff_t;
        using pointer = Pair*;
        using reference = Pair&;

        Pair* ptr = nullptr;
    };

    struct minimal_allocator {
        using value_type = int;
    };

    struct widening_allocator {
        using value_type = char;
        using pointer = char*;
        using size_type = unsigned long long;
    };
} // namespace


TEST(IteratorTraitsTest, RawPointerCategory) {
    static_assert(is_same_v<iter_category_t<int*>, contiguous_iterator_tag>, "raw pointers are contiguous");
    static_assert(is_base_of_v<random_access_iterator_tag, iter_category_t<int*>>, "contiguous implies random access");
}

TEST(IteratorTraitsTest, RawPointerValueTypeStripsConst) {
    static_assert(is_same_v<iter_value_t<int*>, int>, "non-const pointer value type");
    static_assert(is_same_v<iter_value_t<const int*>, int>, "const is removed from value_type");
    static_assert(is_same_v<iter_value_t<volatile int*>, int>, "cv is removed from value_type");
}

TEST(IteratorTraitsTest, RawPointerReferenceKeepsConst) {
    static_assert(is_same_v<iter_reference_t<int*>, int&>, "non-const reference");
    static_assert(is_same_v<iter_reference_t<const int*>, const int&>, "const reference");
    static_assert(is_same_v<iter_pointer_t<const int*>, const int*>, "pointer keeps const");
}

TEST(IteratorTraitsTest, RawPointerDifferenceType) {
    static_assert(is_same_v<iter_difference_t<int*>, ptrdiff_t>, "pointer difference type");
    static_assert(is_same_v<iter_difference_t<double*>, ptrdiff_t>, "difference type is element independent");
}

TEST(IteratorTraitsTest, CustomIteratorNestedTypesAreExtracted) {
    using traits = iterator_traits<counting_iterator<int>>;
    static_assert(is_same_v<traits::iterator_category, bidirectional_iterator_tag>, "category");
    static_assert(is_same_v<traits::value_type, int>, "value type");
    static_assert(is_same_v<traits::difference_type, ptrdiff_t>, "difference type");
    static_assert(is_same_v<traits::pointer, int*>, "pointer type");
    static_assert(is_same_v<traits::reference, int&>, "reference type");
}

TEST(IteratorTraitsTest, CustomIteratorAliases) {
    static_assert(is_same_v<iter_category_t<counting_iterator<double>>, bidirectional_iterator_tag>, "category alias");
    static_assert(is_same_v<iter_value_t<counting_iterator<double>>, double>, "value alias");
    static_assert(is_same_v<iter_reference_t<counting_iterator<double>>, double&>, "reference alias");
    static_assert(is_same_v<iter_pointer_t<counting_iterator<double>>, double*>, "pointer alias");
    static_assert(is_same_v<iter_difference_t<counting_iterator<double>>, ptrdiff_t>, "difference alias");
}

TEST(IteratorTraitsTest, ConstIteratorTraitsOfContainer) {
    using iterator = vector<int>::iterator;
    using const_iterator = vector<int>::const_iterator;

    static_assert(is_same_v<iter_value_t<iterator>, int>, "mutable value type");
    static_assert(is_same_v<iter_value_t<const_iterator>, int>, "const value type");
    static_assert(is_same_v<iter_reference_t<const_iterator>, const int&>, "const reference");
    static_assert(is_same_v<iter_pointer_t<const_iterator>, const int*>, "const pointer");
}

TEST(IteratorTraitsTest, TraitsAreUsableAtRuntime) {
    int samples[3] = {10, 20, 30};
    counting_iterator<int> it{&samples[0]};
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
    --it;
    EXPECT_EQ(it[2], 30);
    EXPECT_EQ((it + 3) - it, 3);
}

#ifdef NEFORCE_STANDARD_20
TEST(IteratorTraitsTest, MapKeyAndValueExtraction) {
    using iterator = pair_iterator<pair<const int, double>>;
    static_assert(is_same_v<iter_map_key_t<iterator>, int>, "const is stripped from the key");
    static_assert(is_same_v<iter_map_value_t<iterator>, double>, "value type is extracted verbatim");
}

TEST(IteratorTraitsTest, MapKeyExtractionOnMutablePair) {
    using iterator = pair_iterator<pair<int, string>>;
    static_assert(is_same_v<iter_map_key_t<iterator>, int>, "mutable key stays mutable");
    static_assert(is_same_v<iter_map_value_t<iterator>, string>, "string value type");
}
#endif


TEST(PointerTraitsTest, RawPointerMembers) {
    static_assert(is_same_v<pointer_traits<int*>::pointer, int*>, "pointer type");
    static_assert(is_same_v<pointer_traits<int*>::element_type, int>, "element type");
    static_assert(is_same_v<pointer_traits<int*>::difference_type, ptrdiff_t>, "difference type");
    static_assert(is_same_v<pointer_traits<int*>::reference, int&>, "reference type");
}

TEST(PointerTraitsTest, RawPointerToVoidUsesCharReference) {
    static_assert(is_same_v<pointer_traits<void*>::element_type, void>, "void element type");
    static_assert(is_same_v<pointer_traits<void*>::reference, char&>, "void reference degrades to char&");
}

TEST(PointerTraitsTest, RawPointerRebind) {
    static_assert(is_same_v<pointer_traits<int*>::rebind<double>, double*>, "rebind to double");
    static_assert(is_same_v<pointer_traits<char*>::rebind<int>, int*>, "rebind to int");
}

TEST(PointerTraitsTest, PointerRebindAlias) {
    static_assert(is_same_v<pointer_rebind<int*, double>, double*>, "alias exposes the same rebind");
    static_assert(is_same_v<pointer_rebind<const char*, long>, long*>, "alias works for const pointers");
}

TEST(PointerTraitsTest, RawPointerToAddressAndPointerTo) {
    int value = 7;
    int* ptr = &value;

    EXPECT_EQ(pointer_traits<int*>::to_address(ptr), &value);
    EXPECT_EQ(*pointer_traits<int*>::pointer_to(value), 7);
    EXPECT_EQ(pointer_traits<int*>::pointer_to(value), &value);
}

TEST(PointerTraitsTest, SmartPointerElementType) {
    static_assert(is_same_v<pointer_traits<unique_ptr<int>>::element_type, int>, "unique_ptr element type");
    static_assert(is_same_v<pointer_traits<shared_ptr<string>>::element_type, string>, "shared_ptr element type");
    static_assert(is_same_v<pointer_traits<shared_ptr<const int>>::element_type, const int>, "const is preserved");
}

TEST(PointerTraitsTest, SmartPointerToAddress) {
    auto owned = make_unique<int>(99);
    EXPECT_EQ(to_address(owned), owned.get());
    EXPECT_EQ(*to_address(owned), 99);

    auto shared = make_shared<int>(5);
    EXPECT_EQ(to_address(shared), shared.get());
    EXPECT_EQ(*to_address(shared), 5);
}

TEST(PointerTraitsTest, ToAddressOnRawPointer) {
    int value = 3;
    int* ptr = &value;
    EXPECT_EQ(to_address(ptr), &value);
    EXPECT_EQ(to_address(&value), &value);
}

TEST(PointerTraitsTest, PtrConstCastOnRawPointer) {
    int value = 11;
    const int* const_ptr = &value;
    int* mutable_ptr = ptr_const_cast(const_ptr);

    EXPECT_EQ(mutable_ptr, &value);
    *mutable_ptr = 12;
    EXPECT_EQ(value, 12);
}

TEST(PointerTraitsTest, PtrConstCastOnVolatilePointer) {
    int value = 1;
    const volatile int* cv_ptr = &value;
    volatile int* mutable_ptr = ptr_const_cast(cv_ptr);

    EXPECT_EQ(mutable_ptr, &value);
    EXPECT_EQ(*mutable_ptr, 1);
}


TEST(AllocatorTraitsExtractorTest, PointerTypeFallsBackToValueTypePointer) {
    static_assert(is_same_v<get_pointer_type<minimal_allocator>::type, int*>, "value_type* fallback");
}

TEST(AllocatorTraitsExtractorTest, PointerTypePrefersNestedAlias) {
    static_assert(is_same_v<get_pointer_type<standard_allocator<double>>::type, double*>, "nested pointer wins");
    static_assert(is_same_v<get_pointer_type<widening_allocator>::type, char*>, "nested pointer for char");
}

TEST(AllocatorTraitsExtractorTest, DifferenceTypeFromPointerTraits) {
    static_assert(is_same_v<get_difference_type<minimal_allocator>::type, ptrdiff_t>, "fallback via pointer_traits");
}

TEST(AllocatorTraitsExtractorTest, DifferenceTypePrefersNestedAlias) {
    struct with_difference {
        using value_type = int;
        using difference_type = short;
    };
    static_assert(is_same_v<get_difference_type<with_difference>::type, short>, "nested difference_type wins");
}

TEST(AllocatorTraitsExtractorTest, SizeTypePrefersNestedAlias) {
    static_assert(is_same_v<get_size_type<widening_allocator>::type, unsigned long long>, "nested size_type wins");
    static_assert(is_same_v<get_size_type<standard_allocator<int>>::type, standard_allocator<int>::size_type>,
                  "standard_allocator size type");
}

TEST(AllocatorTraitsExtractorTest, SizeTypeFallsBackToUnsignedDifference) {
    static_assert(is_same_v<get_size_type<minimal_allocator>::type, make_unsigned_t<ptrdiff_t>>,
                  "size type is the unsigned difference type");
}


TEST(NormalIteratorTest, DereferenceAndArrow) {
    pair<int, int> items[2] = {{1, 2}, {3, 4}};
    normal_iterator<pair<int, int>*> it(items);

    EXPECT_EQ(it->first, 1);
    EXPECT_EQ((*it).second, 2);
}

TEST(NormalIteratorTest, IncrementAndDecrement) {
    int samples[4] = {1, 2, 3, 4};
    normal_iterator<int*> it(samples);

    EXPECT_EQ(*it, 1);
    EXPECT_EQ(*++it, 2);
    EXPECT_EQ(*it++, 2);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(*--it, 2);
    EXPECT_EQ(*it--, 2);
    EXPECT_EQ(*it, 1);
}

TEST(NormalIteratorTest, RandomAccessArithmetic) {
    int samples[5] = {0, 1, 2, 3, 4};
    normal_iterator<int*> it(samples);

    EXPECT_EQ(*(it + 3), 3);
    EXPECT_EQ(*(it - 0), 0);
    EXPECT_EQ(it[4], 4);

    it += 2;
    EXPECT_EQ(*it, 2);
    it -= 1;
    EXPECT_EQ(*it, 1);
}

TEST(NormalIteratorTest, DifferenceBetweenIterators) {
    int samples[6] = {0, 1, 2, 3, 4, 5};
    normal_iterator<int*> first(samples);
    normal_iterator<int*> last(samples + 6);

    EXPECT_EQ(last - first, 6);
    EXPECT_EQ(first - last, -6);
}

TEST(NormalIteratorTest, ComparisonOperators) {
    int samples[3] = {0, 1, 2};
    normal_iterator<int*> first(samples);
    normal_iterator<int*> second(samples + 1);
    normal_iterator<int*> first_again(samples);

    EXPECT_TRUE(first == first_again);
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
    EXPECT_TRUE(first < second);
    EXPECT_TRUE(second > first);
    EXPECT_TRUE(first <= first_again);
    EXPECT_TRUE(second >= first);
}

TEST(NormalIteratorTest, CrossConstComparison) {
    int samples[3] = {0, 1, 2};
    normal_iterator<int*> mutable_it(samples);
    normal_iterator<const int*> const_it(samples);

    EXPECT_TRUE(mutable_it == const_it);
    EXPECT_FALSE(mutable_it != const_it);
    EXPECT_TRUE(mutable_it <= const_it);
}

TEST(NormalIteratorTest, ConvertingConstructionDropsConst) {
    int samples[2] = {4, 5};
    normal_iterator<const int*> const_it(samples);
    normal_iterator<const int*> moved(const_it);

    EXPECT_EQ(*moved, 4);
    EXPECT_EQ(moved.base(), samples);
}

TEST(NormalIteratorTest, BaseExposesUnderlyingIterator) {
    int samples[2] = {8, 9};
    normal_iterator<int*> it(samples + 1);
    EXPECT_EQ(it.base(), samples + 1);
}

TEST(NormalIteratorTest, TraitsAreInherited) {
    static_assert(is_same_v<iter_value_t<normal_iterator<int*>>, int>, "value type");
    static_assert(is_same_v<iter_reference_t<normal_iterator<int*>>, int&>, "reference type");
    static_assert(is_same_v<iter_difference_t<normal_iterator<int*>>, ptrdiff_t>, "difference type");
    static_assert(is_same_v<iter_category_t<normal_iterator<int*>>, contiguous_iterator_tag>, "category");
}

TEST(NormalIteratorTest, WorksWithContainerIterators) {
    vector<int> vec = {1, 2, 3, 4};
    normal_iterator<vector<int>::iterator> first(vec.begin());
    normal_iterator<vector<int>::iterator> last(vec.end());

    EXPECT_EQ(last - first, 4);
    EXPECT_EQ(*first, 1);
    EXPECT_EQ(first[3], 4);
}


TEST(ReverseIteratorTest, DereferenceStartsAtLastElement) {
    int samples[4] = {1, 2, 3, 4};
    reverse_iterator<int*> it(samples + 4);

    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(ReverseIteratorTest, DecrementMovesTowardTheBackOfTheSequence) {
    int samples[3] = {1, 2, 3};
    reverse_iterator<int*> it(samples + 2);

    ASSERT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(it.base(), samples + 3);
}

TEST(ReverseIteratorTest, SubscriptAndArithmeticTowardsTheFront) {
    int samples[5] = {1, 2, 3, 4, 5};
    reverse_iterator<int*> it(samples + 5);

    EXPECT_EQ(it[0], 5);
    EXPECT_EQ(it[4], 1);
    EXPECT_EQ(*(it + 2), 3);

    it += 3;
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(it.base(), samples + 2);
}

TEST(ReverseIteratorTest, ArithmeticTowardsTheBack) {
    int samples[5] = {1, 2, 3, 4, 5};
    reverse_iterator<int*> it(samples + 3);

    ASSERT_EQ(*it, 3);
    EXPECT_EQ(it[0], 3);
    EXPECT_EQ(it[1], 2);
    EXPECT_EQ(*(it - 1), 4);

    it -= 1;
    EXPECT_EQ(*it, 4);
    EXPECT_EQ(it.base(), samples + 4);
}

TEST(ReverseIteratorTest, BaseReturnsUnderlyingIterator) {
    int samples[3] = {1, 2, 3};
    reverse_iterator<int*> it(samples + 2);

    EXPECT_EQ(it.base(), samples + 2);
}

TEST(ReverseIteratorTest, MakeReverseIteratorHelper) {
    int samples[3] = {1, 2, 3};
    auto it = make_reverse_iterator(samples + 3);

    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST(ReverseIteratorTest, ComparisonOperators) {
    int samples[4] = {1, 2, 3, 4};
    auto first = make_reverse_iterator(samples + 4);
    auto second = make_reverse_iterator(samples + 3);

    EXPECT_TRUE(first == first);
    EXPECT_TRUE(first != second);
    EXPECT_TRUE(first < second);
}

TEST(ReverseIteratorTest, IteratorDifference) {
    int samples[5] = {1, 2, 3, 4, 5};
    auto first = make_reverse_iterator(samples + 5);
    auto last = make_reverse_iterator(samples);

    EXPECT_EQ(last - first, 5);
    EXPECT_EQ(first - last, -5);
}

TEST(ReverseIteratorTest, TraitsMirrorBaseIterator) {
    static_assert(is_same_v<iter_value_t<reverse_iterator<int*>>, int>, "value type");
    static_assert(is_same_v<iter_reference_t<reverse_iterator<int*>>, int&>, "reference type");
    static_assert(is_same_v<iter_pointer_t<reverse_iterator<int*>>, int*>, "pointer type");
}

TEST(ReverseIteratorTest, WorksWithContainerIterators) {
    vector<int> vec = {10, 20, 30};
    auto first = make_reverse_iterator(vec.end());

    EXPECT_EQ(*first, 30);
    ++first;
    EXPECT_EQ(*first, 20);
}


TEST(MoveIteratorTest, DereferenceYieldsRvalueReference) {
    string values[2] = {"alpha", "beta"};
    auto it = make_move_iterator(&values[0]);

    static_assert(is_same_v<iter_reference_t<decltype(it)>, string&&>, "dereference is an rvalue reference");

    string stolen = *it;
    EXPECT_EQ(stolen, "alpha");
}

TEST(MoveIteratorTest, BaseExposesUnderlyingIterator) {
    int samples[2] = {1, 2};
    auto it = make_move_iterator(samples + 1);
    EXPECT_EQ(it.base(), samples + 1);
}

TEST(MoveIteratorTest, IncrementAndArithmetic) {
    int samples[4] = {1, 2, 3, 4};
    auto it = make_move_iterator(samples);

    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    it += 2;
    EXPECT_EQ(*it, 4);
    --it;
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(it[1], 4);
}

TEST(MoveIteratorTest, ComparisonOperators) {
    int samples[3] = {1, 2, 3};
    auto first = make_move_iterator(samples);
    auto second = make_move_iterator(samples + 1);

    EXPECT_TRUE(first == first);
    EXPECT_TRUE(first != second);
    EXPECT_TRUE(first < second);
    EXPECT_EQ(second - first, 1);
}

TEST(MoveIteratorTest, TraitsInheritPointerFromBase) {
    static_assert(is_same_v<iter_value_t<move_iterator<int*>>, int>, "value type");
    static_assert(is_same_v<iter_difference_t<move_iterator<int*>>, ptrdiff_t>, "difference type");
    static_assert(is_same_v<move_iterator<int*>::pointer, int*>, "pointer is the underlying iterator");
}

TEST(MoveIteratorTest, MovesElementsOutOfContainer) {
    vector<string> source;
    source.emplace_back("one");
    source.emplace_back("two");

    vector<string> target;
    for (auto it = make_move_iterator(source.begin()); it != make_move_iterator(source.end()); ++it) {
        target.push_back(*it);
    }

    ASSERT_EQ(target.size(), 2U);
    EXPECT_EQ(target[0], "one");
    EXPECT_EQ(target[1], "two");
}


TEST(BackInsertIteratorTest, AppendsToContainer) {
    vector<int> vec;
    auto inserter = make_back_inserter(vec);

    inserter = 1;
    inserter = 2;
    inserter = 3;

    ASSERT_EQ(vec.size(), 3U);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST(BackInsertIteratorTest, DereferenceAndIncrementAreNoOps) {
    vector<int> vec;
    auto inserter = make_back_inserter(vec);

    *inserter = 5;
    ++inserter;
    inserter++;
    *inserter = 6;

    ASSERT_EQ(vec.size(), 2U);
    EXPECT_EQ(vec[0], 5);
    EXPECT_EQ(vec[1], 6);
}

TEST(BackInsertIteratorTest, MovesValuesIntoContainer) {
    vector<string> vec;
    auto inserter = make_back_inserter(vec);

    string value = "moved";
    inserter = _NEFORCE move(value);

    ASSERT_EQ(vec.size(), 1U);
    EXPECT_EQ(vec[0], "moved");
}

TEST(BackInsertIteratorTest, CategoryIsOutput) {
    static_assert(is_same_v<back_insert_iterator<vector<int>>::iterator_category, output_iterator_tag>,
                  "back inserter is an output iterator");
    static_assert(is_same_v<back_insert_iterator<vector<int>>::value_type, void>, "output iterators have void value");
}

TEST(FrontInsertIteratorTest, PrependsToContainer) {
    deque<int> values = {3};
    auto inserter = make_front_inserter(values);

    inserter = 2;
    inserter = 1;

    ASSERT_EQ(values.size(), 3U);
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 3);
}

TEST(FrontInsertIteratorTest, DereferenceAndIncrementAreNoOps) {
    deque<int> values;
    auto inserter = make_front_inserter(values);

    *inserter = 1;
    ++inserter;
    inserter++;
    *inserter = 2;

    ASSERT_EQ(values.size(), 2U);
    EXPECT_EQ(values[0], 2);
    EXPECT_EQ(values[1], 1);
}

TEST(FrontInsertIteratorTest, CategoryIsOutput) {
    static_assert(is_same_v<front_insert_iterator<deque<int>>::iterator_category, output_iterator_tag>,
                  "front inserter is an output iterator");
}

TEST(InsertIteratorTest, InsertsAtPosition) {
    vector<int> vec = {1, 2, 3};
    auto inserter = make_inserter(vec, vec.begin() + 1);

    inserter = 9;
    inserter = 8;

    ASSERT_EQ(vec.size(), 5U);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 9);
    EXPECT_EQ(vec[2], 8);
    EXPECT_EQ(vec[3], 2);
    EXPECT_EQ(vec[4], 3);
}

TEST(InsertIteratorTest, InsertingAtEndAppends) {
    vector<int> vec = {1, 2};
    auto inserter = make_inserter(vec, vec.end());

    inserter = 3;

    ASSERT_EQ(vec.size(), 3U);
    EXPECT_EQ(vec[2], 3);
}

TEST(InsertIteratorTest, DereferenceAndIncrementAreNoOps) {
    vector<int> vec = {1};
    auto inserter = make_inserter(vec, vec.begin());

    *inserter = 0;
    ++inserter;
    inserter++;
    *inserter = 7;

    ASSERT_EQ(vec.size(), 3U);
    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 7);
    EXPECT_EQ(vec[2], 1);
}

TEST(InsertIteratorTest, CategoryIsOutput) {
    static_assert(is_same_v<insert_iterator<vector<int>>::iterator_category, output_iterator_tag>,
                  "insert iterator is an output iterator");
    static_assert(is_same_v<insert_iterator<vector<int>>::difference_type, ptrdiff_t>, "difference type is ptrdiff_t");
}

TEST(InsertIteratorTest, AllInsertIteratorsShareOutputTraits) {
    static_assert(is_same_v<back_insert_iterator<vector<int>>::pointer, void>, "back inserter pointer is void");
    static_assert(is_same_v<front_insert_iterator<deque<int>>::reference, void>, "front inserter reference is void");
    static_assert(is_same_v<insert_iterator<vector<int>>::value_type, void>, "insert iterator value type is void");
}

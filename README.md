# MSTL

本项目旨在建立一套供C++初学者学习并使用的、阅读性强的、较为健全的除并发库外的STL库，同时提供多种功能性接口。
本项目尽最大可能减少除并发库外的标准库的使用，尝试实现简化版本。
欢迎各位issue，star。鄙人才疏学浅，如有不足，还望斧正。

对初学者的建议学习方式：按照下文的文件介绍顺序阅读和使用，在稍有不懂的地方咨询同学或AI。

## 通过阅读和使用MSTL，你能学到什么？

现代C++的实用特性，包括：

constexpr与if constexpr；

concept与requires；

强化noexcept保证；

使用可变参数模板、递归展开和模板特化等模板元技术实现类型萃取并编写功能性容器；

通过enable_if实现SFINAF(Substitution Failure Is Not An Error)；

通过compressed_pair实现EBCO(Empty Base Class Optimization)；

用static_assert进行静态模板保证；

使用alignof实现内存对位分配；

使用attribute优化代码；

区分decltype、auto与template的类型推导退化规则；

内存分配与就地构造的配合使用；

实现绝大部分标准算法(包括并发算法)与所有常用标准容器，并拓展部分教学用的非实用算法；

标准库并发接口的使用(atomic/conditional_variable/thread/mutex/future/package_task等)。

## 支持环境

WINDOWS LINUX

X64 X86

MSVC GCC CLANG

C++ 14 17 20

## 文件介绍

![文件结构](including_structure.png)

以下按照上述文件结构层级依次介绍。

- [basiclib.hpp](/MSTL/basiclib.h)

使用操作系统平台、托管平台、总线宽度和C++版本的宏实现多编译环境适配。

- [type_traits.hpp](/MSTL/type_traits.hpp)

使用模板元技术，在编译期推断类型信息，并提供基础数字类型的哈希函数和迭代器萃取器。

- [errorlib.h](/MSTL/errorlib.h)

定义错误类型和快速调用宏，本项目的所有错误类型都为本文件内的错误类型。

- [functor.hpp](/MSTL/functor.hpp)

定义仿函数和仿函数配接器(已弃用)。

- [concepts.hpp](/MSTL/concepts.hpp)

定义常用的约束。

- [bitmap.hpp](/MSTL/bitmap.hpp)

定义位图类。(未完成)

- [mathlib.h](/MSTL/mathlib.h)

定义常用的constexpr的数学函数。

- [numeric.hpp](/MSTL/numeric.hpp)

定义数学算法。

- [utility.hpp](/MSTL/utility.hpp)

定义常用的constexpr的工具类型与函数，提供键值对pair及其哈希函数。

- [iterator.hpp](/MSTL/iterator.hpp)

定义迭代器工具函数和迭代器配接器。

- [tuple.hpp](/MSTL/tuple.hpp)

定义constexpr的元组tuple类和其辅助函数，提供tuple的哈希函数。

- [algobase.hpp](/MSTL/algobase.hpp)

定义比较、复制和移动算法。

- [optional.hpp](/MSTL/optional.hpp)

定义自选optional类，可以托管一个类型并设置空值nullopt。

- [memory.hpp](/MSTL/memory.hpp)

定义内存操作函数、临时缓存区类、分配器类和智能指针类。

- [variant.hpp](/MSTL/variant.hpp)

定义变体variant类，可在同一块内存同时托管多个类型。

- [string_view.hpp](/MSTL/string_view.hpp)

定义constexpr的字符串萃取类char_traits、辅助萃取函数与constexpr的字符串视图类basic_string_view。

- [functional.hpp](/MSTL/functional.hpp)

定义托管函数指针和类函数类型的函数类function。

- [list.hpp](/MSTL/list.hpp)

定义双向链表类list。

- [deque.hpp](/MSTL/deque.hpp)

定义双端队列类deque。

- [vector.hpp](/MSTL/vector.hpp)

定义constexpr的向量类vector。项目将在绝大部分场景用vector代替动态数组。

- [algo.hpp](/MSTL/algo.hpp)

定义判断、集合、查找、合并、移动、变换、绑定、排列等算法。

- [basic_string.hpp](/MSTL/basic_string.hpp)

定义constexpr的基础字符串类basic_string。

- [heap.hpp](/MSTL/heap.hpp)

定义普通堆算法heap和莱昂纳多堆算法leonardo_heap。

- [queue.hpp](/MSTL/queue.hpp)

定义双端队列deque的配接器 队列类queue，和基于堆算法heap的优先级队列priority_queue。

- [stack.hpp](/MSTL/stack.hpp)

定义双端队列deque的配接器 栈类stack。

- [hashtable.hpp](MSTL/hashtable.hpp)

定义哈希表hashtable作为无序容器的代理类。

- [rb_tree.h](/MSTL/rb_tree.h)

定义红黑树rb_tree作为有序容器的代理类。

- [string.hpp](/MSTL/string.hpp)

定义多种字符类型的字符串类并提供其哈希函数，提供基本数据类型向字符串类的转换函数。

- [sort.hpp](/MSTL/sort.hpp)

定义冒泡、鸡尾酒、选择、希尔、计数、桶、索引、归并、部分、快速、内省、提姆、猴子等多种常见排序算法。

- [unordered_map.hpp](/MSTL/unordered_map.hpp)

定义无序字典类unordered_map和unordered_multimap。

- [unordered_set.hpp](/MSTL/unordered_set.hpp)

定义无序集合类unordered_set和unordered_multiset。

- [map.hpp](/MSTL/map.hpp)

定义有序字典类map和multimap。

- [set.hpp](/MSTL/set.hpp)

定义有序集合类set和multiset。

- [stringstream.hpp](/MSTL/stringstream.hpp)

定义流式字符串类basic_stringstream。它并不基于标准IO流，而仅仅是一个行为像流的字符串类。

- [algorithm.hpp](/MSTL/algorithm.hpp)

包装基础算法、排序算法与数学算法，定义并发算法，方便使用者引入。

- [trace_memory.hpp](/MSTL/trace_memory.hpp)

定义基于boost的栈追踪分配器trace_allocator。

- [check_type.h](MSTL/check_type.h)

定义类型信息分析类，使类型信息更整洁。

- [detailof.hpp](/MSTL/detailof.hpp)

定义类型信息输出函数，快速获取工整的容器类与工具类的类型信息。

- [thread_pool.h](/MSTL/thread_pool.h)

定义线程池ThreadPool。

## 开源协议

本项目基于 [MIT 开源协议](LICENSE) 。



# MSTL

The purpose of this project is to build a set of STL library (without concurrency libraries but used) which is readable and relatively sound for C++ beginners to learn and use, and provide a variety of functional interfaces.
This project tries to reduce the use of standard libraries as much as possible, and tries to implement a simplified version.
Welcome everyone to issue or star. Contemptible talents, if there is a deficiency, but also hope to correct.

Suggested learning style for beginners: Read and use the documents in the following order, and consult with classmates or AI if you don't understand any points.

## What can you learn by reading and using MSTL?

Practical features of modern C++, include:

Constexpr and if constexpr;

Concept and requires;

Strengthen noexcept guarantee;

Use meta programming technologies such as variable argument templates, recursion expansion and template specializations to implement type traits and edit utility containers;

Through enable_if to implement SFINAF(Substitution Failure Is Not An Error)；

Through compressed_pair to implement EBCO(Empty Base Class Optimization)；

Use static_assert to ensure actions of template arguments;

Use alignof to implement memory align aallocations;

Use attribute to optimize code actions;

Distinguish the decay rules of arguments with decltype, auto and template;

Cooperation of memory allocation and placement new;

Implement the vast majority of standard algorithms (including concurrency algorithms) and every commonly used standard containers. And broaden parts of impractical algorithms for teaching purposes only;

Use of standard concurrency interfaces (including atomic/conditional_variable/thread/mutex/future/package_task).

## Supporting Environments

WINDOWS LINUX

X64 X86

MSVC GCC CLANG

C++ 14 17 20

## File Introduction

![File Structure](including_structure.png)

I`ll introduce files with the level of the file structure as follows.

- [basiclib.hpp](/MSTL/basiclib.h)

Use macros of Operation System, Hosting Platform, Data Bus Width and C++ Versions to implement multi-environment adaption.

- [type_traits.hpp](/MSTL/type_traits.hpp)

Use template-meta technology to assert type informations in compilation period, support hash functions of basic number type and iterator adapters.

- [errorlib.h](/MSTL/errorlib.h)

Define error types and quick-invocation macros. All the error types in MSTL are types in this file.

- [functor.hpp](/MSTL/functor.hpp)

Define functors and functor adapters (Deprecated).

- [concepts.hpp](/MSTL/concepts.hpp)

Define commonly used concepts.

- [bitmap.hpp](/MSTL/bitmap.hpp)

Define bitmap (Unfinished).

- [mathlib.h](/MSTL/mathlib.h)

Define commonly used math functions with constexpr property.

- [numeric.hpp](/MSTL/numeric.hpp)

Define math algorithms.

- [utility.hpp](/MSTL/utility.hpp)

Define commonly used utility types and functions with constexpr property. Support pair type and its hash function.

- [iterator.hpp](/MSTL/iterator.hpp)

Define auxiliary iterator functions and iterator adapters.

- [tuple.hpp](/MSTL/tuple.hpp)

Define tuple type with constexpr property, its auxility functions and its hash function.

- [algobase.hpp](/MSTL/algobase.hpp)

Define comparison, range-copy and range-move algorithms.

- [optional.hpp](/MSTL/optional.hpp)

Define optional type which can host a type and set empty by nullopt.

- [memory.hpp](/MSTL/memory.hpp)

Define memory operation functions, temporary buffer type, allocator types and smart pointer types.

- [variant.hpp](/MSTL/variant.hpp)

Define variant type which can host multiple types at the same time on the same block of memory.

- [string_view.hpp](/MSTL/string_view.hpp)

Define char_traits types with constexpr property, auxility extractive functions and basic_string_view type with constexpr property.

- [functional.hpp](/MSTL/functional.hpp)

Define function type which hosting function pointers and function-like types.

- [list.hpp](/MSTL/list.hpp)

Define list type.

- [deque.hpp](/MSTL/deque.hpp)

Define deque type.

- [vector.hpp](/MSTL/vector.hpp)

Define vector with constexpr property. MSTL will use vector instead of variable array.

- [algo.hpp](/MSTL/algo.hpp)

Define judgement, set, find, merge, remove, transformation, bound algorithms and so on.

- [basic_string.hpp](/MSTL/basic_string.hpp)

Define basic_string type with constexpr property.

- [heap.hpp](/MSTL/heap.hpp)

Define ordinary heap algorithms and leonardo heap algorithms.

- [queue.hpp](/MSTL/queue.hpp)

Define queue type, which is the adapter of deque. Define priority_queue type, which based on ordinary heap algorithms.

- [stack.hpp](/MSTL/stack.hpp)

Define stack type, which is the adapter of deque.

- [hashtable.hpp](MSTL/hashtable.hpp)

Define hashtable type as the proxy type of unordered containers.

- [rb_tree.h](/MSTL/rb_tree.h)

Define rb_tree type as the proxy type of ordered containers.

- [string.hpp](/MSTL/string.hpp)

Define string type with multiple char types and support their hash functions. Support transformation functions from basic types to string types.

- [sort.hpp](/MSTL/sort.hpp)

Define bubble, cocktail, select, shell, count, bucket, index, merge, partial, quick, introspective, tim, monkey sort algorithms.

- [unordered_map.hpp](/MSTL/unordered_map.hpp)

Define unordered_map and unordered_multimap.

- [unordered_set.hpp](/MSTL/unordered_set.hpp)

Define unordered_set and unordered_multiset.

- [map.hpp](/MSTL/map.hpp)

Define map and multimap.

- [set.hpp](/MSTL/set.hpp)

Define set and multiset.

- [stringstream.hpp](/MSTL/stringstream.hpp)

Define basic_stringstream type. Its not based on standard IO stream, but only a string type act like a stream.

- [algorithm.hpp](/MSTL/algorithm.hpp)

Include basic algorithms, sort algorithms and math algorithms. Define concurrency algorithms. Easy for user to include.

- [trace_memory.hpp](/MSTL/trace_memory.hpp)

Define trace_allocator type which based on boost library.

- [check_type.h](MSTL/check_type.h)

Defined type-info analysing type, which makes type informations cleaner.

- [detailof.hpp](/MSTL/detailof.hpp)

Define type-info printing functions, which can quickly get clear container and utility type informations.

- [thread_pool.h](/MSTL/thread_pool.h)

Define ThreadPool type.

## Open-Source License

MSTL is based on [MIT Open-Source License](LICENSE) 。

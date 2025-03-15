cmake_minimum_required(VERSION 3.15)
project(MSTL)

set(CMAKE_CXX_STANDARD 20)

add_executable(${CMAKE_PROJECT_NAME} "main.cpp" "MSTL/algo.hpp" "MSTL/algobase.hpp" "MSTL/algorithm.hpp" "MSTL/basic_string.hpp"
        "MSTL/basiclib.cpp" "MSTL/basiclib.h" "MSTL/bitmap.hpp" "MSTL/check_type.cpp" "MSTL/check_type.h" "MSTL/concepts.hpp" "MSTL/deque.hpp" "MSTL/detailof.hpp"
        "MSTL/errorlib.h" "MSTL/functional.hpp" "MSTL/functor.hpp" "MSTL/hashtable.hpp" "MSTL/heap.hpp" "MSTL/iterator.hpp" "MSTL/list.hpp" "MSTL/map.hpp"
        "MSTL/mathlib.h" "MSTL/mathlib.cpp" "MSTL/memory.hpp" "MSTL/mstlc++.hpp" "MSTL/numeric.hpp" "MSTL/queue.hpp" "MSTL/rb_tree.cpp" "MSTL/rb_tree.h"
        "MSTL/set.hpp" "MSTL/sort.hpp" "MSTL/stack.hpp" "MSTL/string.hpp" "MSTL/string_view.hpp" "MSTL/stringstream.hpp" "MSTL/thread_pool.cpp" "MSTL/thread_pool.h"
        "MSTL/trace_memory.hpp" "MSTL/tuple.hpp" "MSTL/type_traits.hpp" "MSTL/unordered_map.hpp" "MSTL/unordered_set.hpp" "MSTL/utility.hpp"
        "MSTL/variant.hpp" "MSTL/vector.hpp")

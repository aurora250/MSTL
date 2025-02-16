cmake_minimum_required(VERSION 3.15.0)
project(MSTL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${CMAKE_PROJECT_NAME} main.cpp algo.hpp algobase.cpp algobase.h algorithm.hpp array.hpp basemstlc++.hpp
    basiclib.cpp basiclib.h check_type.cpp check_type.h concepts.hpp container.cpp container.h database.h depositary.h
    depositary.cpp deque.hpp errorlib.cpp errorlib.h functional.hpp functor.hpp hashtable.h heap.hpp iterator.hpp jsonlib.h 
    list.hpp map.hpp mathlib.h memory.hpp mstlc++.hpp numeric.hpp object.cpp object.h pair.hpp priority_queue.hpp queue.hpp
    rb_tree.cpp rb_tree.h set.hpp stack.hpp string.cpp string.h type_traits.hpp vector.hpp)

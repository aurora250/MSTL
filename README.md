# MSTL

本项目旨在建立一套供C++初学者学习并初步使用的简易STL库，同时提供多种功能性接口。本项目将尽最大可能减少除并发库外的标准库引入，绝大部分功能均主动实现。
欢迎各位issue，star，fork。如有不足，还望斧正。

建议学习方式：按照下文的文件介绍顺序阅读和使用，在稍有不懂的地方咨询AI。

## 通过学习本项目，你能学到什么？

我们将着重学习C++11到20的特性，包括但不限于：

使用编译器环境宏进行多平台与数据总线宽度适配；
constexpr与if constexpr；
concept与requires的使用；
强化noexcept保证；
使用可变参数模板、递归展开和模板特化等模板元技术实现类型萃取并编写功能性容器；
通过enable_if实现SFINAF；
用static_assert进行静态模板保证；
使用attribute优化代码；
区分decltype、auto与template的类型推导退化规则；
allocator与就地构造的配合使用；
实现绝大部分标准算法的实现（包括并发算法）与所有常用标准容器，并拓展部分教学用的非实用算法；
标准库的多线程接口（atomic/conditional_variable/thread/mutex等）的使用。

## 目前支持环境（持续拓展中）

WINDOWS MSVC C++20

## 逐文件介绍

### 一：环境宏

文件：

- [basiclib.hpp](basiclib.hpp)

使用操作系统平台、托管平台、总线宽度和C++版本的宏来实现多编译环境适配。

### 二：类型萃取

文件：

- [type_traits.hpp](type_traits.hpp)

使用模板元技术，在编译期推断类型信息。

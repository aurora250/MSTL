#ifndef NEXUSFORCE_CORE_EXCEPTION_SOURCE_LOCATION_HPP__
#define NEXUSFORCE_CORE_EXCEPTION_SOURCE_LOCATION_HPP__

/**
 * @file source_location.hpp
 * @brief 源码位置信息工具
 *
 * 此文件提供源码位置信息获取与使用工具。
 */

#include "NeForce/core/typeinfo/types.hpp"
NEFORCE_BEGIN_NAMESPACE__

/**
 * @addtogroup DebugAAssertionsAOptimize 调试、断言与优化
 * @{
 */

/**
 * @struct source_location
 * @brief 源码位置信息
 */
struct source_location {
private:
    const char* file_{nullptr}; ///< 文件名
    const char* func_{nullptr}; ///< 函数名
    uint32_t line_{0};          ///< 行号
    uint32_t column_{0};        ///< 列号

public:
    constexpr source_location() noexcept = default;

    constexpr explicit source_location(const char* file, const char* func, uint32_t line = 0,
                                       uint32_t column = 0) noexcept :
    file_(file),
    func_(func),
    line_(line),
    column_(column) {}

    /**
     * @brief 文件名
     */
    NEFORCE_NODISCARD constexpr const char* file_name() const noexcept { return file_ == nullptr ? "" : file_; }

    /**
     * @brief 函数名
     */
    NEFORCE_NODISCARD constexpr const char* func_name() const noexcept { return func_ == nullptr ? "" : func_; }

    /**
     * @brief 行号
     */
    NEFORCE_NODISCARD constexpr uint32_t line() const noexcept { return line_; }

    /**
     * @brief 列号
     */
    NEFORCE_NODISCARD constexpr uint32_t column() const noexcept { return column_; }

#ifdef NEFORCE_COMPILER_GCC
    /**
     * @brief 当前源码位置
     * @param file 文件名
     * @param func 函数名
     * @param line 行号
     * @param column 列号
     * @return 调用点的源码位置
     * @note GCC 不支持获取列号
     */
    NEFORCE_NODISCARD static NEFORCE_CONSTEVAL20 source_location current(const char* file = __builtin_FILE(),
                                                                         const char* func = __builtin_FUNCTION(),
                                                                         const uint32_t line = __builtin_LINE(),
                                                                         const uint32_t column = 0) noexcept {
        return source_location{file, func, line, column};
    }
#else
    /**
     * @brief 当前源码位置
     * @param file 文件名
     * @param func 函数名
     * @param line 行号
     * @param column 列号
     * @return 调用点的源码位置
     */
    NEFORCE_NODISCARD static NEFORCE_CONSTEVAL20 source_location
    current(const char* file = __builtin_FILE(), const char* func = __builtin_FUNCTION(),
            const uint32_t line = __builtin_LINE(), const uint32_t column = __builtin_COLUMN()) noexcept {
        return source_location{file, func, line, column};
    }
#endif
};

/** @} */ // DebugAAssertionsAOptimize

NEFORCE_END_NAMESPACE__
#endif // NEXUSFORCE_CORE_EXCEPTION_SOURCE_LOCATION_HPP__

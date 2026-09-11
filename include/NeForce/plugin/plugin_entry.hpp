#ifndef NEFORCE_PLUGIN_PLUGIN_ENTRY_HPP__
#define NEFORCE_PLUGIN_PLUGIN_ENTRY_HPP__

/**
 * @file plugin_entry.hpp
 * @brief 插件入口点定义
 *
 * 此文件定义了插件必须实现的入口函数的名称与导出属性。
 * 插件需要导出 create_plugin 和 destroy_plugin 函数，
 * 供插件管理器加载和卸载插件。
 */

#include "NeForce/plugin/iplugin.hpp"

/**
 * @defgroup Plugin 插件
 * @brief 动态加载插件管理
 * @{
 */

/**
 * @def NEFORCE_PLUGIN_CREATE_FUNC
 * @brief 插件创建函数名称
 *
 * 插件必须导出的创建函数名，用于实例化插件对象。
 * 该函数无参数，返回新创建的插件对象指针，指针所有权移交插件管理器。
 */
#define NEFORCE_PLUGIN_CREATE_FUNC "create_plugin"

/**
 * @def NEFORCE_PLUGIN_DESTROY_FUNC
 * @brief 插件销毁函数名称
 *
 * 插件必须导出的销毁函数名，用于释放插件对象。
 * 该函数接收创建函数返回的指针并释放其全部资源。
 */
#define NEFORCE_PLUGIN_DESTROY_FUNC "destroy_plugin"

/**
 * @def NEFORCE_PLUGIN_EXPORT
 * @brief 插件入口点导出属性
 *
 * 同时给出 C 链接与动态库导出属性，插件定义入口函数时必须使用。
 *
 * @note 缺少该属性时符号名会被修饰或对外不可见，插件管理器将无法解析入口点。
 */
#if defined(NEFORCE_PLATFORM_WINDOWS)
#    define NEFORCE_PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#    define NEFORCE_PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

/** @} */ // Plugin

#endif // NEFORCE_PLUGIN_PLUGIN_ENTRY_HPP__

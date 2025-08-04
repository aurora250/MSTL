#ifndef MSTL_FILE_HPP__
#define MSTL_FILE_HPP__
#include "string.hpp"
MSTL_BEGIN_NAMESPACE__

// class file {
// private:
//     HANDLE hFile;          // 文件句柄
//     std::wstring filePath; // 文件路径
//     bool isOpen;           // 文件是否打开
//
//     // 私有函数：获取最后错误信息
//     std::string GetLastErrorMsg() const;
//
// public:
//     // 构造函数
//     file();
//     file(const std::wstring& path);
//
//     // 析构函数
//     ~file();
//
//     // 禁止复制构造和赋值操作
//     file(const file&) = delete;
//     file& operator=(const file&) = delete;
//
//     // 移动构造和移动赋值
//     file(file&& other) noexcept;
//     file& operator=(file&& other) noexcept;
//
//     // 打开文件
//     bool Open(const std::wstring& path, DWORD desiredAccess, DWORD shareMode,
//              DWORD creationDisposition, DWORD flagsAndAttributes);
//
//     // 关闭文件
//     void Close();
//
//     // 写入数据
//     DWORD Write(const void* data, DWORD size);
//
//     // 读取数据
//     DWORD Read(void* buffer, DWORD size);
//
//     // 获取文件大小
//     DWORD GetSize() const;
//
//     // 定位文件指针
//     bool Seek(LONG distanceToMove, DWORD moveMethod);
//
//     // 获取当前文件路径
//     std::wstring GetFilePath() const { return filePath; }
//
//     // 检查文件是否打开
//     bool IsOpen() const { return isOpen; }
//
//     // 静态方法：创建文件并写入内容
//     static bool CreateAndWrite(const std::wstring& path, const std::string& content);
//
//     // 静态方法：读取文件内容
//     static bool ReadAll(const std::wstring& path, std::string& content);
// };

MSTL_END_NAMESPACE__
#endif // MSTL_FILE_HPP__

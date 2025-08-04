#ifndef MSTL_FILE_HPP__
#define MSTL_FILE_HPP__
#include "string.hpp"
MSTL_BEGIN_NAMESPACE__

using file_operate_type = unsigned long;

enum class FILE_OPEN_FLAG : file_operate_type {
    READ = GENERIC_READ,
    WRITE = GENERIC_WRITE,
    READ_WRITE = GENERIC_READ | GENERIC_WRITE,
    CREATE_FORCE = CREATE_ALWAYS,
    CREATE_NO_EXIST = CREATE_NEW,
    OPEN_FORCE = OPEN_ALWAYS,
    OPEN_EXIST = OPEN_EXISTING,
    TRUNCATE_EXIST = TRUNCATE_EXISTING
};

enum class FILE_SHARED_MODE : file_operate_type {
    SHARE_READ = FILE_SHARE_READ,
    SHARE_WRITE = FILE_SHARE_WRITE,
    SHARE_READ_WRITE = FILE_SHARE_READ | FILE_SHARE_WRITE,
    SHARE_DELETE = FILE_SHARE_DELETE,
    SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NO_SHARE = 0
};

enum class FILE_ATTRIBUTE : file_operate_type {
    NORMAL = FILE_ATTRIBUTE_NORMAL,
    READONLY = FILE_ATTRIBUTE_READONLY,
    HIDDEN = FILE_ATTRIBUTE_HIDDEN,
    SYSTEM = FILE_ATTRIBUTE_SYSTEM,
    DIRECTORY = FILE_ATTRIBUTE_DIRECTORY,
    ARCHIVE = FILE_ATTRIBUTE_ARCHIVE,
    DEVICE = FILE_ATTRIBUTE_DEVICE,
    TEMPORARY = FILE_ATTRIBUTE_TEMPORARY,
    REPARSE_POINT = FILE_ATTRIBUTE_REPARSE_POINT,
    COMPRESSED = FILE_ATTRIBUTE_COMPRESSED,
    OFFLINE = FILE_ATTRIBUTE_OFFLINE,
    ENCRYPTED = FILE_ATTRIBUTE_ENCRYPTED,
    VIRTUAL = FILE_ATTRIBUTE_VIRTUAL,
    OTHERS = 0
};

enum class FILE_POINT_ORIGIN : file_operate_type {
    BEGIN = FILE_BEGIN,
    CURRENT = FILE_CURRENT,
    END = FILE_END
};


class file {
public:
    using size_type = DWORD;
    using difference_type = LONG;
    
private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
    string path_{};
    bool opened_ = false;

    static string get_last_error_msg()  {
        const size_type error_code = GetLastError();
        if (error_code == 0) {
            return {};
        }

        LPSTR message_buffer = nullptr;
        const size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&message_buffer), 0, nullptr);

        string message(message_buffer, size);
        LocalFree(message_buffer);

        return message;
    }

public:
    file() = default;
    explicit file(string path) : path_(_MSTL move(path)) {}

    file(const file&) = delete;
    file& operator =(const file&) = delete;

    file(file&& other) noexcept
    : handle_(other.handle_), path_(_MSTL move(other.path_)),
    opened_(other.opened_) {
        other.close();
    }

    file& operator =(file&& other) noexcept {
        if (this == _MSTL addressof(other))
            return *this;

        close();
        handle_ = other.handle_;
        path_ = _MSTL move(other.path_);
        opened_ = other.opened_;

        other.handle_ = INVALID_HANDLE_VALUE;
        other.opened_ = false;
        other.path_.clear();

        return *this;
    }

    ~file() {
        close();
    }

    bool open(const string& path,
        FILE_OPEN_FLAG access = FILE_OPEN_FLAG::READ_WRITE,
        FILE_SHARED_MODE share_mode = FILE_SHARED_MODE::SHARE_READ,
        FILE_OPEN_FLAG disposition = FILE_OPEN_FLAG::OPEN_EXIST,
        FILE_ATTRIBUTE attributes = FILE_ATTRIBUTE::NORMAL) {
        close();

        handle_ = CreateFileA(path.c_str(),
            static_cast<file_operate_type>(access),
            static_cast<file_operate_type>(share_mode),
            nullptr, static_cast<file_operate_type>(disposition),
            static_cast<file_operate_type>(attributes), nullptr);
        Exception(handle_ != INVALID_HANDLE_VALUE,
            FileOperateError(get_last_error_msg().c_str()));

        path_ = path;
        opened_ = true;
        return true;
    }

    void close() {
        if (opened_ && handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
            opened_ = false;
        }
    }

    size_type write(const string& data, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        size_type bytes_written;
        const BOOL success = WriteFile(handle_, data.c_str(), size, &bytes_written, nullptr);
        Exception(success, FileOperateError(get_last_error_msg().c_str()));

        return bytes_written;
    }

    size_type read(string& str, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        size_type bytes_read;
        str.resize(size);
        const BOOL success = ReadFile(handle_, str.data(), size, &bytes_read, nullptr);
        Exception(!(!success || GetLastError() == ERROR_HANDLE_EOF),
            FileOperateError(get_last_error_msg().c_str()));

        return bytes_read;
    }

    size_type size() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        const size_type size = GetFileSize(handle_, nullptr);
        Exception(size != INVALID_FILE_SIZE,
            FileOperateError(get_last_error_msg().c_str()));

        return size;
    }

    bool seek(const difference_type distance,
        FILE_POINT_ORIGIN method = FILE_POINT_ORIGIN::BEGIN) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        const size_type offset = SetFilePointer(handle_, distance,
            nullptr, static_cast<file_operate_type>(method));
        return offset != INVALID_SET_FILE_POINTER || GetLastError() == NO_ERROR;
    }

    const string& file_path() const { return path_; }

    bool opened() const { return opened_; }


    static bool directory_exists(const string& path) {
        size_type attrib = GetFileAttributesA(path.c_str());
        return attrib != INVALID_FILE_ATTRIBUTES &&
               (attrib & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool create_directories(const string& path) {
        if (path.empty()) return false;
        if (directory_exists(path)) return true;

        size_t pos = 0;
        while ((pos = path.find_first_of("/\\", pos + 1)) != string::npos) {
            string subdir = path.substr(0, pos);
            if (!directory_exists(subdir)) {
                if (!CreateDirectoryA(subdir.c_str(), nullptr)) {
                    return false;
                }
            }
        }
        return CreateDirectoryA(path.c_str(), nullptr);
    }

    static bool create_and_write(const string& path, const string& content) {
        size_t last_slash = path.find_last_of("/\\");
        if (last_slash != string::npos) {
            string dir = path.substr(0, last_slash);
            if (!dir.empty() && !directory_exists(dir)) {
                if (!create_directories(dir)) {
                    return false;
                }
            }
        }
        remove(path);

        file file;
        try {
            file.open(path, FILE_OPEN_FLAG::WRITE, FILE_SHARED_MODE::NO_SHARE,
                FILE_OPEN_FLAG::CREATE_FORCE, FILE_ATTRIBUTE::NORMAL);
            size_type bytes_written = file.write(content.c_str(), content.size());
            return bytes_written == content.size();
        }
        catch (...) {
            return false;
        }
    }

    static bool remove(const string& path) {
        if (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return DeleteFileA(path.c_str());
        }
        return false;
    }

    static bool read_all(const string& path, string& content) {
        file file;
        try {
            file.open(path, FILE_OPEN_FLAG::READ, FILE_SHARED_MODE::SHARE_READ,
                FILE_OPEN_FLAG::OPEN_EXIST, FILE_ATTRIBUTE::NORMAL);

            size_type size = file.size();
            if (size == 0) {
                content.clear();
                return true;
            }

            content.resize(size);
            const size_type bytes_read = file.read(content, size);
            return bytes_read == size;
        }
        catch (...) {
            return false;
        }
    }
};


MSTL_END_NAMESPACE__
#endif // MSTL_FILE_HPP__

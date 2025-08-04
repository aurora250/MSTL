#ifndef MSTL_FILE_HPP__
#define MSTL_FILE_HPP__
#include "vector.hpp"
#include "datetime.hpp"
MSTL_BEGIN_NAMESPACE__

using file_operate_type = unsigned long;


enum class FILE_ACCESS_MODE : file_operate_type {
    READ = GENERIC_READ,
    WRITE = GENERIC_WRITE,
    READ_WRITE = GENERIC_READ | GENERIC_WRITE,
    APPEND = FILE_APPEND_DATA
};

enum class FILE_SHARED_MODE : file_operate_type {
    SHARE_READ = FILE_SHARE_READ,
    SHARE_WRITE = FILE_SHARE_WRITE,
    SHARE_READ_WRITE = FILE_SHARE_READ | FILE_SHARE_WRITE,
    SHARE_DELETE = FILE_SHARE_DELETE,
    SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NO_SHARE = 0
};

inline FILE_SHARED_MODE operator |(FILE_SHARED_MODE a, FILE_SHARED_MODE b) {
    return static_cast<FILE_SHARED_MODE>(static_cast<int>(a) | static_cast<int>(b));
}

inline FILE_SHARED_MODE operator &(FILE_SHARED_MODE a, FILE_SHARED_MODE b) {
    return static_cast<FILE_SHARED_MODE>(static_cast<int>(a) & static_cast<int>(b));
}


enum class FILE_CREATION_MODE : file_operate_type {
    CREATE_FORCE = CREATE_ALWAYS,
    CREATE_NO_EXIST = CREATE_NEW,
    OPEN_FORCE = OPEN_ALWAYS,
    OPEN_EXIST = OPEN_EXISTING,
    TRUNCATE_EXIST = TRUNCATE_EXISTING
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


inline FILE_ATTRIBUTE operator |(FILE_ATTRIBUTE a, FILE_ATTRIBUTE b) {
    return static_cast<FILE_ATTRIBUTE>(
        static_cast<file_operate_type>(a) | static_cast<file_operate_type>(b));
}

inline FILE_ATTRIBUTE operator &(FILE_ATTRIBUTE a, FILE_ATTRIBUTE b) {
    return static_cast<FILE_ATTRIBUTE>(
        static_cast<file_operate_type>(a) & static_cast<file_operate_type>(b));
}


enum class FILE_POINT_ORIGIN : file_operate_type {
    BEGIN = FILE_BEGIN,
    CURRENT = FILE_CURRENT,
    END = FILE_END
};

enum class FILE_LOCK_MODE : file_operate_type {
    SHARED = 0,
    EXCLUSIVE = LOCKFILE_EXCLUSIVE_LOCK,
    FAIL_IMMEDIATELY = LOCKFILE_FAIL_IMMEDIATELY
};


class file {
public:
    using size_type = DWORD;
    using difference_type = LONG;
    using lock_handle = HANDLE;

private:
    lock_handle handle_ = INVALID_HANDLE_VALUE;
    string path_{};
    bool opened_ = false;
    bool append_mode_ = false;

private:
    static datetime filetime_to_datetime(const FILETIME& ft) {
        if (ft.dwHighDateTime == 0 && ft.dwLowDateTime == 0) {
            return datetime::epoch();
        }
        SYSTEMTIME st_utc;
        if (!FileTimeToSystemTime(&ft, &st_utc)) {
            return datetime::epoch();
        }
        SYSTEMTIME st_local;
        if (!SystemTimeToTzSpecificLocalTime(nullptr, &st_utc, &st_local)) {
            return datetime::epoch();
        }
        return datetime(
            st_local.wYear, st_local.wMonth, st_local.wDay,
            st_local.wHour, st_local.wMinute, st_local.wSecond
        );
    }

    static FILETIME datetime_to_filetime(const datetime& dt) {
        FILETIME ft = {0, 0};
        SYSTEMTIME st_local;
        st_local.wYear = static_cast<WORD>(dt.get_year());
        st_local.wMonth = static_cast<WORD>(dt.get_month());
        st_local.wDay = static_cast<WORD>(dt.get_day());
        st_local.wHour = static_cast<WORD>(dt.get_hours());
        st_local.wMinute = static_cast<WORD>(dt.get_minutes());
        st_local.wSecond = static_cast<WORD>(dt.get_seconds());
        st_local.wMilliseconds = 0;

        SYSTEMTIME st_utc;
        if (!TzSpecificLocalTimeToSystemTime(nullptr, &st_local, &st_utc)) {
            return ft;
        }
        if (!SystemTimeToFileTime(&st_utc, &ft)) {
            ft.dwHighDateTime = 0;
            ft.dwLowDateTime = 0;
        }
        return ft;
    }

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
    opened_(other.opened_), append_mode_(other.append_mode_) {
        other.handle_ = INVALID_HANDLE_VALUE;
        other.opened_ = false;
        other.append_mode_ = false;
    }

    file& operator =(file&& other) noexcept {
        if (this == _MSTL addressof(other))
            return *this;

        close();
        handle_ = other.handle_;
        path_ = _MSTL move(other.path_);
        opened_ = other.opened_;
        append_mode_ = other.append_mode_;

        other.handle_ = INVALID_HANDLE_VALUE;
        other.opened_ = false;
        other.append_mode_ = false;
        other.path_.clear();

        return *this;
    }

    ~file() {
        close();
    }

    bool open(const string& path,
        FILE_ACCESS_MODE access = FILE_ACCESS_MODE::READ_WRITE,
        FILE_SHARED_MODE share_mode = FILE_SHARED_MODE::SHARE_READ,
        FILE_CREATION_MODE creation = FILE_CREATION_MODE::OPEN_EXIST,
        FILE_ATTRIBUTE attributes = FILE_ATTRIBUTE::NORMAL,
        const bool append = false) {
        close();

        handle_ = CreateFileA(path.c_str(),
            static_cast<file_operate_type>(access),
            static_cast<file_operate_type>(share_mode),
            nullptr, static_cast<file_operate_type>(creation),
            static_cast<file_operate_type>(attributes), nullptr);

        if (handle_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        path_ = path;
        opened_ = true;
        append_mode_ = append;

        if (append) {
            if (!seek(0)) return false;
        }
        return true;
    }

    void close() {
        if (opened_ && handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
            handle_ = INVALID_HANDLE_VALUE;
            opened_ = false;
            append_mode_ = false;
        }
    }

    bool flush() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;
        return FlushFileBuffers(handle_) != 0;
    }

    size_type write(const string& data, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        if (append_mode_) {
            SetFilePointer(handle_, 0, nullptr, FILE_END);
        }

        size_type bytes_written;
        const BOOL success = WriteFile(handle_, data.c_str(), size, &bytes_written, nullptr);
        if (!success) {
            return 0;
        }
        return bytes_written;
    }

    size_type read(string& str, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        size_type bytes_read;
        str.resize(size);
        const BOOL success = ReadFile(handle_, str.data(), size, &bytes_read, nullptr);
        if (!success) {
            return 0;
        }
        str.resize(bytes_read);
        return bytes_read;
    }

    bool read_line(string& line) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        line.clear();
        char ch;
        size_type bytes_read;

        while (ReadFile(handle_, &ch, 1, &bytes_read, nullptr) && bytes_read == 1) {
            if (ch == '\n') break;
            if (ch != '\r') line += ch;
        }
        return !line.empty() || bytes_read > 0;
    }

    vector<string> read_all_lines() const {
        vector<string> lines;
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return lines;

        string content;
        if (read_all(path_, content)) {
            size_t start = 0;
            size_t end = content.find('\n');

            while (end != string::npos) {
                string line = content.substr(start, end - start);
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                lines.push_back(line);
                start = end + 1;
                end = content.find('\n', start);
            }

            if (start < content.size()) {
                lines.push_back(content.substr(start));
            }
        }
        return lines;
    }

    size_type size() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(handle_, &file_size)) {
            return 0;
        }
        return static_cast<size_type>(file_size.QuadPart);
    }

    bool seek(const difference_type distance,
        FILE_POINT_ORIGIN method = FILE_POINT_ORIGIN::END) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        LARGE_INTEGER li;
        li.QuadPart = distance;
        return SetFilePointerEx(handle_, li,
            nullptr, static_cast<file_operate_type>(method)) != 0;
    }

    size_type tell() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return 0;

        constexpr LARGE_INTEGER li = {};
        LARGE_INTEGER new_pos;
        if (!SetFilePointerEx(handle_, li, &new_pos, FILE_CURRENT)) {
            return 0;
        }
        return static_cast<size_type>(new_pos.QuadPart);
    }

    bool truncate(const difference_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        if (!seek(size, FILE_POINT_ORIGIN::BEGIN)) {
            return false;
        }
        return SetEndOfFile(handle_) != 0;
    }

    bool lock(const size_type offset, const size_type length,
        FILE_LOCK_MODE mode = FILE_LOCK_MODE::EXCLUSIVE) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        OVERLAPPED ov = {};
        const uint64_t offset_64 = offset;
        ov.Offset = static_cast<size_type>(offset_64 & 0xFFFFFFFF);
        ov.OffsetHigh = static_cast<size_type>(offset_64 >> 32);

        const uint64_t length_64 = length;
        return LockFileEx(handle_, static_cast<file_operate_type>(mode), 0,
            length_64 & 0xFFFFFFFF, length_64 >> 32, &ov) != 0;
    }

    bool unlock(const size_type offset, const size_type length) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        OVERLAPPED ov = {};
        const uint64_t offset_64 = offset;
        ov.Offset = static_cast<size_type>(offset_64 & 0xFFFFFFFF);
        ov.OffsetHigh = static_cast<size_type>(offset_64 >> 32);

        const uint64_t length_64 = length;
        return UnlockFileEx(handle_, 0, length_64 & 0xFFFFFFFF, length_64 >> 32, &ov) != 0;
    }

    FILE_ATTRIBUTE attributes() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return FILE_ATTRIBUTE::OTHERS;

        BY_HANDLE_FILE_INFORMATION info;
        if (!GetFileInformationByHandle(handle_, &info))
            return FILE_ATTRIBUTE::OTHERS;

        return static_cast<FILE_ATTRIBUTE>(info.dwFileAttributes);
    }

    bool set_attributes(FILE_ATTRIBUTE attr) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        return SetFileAttributesA(
            path_.c_str(), static_cast<file_operate_type>(attr)) != 0;
    }

    datetime creation_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return datetime::epoch();

        FILETIME ft_create, ft_access, ft_write;
        if (!GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_create);
    }

    datetime last_access_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return datetime::epoch();

        FILETIME ft_create, ft_access, ft_write;
        if (!GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_access);
    }

    datetime last_write_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return datetime::epoch();

        FILETIME ft_create, ft_access, ft_write;
        if (!GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_write);
    }

    bool set_all_times(const datetime& create,
        const datetime& access, const datetime& write) const {
        if (!opened_ || handle_ == INVALID_HANDLE_VALUE)
            return false;

        const FILETIME ft_create = datetime_to_filetime(create);
        const FILETIME ft_access = datetime_to_filetime(access);
        const FILETIME ft_write = datetime_to_filetime(write);
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }

    bool set_creation_time(const datetime& dt) const {
        const FILETIME ft_create = datetime_to_filetime(dt);
        FILETIME ft_access, ft_write;
        if (!GetFileTime(handle_, nullptr, &ft_access, &ft_write)) {
            return false;
        }
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }

    bool set_last_access_time(const datetime& dt) const {
        const FILETIME ft_access = datetime_to_filetime(dt);
        FILETIME ft_create, ft_write;
        if (!GetFileTime(handle_, &ft_create, nullptr, &ft_write)) {
            return false;
        }
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }

    bool set_last_write_time(const datetime& dt) const {
        const FILETIME ft_write = datetime_to_filetime(dt);
        FILETIME ft_create, ft_access;
        if (!GetFileTime(handle_, &ft_create, &ft_access, nullptr)) {
            return false;
        }
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }


    const string& file_path() const { return path_; }
    bool opened() const { return opened_; }
    bool is_append_mode() const { return append_mode_; }


    static bool exists(const string& path) {
        return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
    }

    static bool is_directory(const string& path) {
        const size_type attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES) &&
               (attrib & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool is_file(const string& path) {
        const size_type attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES) &&
               !(attrib & FILE_ATTRIBUTE_DIRECTORY);
    }

    static bool create_directories(const string& path) {
        if (path.empty()) return false;
        if (is_directory(path)) return true;

        size_t pos = 0;
        string subdir;
        while ((pos = path.find_first_of("/\\", pos + 1)) != string::npos) {
            subdir = path.substr(0, pos);
            if (!subdir.empty() && !is_directory(subdir)) {
                if (!CreateDirectoryA(subdir.c_str(), nullptr)) {
                    if (GetLastError() != ERROR_ALREADY_EXISTS) {
                        return false;
                    }
                }
            }
        }
        return CreateDirectoryA(path.c_str(), nullptr) ||
               GetLastError() == ERROR_ALREADY_EXISTS;
    }

    static bool create_and_write(const string& path, const string& content,
        const bool append = false) {
        const size_t last_slash = path.find_last_of("/\\");
        if (last_slash != string::npos) {
            const string dir = path.substr(0, last_slash);
            if (!dir.empty() && !is_directory(dir)) {
                if (!create_directories(dir)) {
                    return false;
                }
            }
        }

        file file;
        if (!file.open(path,
            append ? FILE_ACCESS_MODE::APPEND : FILE_ACCESS_MODE::WRITE,
            FILE_SHARED_MODE::NO_SHARE, FILE_CREATION_MODE::OPEN_FORCE,
            FILE_ATTRIBUTE::NORMAL, append)) {
            return false;
        }

        const size_type bytes_written = file.write(content.c_str(), content.size());
        return bytes_written == content.size();
    }

    static bool remove(const string& path) {
        if (is_file(path)) {
            return DeleteFileA(path.c_str()) != 0;
        }
        return false;
    }

    static bool remove_directory(const string& path) {
        if (is_directory(path)) {
            return RemoveDirectoryA(path.c_str()) != 0;
        }
        return false;
    }

    static bool read_all(const string& path, string& content) {
        file file;
        if (!file.open(path, FILE_ACCESS_MODE::READ, FILE_SHARED_MODE::SHARE_READ,
            FILE_CREATION_MODE::OPEN_EXIST, FILE_ATTRIBUTE::NORMAL)) {
            return false;
        }

        const size_type size = file.size();
        if (size == 0) {
            content.clear();
            return true;
        }

        content.resize(size);
        const size_type bytes_read = file.read(content, size);
        return bytes_read == size;
    }

    static bool copy(const string& from, const string& to, const bool overwrite = true) {
        return CopyFileA(from.c_str(), to.c_str(), !overwrite) != 0;
    }

    static bool move(const string& from, const string& to, const bool overwrite = true) {
        size_type flags = MOVEFILE_COPY_ALLOWED;
        if (overwrite) {
            flags |= MOVEFILE_REPLACE_EXISTING;
        }
        return MoveFileExA(from.c_str(), to.c_str(), flags) != 0;
    }

    static bool rename(const string& old_name, const string& new_name) {
        return move(old_name, new_name, true);
    }

    static size_type file_size(const string& path) {
        file f;
        if (f.open(path, FILE_ACCESS_MODE::READ, FILE_SHARED_MODE::SHARE_READ,
                   FILE_CREATION_MODE::OPEN_EXIST)) {
            return f.size();
        }
        return 0;
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_FILE_HPP__
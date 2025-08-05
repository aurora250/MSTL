#ifndef MSTL_FILE_HPP__
#define MSTL_FILE_HPP__
#include "vector.hpp"
#include "datetime.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/time.h>
#endif
MSTL_BEGIN_NAMESPACE__

using file_flag_type =
#ifdef MSTL_PLATFORM_WINDOWS__
    unsigned long;
#elif defined(MSTL_PLATFORM_LINUX__)
    int;
#endif


enum class FILE_ACCESS_MODE : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
    READ = GENERIC_READ,
    WRITE = GENERIC_WRITE,
    READ_WRITE = GENERIC_READ | GENERIC_WRITE,
    APPEND = FILE_APPEND_DATA
#elif defined(MSTL_PLATFORM_LINUX__)
    READ = O_RDONLY,
    WRITE = O_WRONLY,
    READ_WRITE = O_RDWR,
    APPEND = O_WRONLY | O_APPEND
#endif
};

enum class FILE_SHARED_MODE : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
    SHARE_READ = FILE_SHARE_READ,
    SHARE_WRITE = FILE_SHARE_WRITE,
    SHARE_READ_WRITE = FILE_SHARE_READ | FILE_SHARE_WRITE,
    SHARE_DELETE = FILE_SHARE_DELETE,
    SHARE_ALL = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NO_SHARE = 0
#elif defined(MSTL_PLATFORM_LINUX__)
    SHARE_READ = 1,
    SHARE_WRITE = 2,
    SHARE_READ_WRITE = 3,
    SHARE_DELETE = 4,
    SHARE_ALL = 7,
    NO_SHARE = 0
#endif
};

inline FILE_SHARED_MODE operator |(FILE_SHARED_MODE a, FILE_SHARED_MODE b) {
    return static_cast<FILE_SHARED_MODE>(static_cast<int>(a) | static_cast<int>(b));
}

inline FILE_SHARED_MODE operator &(FILE_SHARED_MODE a, FILE_SHARED_MODE b) {
    return static_cast<FILE_SHARED_MODE>(static_cast<int>(a) & static_cast<int>(b));
}


enum class FILE_CREATION_MODE : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
    CREATE_FORCE = CREATE_ALWAYS,
    CREATE_NO_EXIST = CREATE_NEW,
    OPEN_FORCE = OPEN_ALWAYS,
    OPEN_EXIST = OPEN_EXISTING,
    TRUNCATE_EXIST = TRUNCATE_EXISTING
#elif defined(MSTL_PLATFORM_LINUX__)
    CREATE_FORCE = O_CREAT | O_TRUNC,
    CREATE_NO_EXIST = O_CREAT | O_EXCL,
    OPEN_FORCE = O_CREAT,
    OPEN_EXIST = 0,
    TRUNCATE_EXIST = O_TRUNC
#endif
};

enum class FILE_ATTRIBUTE : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
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
#elif defined(MSTL_PLATFORM_LINUX__)
    NORMAL = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
    READONLY = S_IRUSR | S_IRGRP | S_IROTH,
    DIRECTORY = S_IFDIR,
    DEVICE = S_IFBLK | S_IFCHR,
    REPARSE_POINT = S_IFLNK,
    OTHERS = 0
#endif
};


inline FILE_ATTRIBUTE operator |(FILE_ATTRIBUTE a, FILE_ATTRIBUTE b) {
    return static_cast<FILE_ATTRIBUTE>(
        static_cast<file_flag_type>(a) | static_cast<file_flag_type>(b));
}

inline FILE_ATTRIBUTE operator &(FILE_ATTRIBUTE a, FILE_ATTRIBUTE b) {
    return static_cast<FILE_ATTRIBUTE>(
        static_cast<file_flag_type>(a) & static_cast<file_flag_type>(b));
}


enum class FILE_POINT_ORIGIN : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
    BEGIN = FILE_BEGIN,
    CURRENT = FILE_CURRENT,
    END = FILE_END
#elif defined(MSTL_PLATFORM_LINUX__)
    BEGIN = SEEK_SET,
    CURRENT = SEEK_CUR,
    END = SEEK_END
#endif
};

enum class FILE_LOCK_MODE : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
    SHARED = 0,
    EXCLUSIVE = LOCKFILE_EXCLUSIVE_LOCK,
    FAIL_IMMEDIATELY = LOCKFILE_FAIL_IMMEDIATELY
#elif defined(MSTL_PLATFORM_LINUX__)
    SHARED = LOCK_SH,
    EXCLUSIVE = LOCK_EX,
    FAIL_IMMEDIATELY = LOCK_NB
#endif
};


class file {
public:
#ifdef MSTL_PLATFORM_WINDOWS__
    using size_type = DWORD;
    using difference_type = LONG;
    using lock_handle = HANDLE;
    using time_type = FILETIME;
#elif defined(MSTL_PLATFORM_LINUX__)
    using size_type = size_t;
    using difference_type = ::off_t;
    using lock_handle = int;
    using time_type = std::time_t;
#endif

private:
    static lock_handle INVALID_HANDLE() {
        static lock_handle INVALID_HANDLE =
#ifdef MSTL_PLATFORM_WINDOWS__
            INVALID_HANDLE_VALUE;
#elif defined(MSTL_PLATFORM_LINUX__)
            -1;
#endif
        return INVALID_HANDLE;
    }

    lock_handle handle_ = INVALID_HANDLE();
    string path_{};
    bool opened_ = false;
    bool append_mode_ = false;

    static constexpr auto FILE_SPLITER =
#ifdef MSTL_PLATFORM_WINDOWS__
        "/\\";
#elif defined(MSTL_PLATFORM_LINUX__)
        "/";
#endif


    static datetime filetime_to_datetime(const time_type& ft) {
#ifdef MSTL_PLATFORM_WINDOWS__
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
#elif defined(MSTL_PLATFORM_LINUX__)
        if (ft == 0) {
            return datetime::epoch();
        }
        std::tm tm_local{};
        ::localtime_r(&ft, &tm_local);
        return datetime(
            tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday,
            tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec
        );
#endif
    }

    static time_type datetime_to_filetime(const datetime& dt) {
#ifdef MSTL_PLATFORM_WINDOWS__
        time_type ft = {0, 0};
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
#elif defined(MSTL_PLATFORM_LINUX__)
        std::tm tm_val{};
        tm_val.tm_year = dt.get_year() - 1900;
        tm_val.tm_mon = dt.get_month() - 1;
        tm_val.tm_mday = dt.get_day();
        tm_val.tm_hour = dt.get_hours();
        tm_val.tm_min = dt.get_minutes();
        tm_val.tm_sec = dt.get_seconds();
        tm_val.tm_isdst = -1;
        return mktime(&tm_val);
#endif
    }

    static string get_last_error_msg()  {
#ifdef MSTL_PLATFORM_WINDOWS__
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
#elif defined(MSTL_PLATFORM_LINUX__)
        return {strerror(errno)};
#endif
    }

#ifdef MSTL_PLATFORM_LINUX__
    static mode_t convert_attributes(const FILE_ATTRIBUTE attr) {
        mode_t mode = 0;

        if ((attr & FILE_ATTRIBUTE::DIRECTORY) != FILE_ATTRIBUTE::OTHERS) {
            mode |= S_IFDIR;
        } else if ((attr & FILE_ATTRIBUTE::DEVICE) != FILE_ATTRIBUTE::OTHERS) {
            mode |= S_IFBLK | S_IFCHR;
        } else if ((attr & FILE_ATTRIBUTE::REPARSE_POINT) != FILE_ATTRIBUTE::OTHERS) {
            mode |= S_IFLNK;
        } else {
            mode |= S_IFREG;
        }

        if ((attr & FILE_ATTRIBUTE::READONLY) != FILE_ATTRIBUTE::OTHERS) {
            mode |= S_IRUSR | S_IRGRP | S_IROTH;
        } else {
            mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        }

        return mode;
    }
#endif

public:
    file() = default;
    explicit file(string path) : path_(_MSTL move(path)) {}

    file(const file&) = delete;
    file& operator =(const file&) = delete;

    file(file&& other) noexcept
    : handle_(other.handle_), path_(_MSTL move(other.path_)),
    opened_(other.opened_), append_mode_(other.append_mode_) {
        other.handle_ = INVALID_HANDLE();
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

        other.handle_ = INVALID_HANDLE();
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

#ifdef MSTL_PLATFORM_WINDOWS__
        handle_ = CreateFileA(path.c_str(),
            static_cast<file_flag_type>(access),
            static_cast<file_flag_type>(share_mode),
            nullptr, static_cast<file_flag_type>(creation),
            static_cast<file_flag_type>(attributes), nullptr);
#elif defined(MSTL_PLATFORM_LINUX__)
        int flags = static_cast<file_flag_type>(access);
        flags |= static_cast<file_flag_type>(creation);
        if (append) flags |= O_APPEND;

        const ::mode_t mode = convert_attributes(attributes);
        handle_ = ::open(path.c_str(), flags, mode);
#endif
        if (handle_ == INVALID_HANDLE()) {
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
        if (opened_ && handle_ != INVALID_HANDLE()) {
#ifdef MSTL_PLATFORM_WINDOWS__
            CloseHandle(handle_);
#elif defined(MSTL_PLATFORM_LINUX__)
            ::close(handle_);
#endif
            handle_ = INVALID_HANDLE();
            opened_ = false;
            append_mode_ = false;
        }
    }

    bool flush() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;
#ifdef MSTL_PLATFORM_WINDOWS__
        return FlushFileBuffers(handle_) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return fsync(handle_) == 0;
#endif
    }

    size_type write(const string& data, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

#ifdef MSTL_PLATFORM_WINDOWS__
        if (append_mode_) {
            SetFilePointer(handle_, 0, nullptr, FILE_END);
        }
        size_type bytes_written;
        const BOOL success = WriteFile(handle_, data.c_str(), size, &bytes_written, nullptr);
        if (!success) {
            return 0;
        }
#elif defined(MSTL_PLATFORM_LINUX__)
        ssize_t bytes_written = ::write(handle_, data.c_str(), size);
        if (bytes_written == -1) {
            return 0;
        }
#endif
        return static_cast<size_type>(bytes_written);
    }

    size_type read(string& str, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

        str.resize(size);
#ifdef MSTL_PLATFORM_WINDOWS__
        size_type bytes_read;
        const BOOL success = ReadFile(handle_, str.data(), size, &bytes_read, nullptr);
        if (!success) {
            return 0;
        }
#elif defined(MSTL_PLATFORM_LINUX__)
        ssize_t bytes_read = ::read(handle_, str.data(), size);
        if (bytes_read == -1) {
            return 0;
        }
#endif
        str.resize(bytes_read);
        return static_cast<size_type>(bytes_read);
    }

    bool read_line(string& line) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

        line.clear();
        char ch;
#ifdef MSTL_PLATFORM_WINDOWS__
        size_type bytes_read;
        while (ReadFile(handle_, &ch, 1, &bytes_read, nullptr) && bytes_read == 1) {
            if (ch == '\n') break;
            if (ch != '\r') line += ch;
        }
#elif defined(MSTL_PLATFORM_LINUX__)
        ssize_t bytes_read;
        while ((bytes_read = ::read(handle_, &ch, 1))) {
            if (bytes_read <= 0) break;
            if (ch == '\n') break;
            if (ch != '\r') line += ch;
        }
#endif
        return !line.empty() || bytes_read > 0;
    }

    vector<string> read_all_lines() const {
        vector<string> lines;
        if (!opened_ || handle_ == INVALID_HANDLE())
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
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

#ifdef MSTL_PLATFORM_WINDOWS__
        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(handle_, &file_size)) {
            return 0;
        }
        return static_cast<size_type>(file_size.QuadPart);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (fstat(handle_, &st) == -1) {
            return 0;
        }
        return static_cast<size_type>(st.st_size);
#endif
    }

    bool seek(const difference_type distance,
        FILE_POINT_ORIGIN method = FILE_POINT_ORIGIN::END) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

#ifdef MSTL_PLATFORM_WINDOWS__
        LARGE_INTEGER li;
        li.QuadPart = distance;
        return SetFilePointerEx(handle_, li,
            nullptr, static_cast<file_flag_type>(method)) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        const ::off_t ret = ::lseek(handle_, distance, static_cast<int>(method));
        return ret != static_cast<off_t>(-1);
#endif
    }

    size_type tell() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

#ifdef MSTL_PLATFORM_WINDOWS__
        constexpr LARGE_INTEGER li = {};
        LARGE_INTEGER new_pos;
        if (!SetFilePointerEx(handle_, li, &new_pos, FILE_CURRENT)) {
            return 0;
        }
        return static_cast<size_type>(new_pos.QuadPart);
#elif defined(MSTL_PLATFORM_LINUX__)
        const ::off_t pos = lseek(handle_, 0, SEEK_CUR);
        return pos == static_cast<::off_t>(-1) ? 0 : static_cast<size_type>(pos);
#endif
    }

    bool truncate(const difference_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

#ifdef MSTL_PLATFORM_WINDOWS__
        if (!seek(size, FILE_POINT_ORIGIN::BEGIN)) {
            return false;
        }
        return SetEndOfFile(handle_) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return ftruncate(handle_, size) == 0;
#endif
    }

    bool lock(const difference_type offset, const difference_type length,
        FILE_LOCK_MODE mode = FILE_LOCK_MODE::EXCLUSIVE) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

#ifdef MSTL_PLATFORM_WINDOWS__
        OVERLAPPED ov = {};
        const uint64_t offset_64 = offset;
        ov.Offset = static_cast<size_type>(offset_64 & 0xFFFFFFFF);
        ov.OffsetHigh = static_cast<size_type>(offset_64 >> 32);

        const uint64_t length_64 = length;
        return LockFileEx(handle_, static_cast<file_flag_type>(mode), 0,
            length_64 & 0xFFFFFFFF, length_64 >> 32, &ov) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::flock fl{};
        fl.l_type = static_cast<int>(mode) & LOCK_EX ? F_WRLCK : F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = offset;
        fl.l_len = length;

        const int cmd = static_cast<int>(mode) & LOCK_NB ? F_SETLK : F_SETLKW;
        return fcntl(handle_, cmd, &fl) != -1;
#endif
    }

    bool unlock(const difference_type offset, const difference_type length) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

#ifdef MSTL_PLATFORM_WINDOWS__
        OVERLAPPED ov = {};
        const uint64_t offset_64 = offset;
        ov.Offset = static_cast<size_type>(offset_64 & 0xFFFFFFFF);
        ov.OffsetHigh = static_cast<size_type>(offset_64 >> 32);

        const uint64_t length_64 = length;
        return UnlockFileEx(handle_, 0, length_64 & 0xFFFFFFFF, length_64 >> 32, &ov) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::flock fl{};
        fl.l_type = F_UNLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = offset;
        fl.l_len = length;
        return fcntl(handle_, F_SETLK, &fl) != -1;
#endif
    }

    FILE_ATTRIBUTE attributes() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return FILE_ATTRIBUTE::OTHERS;

#ifdef MSTL_PLATFORM_WINDOWS__
        BY_HANDLE_FILE_INFORMATION info;
        if (!GetFileInformationByHandle(handle_, &info))
            return FILE_ATTRIBUTE::OTHERS;
        return static_cast<FILE_ATTRIBUTE>(info.dwFileAttributes);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (fstat(handle_, &st) == -1)
            return FILE_ATTRIBUTE::OTHERS;
        return static_cast<FILE_ATTRIBUTE>(st.st_mode);
#endif
    }

    bool set_attributes(FILE_ATTRIBUTE attr) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;
#ifdef MSTL_PLATFORM_WINDOWS__
        return SetFileAttributesA(path_.c_str(), static_cast<file_flag_type>(attr)) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st_old{};
        if (fstat(handle_, &st_old) == -1) {
            return false;
        }
        const mode_t current_mode = st_old.st_mode;
        constexpr mode_t perm_mask = S_IRWXU | S_IRWXG | S_IRWXO;
        const mode_t new_perm = static_cast<mode_t>(attr) & perm_mask;
        const mode_t new_mode = (current_mode & ~perm_mask) | new_perm;
        return fchmod(handle_, new_mode) == 0;
#endif
    }

#ifdef MSTL_PLATFORM_WINDOWS__
    datetime creation_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return datetime::epoch();

        time_type ft_create, ft_access, ft_write;
        if (!GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_create);
    }
#endif

    datetime last_access_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return datetime::epoch();

#ifdef MSTL_PLATFORM_WINDOWS__
        time_type ft_create, ft_access, ft_write;
        if (!GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_access);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (fstat(handle_, &st) == -1) {
            const ::time_t now = ::time(nullptr);
            return filetime_to_datetime(now);
        }
        return filetime_to_datetime(st.st_atime);
#endif
    }

    datetime last_write_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return datetime::epoch();

#ifdef MSTL_PLATFORM_WINDOWS__
        time_type ft_create, ft_access, ft_write;
        if (!GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_write);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (fstat(handle_, &st) == -1) {
            const ::time_t now = ::time(nullptr);
            return filetime_to_datetime(now);
        }
        return filetime_to_datetime(st.st_mtime);
#endif
    }

#ifdef MSTL_PLATFORM_WINDOWS__
    bool set_all_times(const datetime& create,
        const datetime& access, const datetime& write) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

        const time_type ft_create = datetime_to_filetime(create);
        const time_type ft_access = datetime_to_filetime(access);
        const time_type ft_write = datetime_to_filetime(write);
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }
#elif defined(MSTL_PLATFORM_LINUX__)
    bool set_all_times(const datetime& access, const datetime& write) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

        ::timeval times[2];
        times[0].tv_sec = access.to_timestamp().get_seconds();
        times[0].tv_usec = 0;
        times[1].tv_sec = write.to_timestamp().get_seconds();
        times[1].tv_usec = 0;
        return futimes(handle_, times) == 0;
    }
#endif

#ifdef MSTL_PLATFORM_WINDOWS__
    bool set_creation_time(const datetime& dt) const {
        const time_type ft_create = datetime_to_filetime(dt);
        time_type ft_access, ft_write;
        if (!GetFileTime(handle_, nullptr, &ft_access, &ft_write)) {
            return false;
        }
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }
#endif

    bool set_last_access_time(const datetime& dt) const {
#ifdef MSTL_PLATFORM_WINDOWS__
        const time_type ft_access = datetime_to_filetime(dt);
        time_type ft_create, ft_write;
        if (!GetFileTime(handle_, &ft_create, nullptr, &ft_write)) {
            return false;
        }
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return set_all_times(dt, last_write_time());
#endif
    }

    bool set_last_write_time(const datetime& dt) const {
#ifdef MSTL_PLATFORM_WINDOWS__
        const time_type ft_write = datetime_to_filetime(dt);
        time_type ft_create, ft_access;
        if (!GetFileTime(handle_, &ft_create, &ft_access, nullptr)) {
            return false;
        }
        return SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return set_all_times(last_access_time(), datetime::to_utc(dt));
#endif
    }


    const string& file_path() const { return path_; }
    bool opened() const { return opened_; }
    bool is_append_mode() const { return append_mode_; }


    static bool exists(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        return stat(path.c_str(), &st) == 0;
#endif
    }

    static bool is_directory(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        const size_type attrib = GetFileAttributesA(path.c_str());
        return attrib != INVALID_FILE_ATTRIBUTES
            && attrib & FILE_ATTRIBUTE_DIRECTORY;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (stat(path.c_str(), &st) == -1) return false;
        return S_ISDIR(st.st_mode);
#endif
    }

    static bool is_file(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        const size_type attrib = GetFileAttributesA(path.c_str());
        return attrib != INVALID_FILE_ATTRIBUTES
            && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (stat(path.c_str(), &st) == -1) return false;
        return S_ISREG(st.st_mode) || S_ISLNK(st.st_mode);
#endif
    }

    static bool create_directories(const string& path) {
        if (path.empty()) return false;
        if (is_directory(path)) return true;
        size_t pos = 0;
#ifdef MSTL_PLATFORM_WINDOWS__
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
        return CreateDirectoryA(path.c_str(), nullptr)
            || GetLastError() == ERROR_ALREADY_EXISTS;
#elif defined(MSTL_PLATFORM_LINUX__)
        string dir;
        while ((pos = path.find_first_of(FILE_SPLITER, pos + 1)) != string::npos) {
            dir = path.substr(0, pos);
            if (mkdir(dir.c_str(), 0755) == -1 && errno != EEXIST) {
                return false;
            }
        }
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
    }

    static bool create_and_write(const string& path, const string& content,
        const bool append = false) {
        const size_t last_slash = path.find_last_of(FILE_SPLITER);
        if (last_slash != string::npos) {
            const string dir = path.substr(0, last_slash);
            if (!dir.empty() && !is_directory(dir)) {
                if (!create_directories(dir)) {
                    return false;
                }
            }
        }
#ifdef MSTL_PLATFORM_WINDOWS__
        file file;
        if (!file.open(path,
            append ? FILE_ACCESS_MODE::APPEND : FILE_ACCESS_MODE::WRITE,
            FILE_SHARED_MODE::NO_SHARE, FILE_CREATION_MODE::OPEN_FORCE,
            FILE_ATTRIBUTE::NORMAL, append)) {
            return false;
        }
        const size_type bytes_written = file.write(content.c_str(), content.size());
        return bytes_written == content.size();
#elif defined(MSTL_PLATFORM_LINUX__)
        int flags = O_WRONLY | O_CREAT;
        if (append) flags |= O_APPEND;
        else flags |= O_TRUNC;

        const int fd = ::open(path.c_str(), flags, 0644);
        if (fd == -1) return false;

        const ssize_t written = ::write(fd, content.c_str(), content.size());
        ::close(fd);
        return written == static_cast<ssize_t>(content.size());
#endif
    }

    static bool remove(const string& path) {
        if (is_file(path)) {
#ifdef MSTL_PLATFORM_WINDOWS__
            return DeleteFileA(path.c_str()) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
            return ::unlink(path.c_str()) == 0;
#endif
        }
        return false;
    }

    static bool remove_directory(const string& path) {
        if (is_directory(path)) {
#ifdef MSTL_PLATFORM_WINDOWS__
            return RemoveDirectoryA(path.c_str()) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
            return ::rmdir(path.c_str()) == 0;
#endif
        }
        return false;
    }

    static bool read_all(const string& path, string& content) {
#ifdef MSTL_PLATFORM_WINDOWS__
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
#elif defined(MSTL_PLATFORM_LINUX__)
        const int fd = ::open(path.c_str(), O_RDONLY);
        if (fd == -1) return false;

        struct ::stat st{};
        if (fstat(fd, &st) == -1) {
            ::close(fd);
            return false;
        }
        content.resize(st.st_size);
        ssize_t read = ::read(fd, content.data(), st.st_size);
        ::close(fd);
        return read == st.st_size;
#endif
    }

    static bool copy(const string& from, const string& to, const bool overwrite = true) {
#ifdef MSTL_PLATFORM_WINDOWS__
        return CopyFileA(from.c_str(), to.c_str(), !overwrite) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        if (!overwrite && exists(to)) return false;
        string content;
        if (!read_all(from, content)) return false;
        return create_and_write(to, content, false);
#endif
    }

    static bool move(const string& from, const string& to, const bool overwrite = true) {
#ifdef MSTL_PLATFORM_WINDOWS__
        size_type flags = MOVEFILE_COPY_ALLOWED;
        if (overwrite) {
            flags |= MOVEFILE_REPLACE_EXISTING;
        }
        return MoveFileExA(from.c_str(), to.c_str(), flags) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        if (overwrite) {
            if (::rename(from.c_str(), to.c_str()) == 0) return true;
            if (errno != EEXIST) return false;
            if (remove(to) != 0) return false;
            return ::rename(from.c_str(), to.c_str()) == 0;
        }
        return ::rename(from.c_str(), to.c_str()) == 0;
#endif
    }

    static bool rename(const string& old_name, const string& new_name) {
        return move(old_name, new_name, true);
    }

    static size_type file_size(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        file f;
        if (f.open(path, FILE_ACCESS_MODE::READ, FILE_SHARED_MODE::SHARE_READ,
                   FILE_CREATION_MODE::OPEN_EXIST)) {
            return f.size();
        }
        return 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (stat(path.c_str(), &st) == -1) return 0;
        return static_cast<size_type>(st.st_size);
#endif
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_FILE_HPP__
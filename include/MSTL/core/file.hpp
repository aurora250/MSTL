#ifndef MSTL_FILE_HPP__
#define MSTL_FILE_HPP__
#include "vector.hpp"
#include "datetime.hpp"
#ifdef MSTL_PLATFORM_LINUX__
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#endif
MSTL_BEGIN_NAMESPACE__

using file_flag_type =
#ifdef MSTL_PLATFORM_WINDOWS__
    unsigned long;
#elif defined(MSTL_PLATFORM_LINUX__)
    int;
#endif


enum class FILE_ACCESS : file_flag_type {
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

inline FILE_ACCESS operator |(FILE_ACCESS a, FILE_ACCESS b) {
    return static_cast<FILE_ACCESS>(static_cast<int>(a) | static_cast<int>(b));
}
inline FILE_ACCESS operator &(FILE_ACCESS a, FILE_ACCESS b) {
    return static_cast<FILE_ACCESS>(static_cast<int>(a) & static_cast<int>(b));
}


enum class FILE_SHARED : file_flag_type {
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

inline FILE_SHARED operator |(FILE_SHARED a, FILE_SHARED b) {
    return static_cast<FILE_SHARED>(static_cast<int>(a) | static_cast<int>(b));
}
inline FILE_SHARED operator &(FILE_SHARED a, FILE_SHARED b) {
    return static_cast<FILE_SHARED>(static_cast<int>(a) & static_cast<int>(b));
}


enum class FILE_CREATION : file_flag_type {
#ifdef MSTL_PLATFORM_WINDOWS__
    CREATE_FORCE = CREATE_ALWAYS,
    CREATE_NO_EXIST = CREATE_NEW,
    OPEN_FORCE = OPEN_ALWAYS,
    OPEN_EXIST = OPEN_EXISTING,
    TRUNCATE_EXIST = TRUNCATE_EXISTING
#elif defined(MSTL_PLATFORM_LINUX__)
    CREATE_FORCE = O_CREAT | O_TRUNC | O_WRONLY,
    CREATE_NO_EXIST = O_CREAT | O_EXCL,
    OPEN_FORCE = O_CREAT,
    OPEN_EXIST = 0,
    TRUNCATE_EXIST = O_TRUNC
#endif
};

inline FILE_CREATION operator |(FILE_CREATION a, FILE_CREATION b) {
    return static_cast<FILE_CREATION>(static_cast<int>(a) | static_cast<int>(b));
}
inline FILE_CREATION operator &(FILE_CREATION a, FILE_CREATION b) {
    return static_cast<FILE_CREATION>(static_cast<int>(a) & static_cast<int>(b));
}


enum class FILE_ATTRI : file_flag_type {
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

inline FILE_ATTRI operator |(FILE_ATTRI a, FILE_ATTRI b) {
    return static_cast<FILE_ATTRI>(
        static_cast<file_flag_type>(a) | static_cast<file_flag_type>(b));
}
inline FILE_ATTRI operator &(FILE_ATTRI a, FILE_ATTRI b) {
    return static_cast<FILE_ATTRI>(
        static_cast<file_flag_type>(a) & static_cast<file_flag_type>(b));
}


enum class FILE_POINTER : file_flag_type {
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

enum class FILE_LOCK : file_flag_type {
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

inline FILE_LOCK operator |(FILE_LOCK a, FILE_LOCK b) {
    return static_cast<FILE_LOCK>(static_cast<int>(a) | static_cast<int>(b));
}
inline FILE_LOCK operator &(FILE_LOCK a, FILE_LOCK b) {
    return static_cast<FILE_LOCK>(static_cast<int>(a) & static_cast<int>(b));
}


class file {
public:
#ifdef MSTL_PLATFORM_WINDOWS__
    using size_type = DWORD;
    using difference_type = LONG;
    using file_handle = HANDLE;
    using time_type = FILETIME;
#elif defined(MSTL_PLATFORM_LINUX__)
    using size_type = size_t;
    using difference_type = ::off_t;
    using file_handle = int;
    using time_type = ::time_t;
#endif

private:
    static file_handle INVALID_HANDLE() {
        static file_handle INVALID_HANDLE =
#ifdef MSTL_PLATFORM_WINDOWS__
            INVALID_HANDLE_VALUE;
#elif defined(MSTL_PLATFORM_LINUX__)
            -1;
#endif
        return INVALID_HANDLE;
    }

    file_handle handle_ = INVALID_HANDLE();
    string path_{};
    bool opened_ = false;
    bool append_mode_ = false;

    static constexpr size_type BUFFER_SIZE = 8192; // 8KB Buffer
    mutable vector<char> read_buffer_{};
    mutable size_type read_buffer_pos_ = 0;
    mutable size_type read_buffer_size_ = 0;
    mutable vector<char> write_buffer_{};
    mutable size_type write_buffer_pos_ = 0;

    static constexpr auto FILE_SPLITER =
#ifdef MSTL_PLATFORM_WINDOWS__
        "/\\";
#elif defined(MSTL_PLATFORM_LINUX__)
        "/";
#endif

private:
    bool flush_write_buffer() const {
        if (write_buffer_pos_ == 0) return true;
#ifdef MSTL_PLATFORM_WINDOWS__
        size_type bytes_written;
        const BOOL success = WriteFile(handle_, write_buffer_.data(),
            write_buffer_pos_, &bytes_written, nullptr
        );
        if (!success || bytes_written != write_buffer_pos_) {
            return false;
        }
#elif defined(MSTL_PLATFORM_LINUX__)
        ssize_t bytes_written = ::write(handle_, write_buffer_.data(), write_buffer_pos_);
        if (bytes_written != static_cast<ssize_t>(write_buffer_pos_)) {
            return false;
        }
#endif
        write_buffer_pos_ = 0;
        return true;
    }

    bool fill_read_buffer() const {
#ifdef MSTL_PLATFORM_WINDOWS__
        size_type bytes_read;
        const BOOL success = ReadFile(
            handle_, read_buffer_.data(),
            BUFFER_SIZE,
            &bytes_read, nullptr
        );
        if (!success) {
            read_buffer_size_ = 0;
            return false;
        }
        read_buffer_size_ = bytes_read;
#elif defined(MSTL_PLATFORM_LINUX__)
        ssize_t bytes_read = ::read(handle_, read_buffer_.data(), BUFFER_SIZE);
        if (bytes_read <= 0) {
            read_buffer_size_ = 0;
            return false;
        }
        read_buffer_size_ = static_cast<size_type>(bytes_read);
#endif
        read_buffer_pos_ = 0;
        return true;
    }


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
        ::tm tm_local{};
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
        ::SYSTEMTIME st_local;
        st_local.wYear = static_cast<WORD>(dt.get_year());
        st_local.wMonth = static_cast<WORD>(dt.get_month());
        st_local.wDay = static_cast<WORD>(dt.get_day());
        st_local.wHour = static_cast<WORD>(dt.get_hours());
        st_local.wMinute = static_cast<WORD>(dt.get_minutes());
        st_local.wSecond = static_cast<WORD>(dt.get_seconds());
        st_local.wMilliseconds = 0;

        ::SYSTEMTIME st_utc;
        if (!::TzSpecificLocalTimeToSystemTime(nullptr, &st_local, &st_utc)) {
            return ft;
        }
        if (!::SystemTimeToFileTime(&st_utc, &ft)) {
            ft.dwHighDateTime = 0;
            ft.dwLowDateTime = 0;
        }
        return ft;
#elif defined(MSTL_PLATFORM_LINUX__)
        ::tm tm_val{};
        tm_val.tm_year = dt.get_year() - 1900;
        tm_val.tm_mon = dt.get_month() - 1;
        tm_val.tm_mday = dt.get_day();
        tm_val.tm_hour = dt.get_hours();
        tm_val.tm_min = dt.get_minutes();
        tm_val.tm_sec = dt.get_seconds();
        tm_val.tm_isdst = -1;
        return ::mktime(&tm_val);
#endif
    }

    static string get_last_error_msg()  {
#ifdef MSTL_PLATFORM_WINDOWS__
        const size_type error_code = ::GetLastError();
        if (error_code == 0) {
            return {};
        }

        ::LPSTR message_buffer = nullptr;
        const size_t size = ::FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<::LPSTR>(&message_buffer), 0, nullptr);

        string message(message_buffer, size);
        ::LocalFree(message_buffer);

        return message;
#elif defined(MSTL_PLATFORM_LINUX__)
        char buffer[256];
        return ::strerror_r(errno, buffer, sizeof(buffer));
#endif
    }

#ifdef MSTL_PLATFORM_LINUX__
    static mode_t convert_attributes(const FILE_ATTRI attr) {
        mode_t mode = 0;

        if ((attr & FILE_ATTRI::DIRECTORY) != FILE_ATTRI::OTHERS) {
            mode |= S_IFDIR;
        } else if ((attr & FILE_ATTRI::DEVICE) != FILE_ATTRI::OTHERS) {
            mode |= S_IFBLK | S_IFCHR;
        } else if ((attr & FILE_ATTRI::REPARSE_POINT) != FILE_ATTRI::OTHERS) {
            mode |= S_IFLNK;
        } else {
            mode |= S_IFREG;
        }

        if ((attr & FILE_ATTRI::READONLY) != FILE_ATTRI::OTHERS) {
            mode |= S_IRUSR | S_IRGRP | S_IROTH;
        } else {
            mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        }

        return mode;
    }
#endif

public:
    file() = default;

    explicit file(string path,
        const FILE_ACCESS access = FILE_ACCESS::READ_WRITE,
        const FILE_SHARED share_mode = FILE_SHARED::SHARE_READ,
        const FILE_CREATION creation = FILE_CREATION::OPEN_EXIST,
        const FILE_ATTRI attributes = FILE_ATTRI::NORMAL,
        const bool append = false) : path_(_MSTL move(path)) {
        this->open(access, share_mode, creation, attributes, append);
    }

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

        this->close();
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
        this->close();
    }


    bool open(const string& path,
        FILE_ACCESS access = FILE_ACCESS::READ_WRITE,
        FILE_SHARED share_mode = FILE_SHARED::SHARE_READ,
        FILE_CREATION creation = FILE_CREATION::OPEN_EXIST,
        FILE_ATTRI attributes = FILE_ATTRI::NORMAL,
        const bool append = false) {
        this->close();

        read_buffer_.resize(BUFFER_SIZE);
        write_buffer_.resize(BUFFER_SIZE);
        read_buffer_pos_ = 0;
        read_buffer_size_ = 0;
        write_buffer_pos_ = 0;

#ifdef MSTL_PLATFORM_WINDOWS__
        handle_ = ::CreateFileA(path.c_str(),
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
            if (!this->seek(0, FILE_POINTER::END)) return false;
        }
        return true;
    }

    bool open(const FILE_ACCESS access = FILE_ACCESS::READ_WRITE,
        const FILE_SHARED share_mode = FILE_SHARED::SHARE_READ,
        const FILE_CREATION creation = FILE_CREATION::OPEN_EXIST,
        const FILE_ATTRI attributes = FILE_ATTRI::NORMAL,
        const bool append = false) {
        return this->open(path_, access, share_mode, creation, attributes, append);
    }

    void close() {
        if (opened_ && handle_ != INVALID_HANDLE()) {
            this->flush_write_buffer();
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
        if (!opened_ || handle_ == INVALID_HANDLE()) {
            return false;
        }
        if (!flush_write_buffer()) {
            return false;
        }
#ifdef MSTL_PLATFORM_WINDOWS__
        return ::FlushFileBuffers(handle_) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return ::fsync(handle_) == 0;
#endif
    }

    size_type write(const string& data, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

        const size_type real_size = size > data.size() ? data.size() : size;
        if (real_size == 0) return 0;

        if (real_size > BUFFER_SIZE * 4) {
            if (!flush_write_buffer()) {
                return 0;
            }
#ifdef MSTL_PLATFORM_WINDOWS__
            size_type bytes_written = 0;
            if (::WriteFile(handle_, data.data(), real_size, &bytes_written, nullptr)) {
                return bytes_written;
            }
            return 0;
#elif defined(MSTL_PLATFORM_LINUX__)
            ssize_t written;
            do {
                written = ::write(handle_, data.data(), real_size);
            } while (written == -1 && errno == EINTR);
            return written > 0 ? static_cast<size_type>(written) : 0;
#else
            return 0;
#endif
        }

        const char* ptr = data.data();
        size_type total_written = 0;
        size_type remaining = size;

        while (remaining > 0) {
            const size_type available = BUFFER_SIZE - write_buffer_pos_;
            const size_type to_copy = remaining < available ? remaining : available;

            _MSTL copy_n(ptr, to_copy, write_buffer_.begin() + static_cast<ptrdiff_t>(write_buffer_pos_));
            write_buffer_pos_ += to_copy;
            total_written += to_copy;
            ptr += to_copy;
            remaining -= to_copy;

            if (write_buffer_pos_ == BUFFER_SIZE && !flush_write_buffer()) {
                break;
            }
        }
        return total_written;
    }

    size_type write(const string& data) const {
        return this->write(data, data.size());
    }

    size_type read(string& str, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

        str.clear();
        str.reserve(size);
        size_type total_read = 0;

        while (total_read < size) {
            if (read_buffer_pos_ >= read_buffer_size_) {
                if (!fill_read_buffer() || read_buffer_size_ == 0) {
                    break;
                }
            }
            const size_type available_in_buffer = read_buffer_size_ - read_buffer_pos_;
            const size_type needed = size - total_read;
            const size_type to_read = (needed < available_in_buffer) ?
                needed : available_in_buffer;

            str.append(read_buffer_.data() + read_buffer_pos_, to_read);
            read_buffer_pos_ += to_read;
            total_read += to_read;
        }
        return total_read;
    }

    string read() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return {};

        const size_type file_size = size();
        if (file_size == 0) return {};

        const difference_type current_pos = this->tell();

        if (!this->seek(0, FILE_POINTER::BEGIN)) return {};

        string content;
        content.resize(file_size);
        const size_type bytes_read = this->read(content, file_size);

        if (!this->seek(current_pos, FILE_POINTER::BEGIN)) return {};

        if (bytes_read != file_size) {
            content.resize(bytes_read);
        }
        return content;
    }

    size_type read_binary(string& str, const size_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE() || str.empty() || size == 0)
            return 0;

        size_type total_read = 0;

        while (total_read < size) {
            if (read_buffer_pos_ >= read_buffer_size_) {
                if (!fill_read_buffer() || read_buffer_size_ == 0) {
                    break;
                }
            }
            const size_type available = read_buffer_size_ - read_buffer_pos_;
            const size_type to_read =
                size - total_read < available ? size - total_read : available;

            const auto buffer = str.data();
            _MSTL copy_n(read_buffer_.data() + read_buffer_pos_, to_read, buffer + total_read);
            read_buffer_pos_ += to_read;
            total_read += to_read;
        }
        return total_read;
    }

    string read_binary() const {
        return this->read_binary(path_);
    }

    bool read_line(string& line) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

        line.clear();
        bool line_complete = false;

        while (!line_complete) {
            if (read_buffer_pos_ >= read_buffer_size_) {
                if (!fill_read_buffer() || read_buffer_size_ == 0) {
                    break;
                }
            }

            while (read_buffer_pos_ < read_buffer_size_ && !line_complete) {
                const char ch = read_buffer_[read_buffer_pos_++];
                if (ch == '\n') {
                    line_complete = true;
                } else if (ch != '\r') {
                    line += ch;
                }
            }
        }
        return !line.empty() || line_complete;
    }

    vector<string> read_lines() const {
        vector<string> lines;
        if (!opened_ || handle_ == INVALID_HANDLE())
            return lines;

        string content;
        if (this->read(path_, content)) {
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
        if (!opened_ || handle_ == INVALID_HANDLE()) {
            return 0;
        }
        if (write_buffer_pos_ > 0 && !flush_write_buffer()) {
            return 0;
        }

#ifdef MSTL_PLATFORM_WINDOWS__
        LARGE_INTEGER file_size;
        if (!GetFileSizeEx(handle_, &file_size)) {
            return 0;
        }
        return static_cast<size_type>(file_size.QuadPart);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::fstat(handle_, &st) == -1) {
            return 0;
        }
        return static_cast<size_type>(st.st_size);
#endif
    }

    bool seek(const difference_type distance,
        FILE_POINTER method = FILE_POINTER::END) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

        if (write_buffer_pos_ > 0 && !flush_write_buffer()) {
            return false;
        }
        read_buffer_pos_ = 0;
        read_buffer_size_ = 0;

#ifdef MSTL_PLATFORM_WINDOWS__
        LARGE_INTEGER li;
        li.QuadPart = distance;
        return ::SetFilePointerEx(handle_, li,
            nullptr, static_cast<file_flag_type>(method)) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        const ::off_t ret = ::lseek(handle_, distance, static_cast<int>(method));
        return ret != static_cast<off_t>(-1);
#endif
    }

    difference_type tell() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return 0;

#ifdef MSTL_PLATFORM_WINDOWS__
        constexpr LARGE_INTEGER li = {};
        LARGE_INTEGER new_pos;
        if (!::SetFilePointerEx(handle_, li, &new_pos, FILE_CURRENT)) {
            return 0;
        }
        return static_cast<size_type>(new_pos.QuadPart);
#elif defined(MSTL_PLATFORM_LINUX__)
        const ::off_t pos = ::lseek(handle_, 0, SEEK_CUR);
        return pos == static_cast<::off_t>(-1) ? 0 : pos;
#endif
    }

    bool prefetch(const size_type hint_size = 0) const {
        if (read_buffer_pos_ < read_buffer_size_) return true;

        const size_type read_size = hint_size > 0 ?
            _MSTL min(hint_size * 2, BUFFER_SIZE) :
            BUFFER_SIZE;
#ifdef MSTL_PLATFORM_LINUX__
        ::posix_fadvise(handle_, this->tell(), static_cast<difference_type>(read_size), POSIX_FADV_WILLNEED);
#endif
        return fill_read_buffer();
    }

    bool truncate(const difference_type size) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

#ifdef MSTL_PLATFORM_WINDOWS__
        if (!this->seek(size, FILE_POINTER::BEGIN)) {
            return false;
        }
        return ::SetEndOfFile(handle_) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return ::ftruncate(handle_, size) == 0;
#endif
    }

    bool lock(const difference_type offset, const difference_type length,
        FILE_LOCK mode = FILE_LOCK::EXCLUSIVE) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;

#ifdef MSTL_PLATFORM_WINDOWS__
        OVERLAPPED ov = {};
        const uint64_t offset_64 = offset;
        ov.Offset = static_cast<size_type>(offset_64 & 0xFFFFFFFF);
        ov.OffsetHigh = static_cast<size_type>(offset_64 >> 32);

        const uint64_t length_64 = length;
        return ::LockFileEx(handle_, static_cast<file_flag_type>(mode), 0,
            length_64 & 0xFFFFFFFF, length_64 >> 32, &ov) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::flock fl{};
        if ((mode & FILE_LOCK::EXCLUSIVE) != FILE_LOCK::SHARED) {
            fl.l_type = F_WRLCK;
        } else {
            fl.l_type = F_RDLCK;
        }
        fl.l_whence = SEEK_SET;
        fl.l_start = offset;
        fl.l_len = length;

        const int cmd = static_cast<int>(mode) & LOCK_NB ? F_SETLK : F_SETLKW;
        return ::fcntl(handle_, cmd, &fl) != -1;
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
        return ::UnlockFileEx(handle_, 0, length_64 & 0xFFFFFFFF, length_64 >> 32, &ov) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::flock fl{};
        fl.l_type = F_UNLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = offset;
        fl.l_len = length;
        return ::fcntl(handle_, F_SETLK, &fl) != -1;
#endif
    }

    FILE_ATTRI attributes() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return FILE_ATTRI::OTHERS;

#ifdef MSTL_PLATFORM_WINDOWS__
        BY_HANDLE_FILE_INFORMATION info;
        if (!::GetFileInformationByHandle(handle_, &info))
            return FILE_ATTRI::OTHERS;
        return static_cast<FILE_ATTRI>(info.dwFileAttributes);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::fstat(handle_, &st) == -1)
            return FILE_ATTRI::OTHERS;
        return static_cast<FILE_ATTRI>(st.st_mode);
#endif
    }

    bool set_attributes(FILE_ATTRI attr) const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return false;
#ifdef MSTL_PLATFORM_WINDOWS__
        return ::SetFileAttributesA(path_.c_str(), static_cast<file_flag_type>(attr)) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st_old{};
        if (::fstat(handle_, &st_old) == -1) {
            return false;
        }
        const mode_t current_mode = st_old.st_mode;
        constexpr mode_t perm_mask = S_IRWXU | S_IRWXG | S_IRWXO;
        const mode_t new_perm = static_cast<mode_t>(attr) & perm_mask;
        const mode_t new_mode = (current_mode & ~perm_mask) | new_perm;
        return ::fchmod(handle_, new_mode) == 0;
#endif
    }

#ifdef MSTL_PLATFORM_WINDOWS__
    datetime creation_time() const {
        if (!opened_ || handle_ == INVALID_HANDLE())
            return datetime::epoch();

        time_type ft_create, ft_access, ft_write;
        if (!::GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
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
        if (!::GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_access);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::fstat(handle_, &st) == -1) {
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
        if (!::GetFileTime(handle_, &ft_create, &ft_access, &ft_write)) {
            return datetime::epoch();
        }
        return filetime_to_datetime(ft_write);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::fstat(handle_, &st) == -1) {
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
        return ::SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
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
        return ::futimes(handle_, times) == 0;
    }
#endif

#ifdef MSTL_PLATFORM_WINDOWS__
    bool set_creation_time(const datetime& dt) const {
        const time_type ft_create = datetime_to_filetime(dt);
        time_type ft_access, ft_write;
        if (!::GetFileTime(handle_, nullptr, &ft_access, &ft_write)) {
            return false;
        }
        return ::SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
    }
#endif

    bool set_last_access_time(const datetime& dt) const {
#ifdef MSTL_PLATFORM_WINDOWS__
        const time_type ft_access = datetime_to_filetime(dt);
        time_type ft_create, ft_write;
        if (!::GetFileTime(handle_, &ft_create, nullptr, &ft_write)) {
            return false;
        }
        return ::SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return set_all_times(dt, last_write_time());
#endif
    }

    bool set_last_write_time(const datetime& dt) const {
#ifdef MSTL_PLATFORM_WINDOWS__
        const time_type ft_write = datetime_to_filetime(dt);
        time_type ft_create, ft_access;
        if (!::GetFileTime(handle_, &ft_create, &ft_access, nullptr)) {
            return false;
        }
        return ::SetFileTime(handle_, &ft_create, &ft_access, &ft_write) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        return set_all_times(last_access_time(), datetime::to_utc(dt));
#endif
    }

    const string& file_path() const { return path_; }
    bool opened() const { return opened_; }
    bool is_append_mode() const { return append_mode_; }


    static bool exists(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        return ::GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        return ::stat(path.c_str(), &st) == 0;
#endif
    }

    static bool is_directory(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        const size_type attrib = ::GetFileAttributesA(path.c_str());
        return attrib != INVALID_FILE_ATTRIBUTES
            && attrib & FILE_ATTRIBUTE_DIRECTORY;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::stat(path.c_str(), &st) == -1) return false;
        return S_ISDIR(st.st_mode);
#endif
    }

    static bool is_file(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        const size_type attrib = ::GetFileAttributesA(path.c_str());
        return attrib != INVALID_FILE_ATTRIBUTES
            && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::stat(path.c_str(), &st) == -1) return false;
        return S_ISREG(st.st_mode) || S_ISLNK(st.st_mode);
#endif
    }

    static string get_extension(const string& path) {
        const size_t last_sep = path.find_last_of(FILE_SPLITER);
        string filename = last_sep == string::npos ? path : path.substr(last_sep + 1);

        const size_t last_dot = filename.find_last_of('.');
        if (last_dot == string::npos || last_dot == 0 || last_dot == filename.size() - 1) {
            return {};
        }
        return filename.substr(last_dot + 1);
    }


    bool exists() const { return file::exists(path_); }
    bool is_directory() const { return file::is_directory(path_); }
    bool is_file() const { return file::is_file(path_); }
    string extension() const { return file::get_extension(path_); }


    static bool create_directories(const string& path) {
        if (path.empty()) return false;
        if (file::is_directory(path)) return true;
        size_t pos = 0;
#ifdef MSTL_PLATFORM_WINDOWS__
        string subdir;
        while ((pos = path.find_first_of("/\\", pos + 1)) != string::npos) {
            subdir = path.substr(0, pos);
            if (!subdir.empty() && !file::is_directory(subdir)) {
                if (!::CreateDirectoryA(subdir.c_str(), nullptr)) {
                    if (::GetLastError() != ERROR_ALREADY_EXISTS) {
                        return false;
                    }
                }
            }
        }
        return ::CreateDirectoryA(path.c_str(), nullptr)
            || ::GetLastError() == ERROR_ALREADY_EXISTS;
#elif defined(MSTL_PLATFORM_LINUX__)
        string dir;
        while ((pos = path.find_first_of(FILE_SPLITER, pos + 1)) != string::npos) {
            dir = path.substr(0, pos);
            if (::mkdir(dir.c_str(), 0755) == -1 && errno != EEXIST) {
                return false;
            }
        }
        return ::mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
    }

    bool create_directories() const { return this->create_directories(path_); }

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
            append ? FILE_ACCESS::APPEND : FILE_ACCESS::WRITE,
            FILE_SHARED::NO_SHARE, FILE_CREATION::OPEN_FORCE,
            FILE_ATTRI::NORMAL, append)) {
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
        if (file::is_file(path)) {
#ifdef MSTL_PLATFORM_WINDOWS__
            return ::DeleteFileA(path.c_str()) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
            return ::unlink(path.c_str()) == 0;
#endif
        }
        return false;
    }

    static bool remove_directory(const string& path) {
        if (file::is_directory(path)) {
#ifdef MSTL_PLATFORM_WINDOWS__
            return ::RemoveDirectoryA(path.c_str()) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
            return ::rmdir(path.c_str()) == 0;
#endif
        }
        return false;
    }


    bool remove() const { return this->remove(path_); }
    bool remove_directory() const { return this->remove_directory(path_); }


    static bool read(const string& path, string& content) {
#ifdef MSTL_PLATFORM_WINDOWS__
        file file;
        if (!file.open(path, FILE_ACCESS::READ, FILE_SHARED::SHARE_READ,
            FILE_CREATION::OPEN_EXIST, FILE_ATTRI::NORMAL)) {
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
        if (::fstat(fd, &st) == -1) {
            ::close(fd);
            return false;
        }
        content.resize(st.st_size);
        ssize_t read = ::read(fd, content.data(), st.st_size);
        ::close(fd);
        return read == st.st_size;
#endif
    }

    static string read(const string& path) {
        string content;
        file::read(path, content);
        return content;
    }

    static bool read_binary(const string& path, string& content) {
        file f;
        if (!f.open(path,
            FILE_ACCESS::READ,
            FILE_SHARED::SHARE_READ,
            FILE_CREATION::OPEN_EXIST)) {
            return false;
        }

        const size_type sz = f.size();
        content.resize(sz);

        if (sz > 0) {
            const size_type bytes_read = f.read_binary(content, sz);
            if (bytes_read != sz) {
                content.resize(bytes_read);
            }
        }
        return true;
    }

    static string read_binary(const string& path) {
        string content;
        file::read_binary(path, content);
        return content;
    }

    static bool copy(const string& from, const string& to, const bool overwrite = true) {
#ifdef MSTL_PLATFORM_WINDOWS__
        return ::CopyFileA(from.c_str(), to.c_str(), !overwrite) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        if (!overwrite && file::exists(to)) return false;
        string content;
        if (!file::read(from, content)) return false;
        return create_and_write(to, content, false);
#endif
    }

    static bool move(const string& from, const string& to, const bool overwrite = true) {
#ifdef MSTL_PLATFORM_WINDOWS__
        size_type flags = MOVEFILE_COPY_ALLOWED;
        if (overwrite) {
            flags |= MOVEFILE_REPLACE_EXISTING;
        }
        return ::MoveFileExA(from.c_str(), to.c_str(), flags) != 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        if (overwrite) {
            if (::rename(from.c_str(), to.c_str()) == 0) return true;
            if (errno != EEXIST) return false;
            if (file::remove(to) != 0) return false;
            return ::rename(from.c_str(), to.c_str()) == 0;
        }
        return ::rename(from.c_str(), to.c_str()) == 0;
#endif
    }

    static bool rename(const string& old_name, const string& new_name) {
        return file::move(old_name, new_name, true);
    }

    static size_type file_size(const string& path) {
#ifdef MSTL_PLATFORM_WINDOWS__
        file f;
        if (f.open(path, FILE_ACCESS::READ, FILE_SHARED::SHARE_READ,
                   FILE_CREATION::OPEN_EXIST)) {
            return f.size();
        }
        return 0;
#elif defined(MSTL_PLATFORM_LINUX__)
        struct ::stat st{};
        if (::stat(path.c_str(), &st) == -1) return 0;
        return static_cast<size_type>(st.st_size);
#endif
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_FILE_HPP__
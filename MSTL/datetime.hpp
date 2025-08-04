#ifndef MSTL_DATETIME_HPP__
#define MSTL_DATETIME_HPP__
#include "string.hpp"
MSTL_BEGIN_NAMESPACE__

namespace constants {
    MSTL_INLINE17 static constexpr int32_t month_day[12] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };
}

class date {
public:
    using date_type = int32_t;

private:
    date_type year_ = 1970;
    date_type month_ = 1;
    date_type day_ = 1;

public:
    constexpr explicit date(const date_type year = 1970,
        const date_type month = 1, const date_type day = 1) noexcept {
        if (year >= 0 && (month > 0 && month < 13) &&
            (day > 0 && day <= get_month_day_(year, month))) {
            year_ = year;
            month_ = month;
            day_ = day;
        }
    }
    ~date() = default;


    MSTL_NODISCARD constexpr date_type get_year() const noexcept { return year_; }
    MSTL_NODISCARD constexpr date_type get_month() const noexcept { return month_; }
    MSTL_NODISCARD constexpr date_type get_day() const noexcept { return day_; }


    static constexpr date epoch() noexcept {
        return date{};
    }

    static constexpr bool is_leap_year(const date_type year) noexcept {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    static constexpr date_type get_month_day_(
        const date_type year, const date_type month) noexcept {
        date_type day = constants::month_day[month - 1];
        if (month == 2 && is_leap_year(year)) {
            day += 1;
        }
        return day;
    }

    MSTL_NODISCARD constexpr date_type day_of_week() const noexcept {
        date_type y = year_;
        date_type m = month_;
        date_type d = day_;
        if (m < 3) {
            y--;
            m += 12;
        }
        return (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;
    }

    MSTL_NODISCARD constexpr date_type day_of_year() const noexcept {
        date_type days = 0;
        for (date_type i = 1; i < month_; ++i) {
            days += get_month_day_(year_, i);
        }
        return days + day_;
    }


    constexpr bool operator ==(const date& d) const noexcept {
        return year_ == d.year_ && month_ == d.month_ && day_ == d.day_;
    }
    constexpr bool operator !=(const date& d) const noexcept {
        return !(*this == d);
    }

    constexpr bool operator >(const date& d) const noexcept {
        if (year_ > d.year_) return true;
        if (year_ == d.year_ && month_ > d.month_) return true;
        if (year_ == d.year_ && month_ == d.month_ && day_ > d.day_) return true;
        return false;
    }
    constexpr bool operator >=(const date& d) const noexcept {
        return *this > d || *this == d;
    }
    constexpr bool operator <(const date& d) const noexcept {
        return !(*this >= d);
    }
    constexpr bool operator <=(const date& d) const noexcept {
        return !(*this > d);
    }


    constexpr date& operator +=(date_type day) {
        if (day < 0) return *this -= -day;

        day_ += day;
        while (day_ > get_month_day_(year_, month_)) {
            day_ -= get_month_day_(year_, month_);
            if (++month_ == 13) {
                month_ = 1;
                year_++;
            }
        }
        return *this;
    }

    constexpr date& operator -=(date_type day) noexcept {
        if (day < 0) return *this += -day;

        day_ -= day;
        while (day_ <= 0) {
            if (--month_ == 0) {
                month_ = 12;
                year_--;
            }
            day_ += get_month_day_(year_, month_);
        }
        return *this;
    }

    constexpr date operator +(date_type day) const noexcept {
        date ret(*this);
        ret += day;
        return ret;
    }

    constexpr date operator -(date_type day) const noexcept {
        date ret(*this);
        ret -= day;
        return ret;
    }

    constexpr date& operator ++() {
        *this += 1;
        return *this;
    }
    constexpr date operator ++(int) {
        const date ret(*this);
        *this += 1;
        return ret;
    }

    constexpr date_type operator -(const date& d) const noexcept {
        date max = *this;
        date min = d;
        int flag = 1;

        if (*this < d) {
            max = d;
            min = *this;
            flag = -1;
        }

        date_type count = 0;
        while (min != max) {
            ++min;
            count++;
        }
        return count * flag;
    }

    MSTL_NODISCARD string to_string() const noexcept {
        char buf[11];
        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
            static_cast<int>(year_),
            static_cast<int>(month_),
            static_cast<int>(day_));
        return {buf};
    }

    static MSTL_CONSTEXPR20 date from_string(const string& str) noexcept {
        if (str.size() != 10 || str[4] != '-' || str[7] != '-') {
            return date{};
        }
        try {
            const date_type year = _MSTL to_int32(str.substr(0, 4).c_str());
            const date_type month = _MSTL to_int32(str.substr(5, 2).c_str());
            const date_type day = _MSTL to_int32(str.substr(8, 2).c_str());
            return date(year, month, day);
        } catch (...) {
            return date{};
        }
    }
};


class time {
public:
    using time_type = int32_t;

private:
    time_type hours_ = 0;
    time_type minutes_ = 0;
    time_type seconds_ = 0;

public:
    constexpr explicit time(const time_type h = 0,
        const time_type m = 0, const time_type s = 0) noexcept {
        if (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60) {
            hours_ = h;
            minutes_ = m;
            seconds_ = s;
        }
    }
    ~time() = default;


    MSTL_NODISCARD constexpr time_type get_hours() const noexcept { return hours_; }
    MSTL_NODISCARD constexpr time_type get_minutes() const noexcept { return minutes_; }
    MSTL_NODISCARD constexpr time_type get_seconds() const noexcept { return seconds_; }

    MSTL_NODISCARD constexpr time_type to_seconds() const noexcept {
        return hours_ * 3600 + minutes_ * 60 + seconds_;
    }


    constexpr bool operator ==(const time& other) const noexcept {
        return hours_ == other.hours_ &&
               minutes_ == other.minutes_ &&
               seconds_ == other.seconds_;
    }
    constexpr bool operator !=(const time& other) const noexcept { return !(*this == other); }

    constexpr bool operator >(const time& other) const noexcept {
        if (hours_ > other.hours_) return true;
        if (hours_ == other.hours_) {
            if (minutes_ > other.minutes_) return true;
            if (minutes_ == other.minutes_ && seconds_ > other.seconds_) return true;
        }
        return false;
    }
    constexpr bool operator >=(const time& other) const noexcept { return *this > other || *this == other; }
    constexpr bool operator <(const time& other) const noexcept { return !(*this >= other); }
    constexpr bool operator <=(const time& other) const noexcept { return !(*this > other); }


    constexpr time& operator +=(const time_type seconds) {
        if (seconds < 0) return *this -= -seconds;

        seconds_ += seconds;
        const time_type extra_min = seconds_ / 60;
        seconds_ %= 60;

        minutes_ += extra_min;
        const time_type extra_hour = minutes_ / 60;
        minutes_ %= 60;

        hours_ += extra_hour;
        hours_ %= 24;

        return *this;
    }

    constexpr time& operator -=(const time_type seconds) noexcept {
        if (seconds < 0) return *this += -seconds;

        seconds_ -= seconds;
        while (seconds_ < 0) {
            seconds_ += 60;
            minutes_--;
        }

        while (minutes_ < 0) {
            minutes_ += 60;
            hours_--;
        }

        while (hours_ < 0) {
            hours_ += 24;
        }
        return *this;
    }

    constexpr time operator +(const time_type seconds) const noexcept {
        time ret(*this);
        ret += seconds;
        return ret;
    }

    constexpr time operator -(const time_type seconds) const noexcept {
        time ret(*this);
        ret -= seconds;
        return ret;
    }

    constexpr time& operator ++() { return *this += 1; }
    constexpr time operator ++(int) {
        const time ret(*this);
        *this += 1;
        return ret;
    }
    constexpr time& operator --() { return *this -= 1; }
    constexpr time operator --(int) {
        const time ret(*this);
        *this -= 1;
        return ret;
    }

    constexpr time_type operator -(const time& other) const noexcept {
        time_type sec_diff = (hours_ - other.hours_) * 3600;
        sec_diff += (minutes_ - other.minutes_) * 60;
        sec_diff += (seconds_ - other.seconds_);
        return sec_diff;
    }


    MSTL_NODISCARD string to_string() const noexcept {
        char buf[9];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
            static_cast<int>(hours_),
            static_cast<int>(minutes_),
            static_cast<int>(seconds_));
        return {buf};
    }

    static time from_string(const string& str) noexcept {
        if (str.size() != 8 || str[2] != ':' || str[5] != ':') {
            return time{};
        }
        try {
            const time_type h = _MSTL to_int32(str.substr(0, 2).c_str());
            const time_type m = _MSTL to_int32(str.substr(3, 2).c_str());
            const time_type s = _MSTL to_int32(str.substr(6, 2).c_str());
            return time(h, m, s);
        } catch (...) {
            return time{};
        }
    }
};


class timestamp;

class datetime {
public:
    using date_type = date::date_type;
    using time_type = time::time_type;

private:
    date date_{};
    time time_{};

public:
    constexpr explicit datetime(const date& d, const time& t) noexcept
        : date_(d), time_(t) {}

    constexpr explicit datetime(const date& d,
        const time_type h = 0, const time_type m = 0, const time_type s = 0) noexcept
        : date_(d), time_(h, m, s) {}

    constexpr explicit datetime(
        const date_type year = 1970, const date_type month = 1, const date_type day = 1,
        const time_type h = 0, const time_type m = 0, const time_type s = 0) noexcept
        : date_(year, month, day), time_(h, m, s) {}


    MSTL_NODISCARD constexpr const date& get_date() const noexcept { return date_; }
    MSTL_NODISCARD constexpr const time& get_time() const noexcept { return time_; }

    MSTL_NODISCARD constexpr time_type get_hours() const noexcept { return time_.get_hours(); }
    MSTL_NODISCARD constexpr time_type get_minutes() const noexcept { return time_.get_minutes(); }
    MSTL_NODISCARD constexpr time_type get_seconds() const noexcept { return time_.get_seconds(); }
    MSTL_NODISCARD constexpr date_type get_year() const noexcept { return date_.get_year(); }
    MSTL_NODISCARD constexpr date_type get_month() const noexcept { return date_.get_month(); }
    MSTL_NODISCARD constexpr date_type get_day() const noexcept { return date_.get_day(); }


    MSTL_NODISCARD static constexpr datetime epoch() noexcept { return datetime{}; }
    MSTL_NODISCARD static datetime now() noexcept;
    MSTL_NODISCARD static datetime from_utc(const datetime& utc_dt) noexcept;
    MSTL_NODISCARD static datetime to_utc(const datetime& local_dt) noexcept;

    MSTL_NODISCARD timestamp to_timestamp() const noexcept;


    constexpr bool operator ==(const datetime& other) const noexcept {
        return date_ == other.date_ && time_ == other.time_;
    }
    constexpr bool operator !=(const datetime& other) const noexcept { return !(*this == other); }

    constexpr bool operator >(const datetime& other) const noexcept {
        if (date_ > other.date_) return true;
        if (date_ == other.date_ && time_ > other.time_) return true;
        return false;
    }
    constexpr bool operator >=(const datetime& other) const noexcept {
        return *this > other || *this == other;
    }
    constexpr bool operator <(const datetime& other) const noexcept { return !(*this >= other); }
    constexpr bool operator <=(const datetime& other) const noexcept { return !(*this > other); }


    constexpr datetime& operator +=(const time_type seconds) {
        if (seconds < 0) return *this -= -seconds;

        const time temp_time = time_;
        const time_type max_seconds_without_day_change = 86400 -
            (temp_time.get_hours() * 3600 + temp_time.get_minutes() * 60 + temp_time.get_seconds());

        if (seconds < max_seconds_without_day_change) {
            time_ += seconds;
        }
        else {
            const time_type remaining_seconds = seconds - max_seconds_without_day_change - 1;
            date_ += 1;
            time_ = time(0, 0, 0) + (remaining_seconds + 1);
        }
        return *this;
    }

    constexpr datetime& operator -=(const time_type seconds) noexcept {
        if (seconds < 0) return *this += -seconds;

        const  time_type current_seconds =
            time_.get_hours() * 3600 + time_.get_minutes() * 60 + time_.get_seconds();

        if (seconds < current_seconds) {
            time_ -= seconds;
        }
        else {
            const time_type remaining_seconds = seconds - current_seconds - 1;
            date_ -= 1;
            time_ = time(23, 59, 59) - remaining_seconds;
        }
        return *this;
    }

    constexpr datetime operator +(const time_type seconds) const noexcept {
        datetime ret(*this);
        ret += seconds;
        return ret;
    }

    constexpr datetime operator -(const time_type seconds) const noexcept {
        datetime ret(*this);
        ret -= seconds;
        return ret;
    }

    constexpr datetime& operator ++() { return *this += 1; }
    constexpr datetime operator ++(int) {
        const datetime ret(*this);
        *this += 1;
        return ret;
    }
    constexpr datetime& operator --() { return *this -= 1; }
    constexpr datetime operator --(int) {
        const datetime ret(*this);
        *this -= 1;
        return ret;
    }

    constexpr time_type operator -(const datetime& other) const noexcept {
        const time_type day_diff = date_ - other.date_;
        time_type sec_diff = day_diff * 86400;
        sec_diff += (time_ - other.time_);
        return sec_diff;
    }


    MSTL_NODISCARD string to_string() const noexcept {
        return date_.to_string() + " " + time_.to_string();
    }

    static datetime from_string(const string& str) noexcept {
        if (str.size() != 19 || str[10] != ' ') {
            return datetime{};
        }
        const date d = date::from_string(str.substr(0, 10));
        const time t = time::from_string(str.substr(11, 8));
        return datetime(d, t);
    }
};


class timestamp {
public:
    using value_type = int64_t;

private:
    value_type sec_since_epoch_ = 0;

public:
    constexpr explicit timestamp(const value_type sec = 0) noexcept
        : sec_since_epoch_(sec) {}

    constexpr explicit timestamp(const datetime& dt) noexcept {
        sec_since_epoch_ = dt - datetime::epoch();
    }

    ~timestamp() = default;


    static timestamp now() noexcept {
        return timestamp(datetime::now());
    }


    MSTL_NODISCARD constexpr datetime to_datetime() const noexcept {
        return datetime::epoch() + sec_since_epoch_;
    }

    MSTL_NODISCARD constexpr value_type get_seconds() const noexcept {
        return sec_since_epoch_;
    }


    constexpr timestamp& operator +=(const value_type sec) noexcept {
        sec_since_epoch_ += sec;
        return *this;
    }
    constexpr timestamp& operator -=(const value_type sec) noexcept {
        sec_since_epoch_ -= sec;
        return *this;
    }

    constexpr timestamp operator +(const value_type sec) const noexcept {
        timestamp ret(*this);
        ret += sec;
        return ret;
    }

    constexpr timestamp operator -(const value_type sec) const noexcept {
        timestamp ret(*this);
        ret -= sec;
        return ret;
    }

    constexpr value_type operator -(const timestamp& other) const noexcept {
        return sec_since_epoch_ - other.sec_since_epoch_;
    }


    constexpr bool operator ==(const timestamp& other) const noexcept {
        return sec_since_epoch_ == other.sec_since_epoch_;
    }
    constexpr bool operator !=(const timestamp& other) const noexcept { return !(*this == other); }

    constexpr bool operator >(const timestamp& other) const noexcept {
        return sec_since_epoch_ > other.sec_since_epoch_;
    }
    constexpr bool operator >=(const timestamp& other) const noexcept {
        return *this > other || *this == other;
    }
    constexpr bool operator <(const timestamp& other) const noexcept { return !(*this >= other); }
    constexpr bool operator <=(const timestamp& other) const noexcept { return !(*this > other); }
};


MSTL_NODISCARD inline datetime datetime::now() noexcept {
#ifdef MSTL_PLATFORM_WINDOWS__
    SYSTEMTIME st{};
    GetLocalTime(&st);
    return datetime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#elif defined(MSTL_PLATFORM_LINUX__)
    const std::time_t now_time = std::time(nullptr);
    std::tm local_tm{};
    localtime_r(&now_time, &local_tm);
    return datetime(local_tm.tm_year + 1900, local_tm.tm_mon + 1,
        local_tm.tm_mday, local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec);
#else
    return datetime();
#endif
}

inline datetime datetime::from_utc(const datetime& utc_dt) noexcept {
#ifdef MSTL_PLATFORM_WINDOWS__
    SYSTEMTIME st_utc;
    st_utc.wYear = static_cast<WORD>(utc_dt.get_year());
    st_utc.wMonth = static_cast<WORD>(utc_dt.get_month());
    st_utc.wDay = static_cast<WORD>(utc_dt.get_day());
    st_utc.wHour = static_cast<WORD>(utc_dt.get_hours());
    st_utc.wMinute = static_cast<WORD>(utc_dt.get_minutes());
    st_utc.wSecond = static_cast<WORD>(utc_dt.get_seconds());
    st_utc.wMilliseconds = 0;

    SYSTEMTIME st;
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &st_utc, &st)) {
        return datetime::epoch();
    }
    return datetime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#elif defined(MSTL_PLATFORM_LINUX__)
    std::tm utc_tm{};
    utc_tm.tm_year = utc_dt.get_year() - 1900;
    utc_tm.tm_mon = utc_dt.get_month() - 1;
    utc_tm.tm_mday = utc_dt.get_day();
    utc_tm.tm_hour = utc_dt.get_hours();
    utc_tm.tm_min = utc_dt.get_minutes();
    utc_tm.tm_sec = utc_dt.get_seconds();
    utc_tm.tm_isdst = -1;

    const std::time_t t = ::timegm(&utc_tm);
    if (t == -1) {
        return datetime::epoch();
    }
    std::tm local_tm{};
    if (!localtime_r(&t, &local_tm)) {
        return datetime::epoch();
    }
    return datetime(local_tm.tm_year + 1900, local_tm.tm_mon + 1,
        local_tm.tm_mday, local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec);
#else
    return utc_dt;
#endif
}

inline datetime datetime::to_utc(const datetime& local_dt) noexcept {
#ifdef MSTL_PLATFORM_WINDOWS__
    SYSTEMTIME st_local;
    st_local.wYear = static_cast<WORD>(local_dt.get_year());
    st_local.wMonth = static_cast<WORD>(local_dt.get_month());
    st_local.wDay = static_cast<WORD>(local_dt.get_day());
    st_local.wHour = static_cast<WORD>(local_dt.get_hours());
    st_local.wMinute = static_cast<WORD>(local_dt.get_minutes());
    st_local.wSecond = static_cast<WORD>(local_dt.get_seconds());
    st_local.wMilliseconds = 0;

    SYSTEMTIME st;
    if (!TzSpecificLocalTimeToSystemTime(nullptr, &st_local, &st)) {
        return datetime::epoch();
    }
    return datetime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#elif defined(MSTL_PLATFORM_LINUX__)
    std::tm local_tm{};
    local_tm.tm_year = local_dt.get_year() - 1900;
    local_tm.tm_mon = local_dt.get_month() - 1;
    local_tm.tm_mday = local_dt.get_day();
    local_tm.tm_hour = local_dt.get_hours();
    local_tm.tm_min = local_dt.get_minutes();
    local_tm.tm_sec = local_dt.get_seconds();
    local_tm.tm_isdst = -1;

    const std::time_t t = std::mktime(&local_tm);
    if (t == -1) {
        return datetime::epoch();
    }
    std::tm utc_tm{};
    if (!gmtime_r(&t, &utc_tm)) {
        return datetime::epoch();
    }
    return datetime(utc_tm.tm_year + 1900, utc_tm.tm_mon + 1,
        utc_tm.tm_mday, utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec);
#else
    return *this;
#endif
}

MSTL_NODISCARD inline timestamp datetime::to_timestamp() const noexcept {
    return timestamp(*this);
}


MSTL_END_NAMESPACE__
#endif // MSTL_DATETIME_HPP__
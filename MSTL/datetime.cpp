#include "datetime.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_NODISCARD datetime datetime::now() noexcept {
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

datetime datetime::from_utc(const datetime& utc_dt) noexcept {
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

datetime datetime::to_utc(const datetime& local_dt) noexcept {
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

MSTL_NODISCARD timestamp datetime::to_timestamp() const noexcept {
    return timestamp(*this);
}

MSTL_END_NAMESPACE__

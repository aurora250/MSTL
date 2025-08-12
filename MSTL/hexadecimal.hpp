#ifndef MSTL_HEXADECIMAL_HPP__
#define MSTL_HEXADECIMAL_HPP__
#include "string.hpp"
#include "algo.hpp"
MSTL_BEGIN_NAMESPACE__

class hexadecimal {
private:
    int64_t value;

    static int64_t parse_hex(const string& str) {
        if (str.empty()) return 0;

        string s = str;
        bool negative = false;
        size_t start = 0;

        while (start < s.size() && _MSTL is_space(s[start])) ++start;
        if (start == s.size()) return 0;

        if (s[start] == '-') {
            negative = true;
            ++start;
        } else if (s[start] == '+') {
            ++start;
        }

        if (start + 1 < s.size() && s[start] == '0' &&
            (s[start+1] == 'x' || s[start+1] == 'X')) {
            start += 2;
        }

        string hex_digits;
        while (start < s.size()) {
            char c = s[start++];
            if (_MSTL is_xdigit(c)) {
                hex_digits += c;
            } else if (!_MSTL is_space(c)) {
                Exception(ValueError("Invalid hexadecimal character"));
            }
        }

        if (hex_digits.empty()) return 0;

        size_t pos = 0;
        const uint64_t raw = to_uint64(hex_digits.data(), &pos, 16);
        if (pos != hex_digits.size()) {
            Exception(ValueError("Invalid hexadecimal format"));
        }

        if (negative) {
            if (raw > static_cast<uint64_t>(INT64_MAX_SIZE) + 1) {
                Exception(ValueError("Hexadecimal value out of range"));
            }
            return -static_cast<long long>(raw);
        }
        if (raw > static_cast<uint64_t>(INT64_MAX_SIZE)) {
            Exception(ValueError("Hexadecimal value out of range"));
        }
        return static_cast<int64_t>(raw);
    }

public:
    hexadecimal() : value(0) {}
    hexadecimal(const int64_t v) : value(v) {}
    explicit hexadecimal(const string& s) : value(parse_hex(s)) {}

    long long to_decimal() const { return value; }

    hexadecimal operator +(const hexadecimal& other) const { return {value + other.value}; }
    hexadecimal operator -(const hexadecimal& other) const { return {value - other.value}; }
    hexadecimal operator *(const hexadecimal& other) const { return {value * other.value}; }
    hexadecimal operator /(const hexadecimal& other) const {
        if (other.value == 0) Exception(MathError("Division by zero"));
        return {value / other.value};
    }
    hexadecimal operator %(const hexadecimal& other) const {
        if (other.value == 0) Exception(MathError("Modulo by zero"));
        return {value % other.value};
    }

    bool operator ==(const hexadecimal& other) const { return value == other.value; }
    bool operator !=(const hexadecimal& other) const { return value != other.value; }
    bool operator <(const hexadecimal& other) const { return value < other.value; }
    bool operator <=(const hexadecimal& other) const { return value <= other.value; }
    bool operator >(const hexadecimal& other) const { return value > other.value; }
    bool operator >=(const hexadecimal& other) const { return value >= other.value; }


    string to_string() const {
        if (value == 0) {
            return "0x0";
        }

        bool is_negative = value < 0;
        uint64_t abs_value = is_negative ?
            static_cast<uint64_t>(-value) :
            static_cast<uint64_t>(value);

        static const string hex_digits = "0123456789abcdef";
        string result_digits;

        while (abs_value > 0) {
            uint64_t remainder = abs_value % 16;
            result_digits += hex_digits[remainder];
            abs_value /= 16;
        }
        _MSTL reverse(result_digits.begin(), result_digits.end());

        string output;
        if (is_negative) {
            output += "-";
        }
        output += "0x";
        output += result_digits;
        return output;
    }

    string to_std_string() const {
        if (value == 0) {
            return "0";
        }
        uint64_t num = static_cast<uint64_t>(value);
        constexpr char hex_digits[] = "0123456789abcdef";
        string result;

        while (num > 0) {
            const uint64_t remainder = num % 16;
            result += hex_digits[remainder];
            num /= 16;
        }
        _MSTL reverse(result.begin(), result.end());
        return result;
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_HEXADECIMAL_HPP__

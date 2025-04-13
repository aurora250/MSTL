#ifndef MSTL_JSON_ANALYSE_HPP__
#define MSTL_JSON_ANALYSE_HPP__
#include "basiclib.hpp"
#ifdef MSTL_VERSION_17__
#include "unordered_map.hpp"
#include "vector.hpp"
#include "variant.hpp"
#include "string.hpp"
#include "optional.hpp"
#include <charconv>
#include <regex>
MSTL_BEGIN_NAMESPACE__

struct JSONObject;

using JSONDict = unordered_map<string, JSONObject>;
using JSONList = vector<JSONObject>;

struct JSONObject {
    variant<nullptr_t, bool, int, double, string, JSONList, JSONDict> inner{};

    template <class T>
    MSTL_NODISCARD bool is() const {
        return holds_alternative<T>(inner);
    }

    template <class T>
    const T& get() const {
        return get<T>(inner);
    }
    template <class T>
    T& get() {
        return get<T>(inner);
    }
};

template <class T>
optional<T> try_parse_num(const string_view str) {
    T value;
    auto res = std::from_chars(str.data(), str.data() + str.size(), value);
    if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
        return value;
    }
    return nullopt;
}

inline char unescaped_char(const char c) {
    switch (c) {
    case 'n': return '\n';
    case 'r': return '\r';
    case '0': return '\0';
    case 't': return '\t';
    case 'v': return '\v';
    case 'f': return '\f';
    case 'b': return '\b';
    case 'a': return '\a';
    default: return c;
    }
}

inline pair<JSONObject, size_t> parse(const string_view json) {
    if (json.empty()) {
        return {JSONObject{std::nullptr_t{}}, 0};
    } else if (size_t off = json.find_first_not_of(" \n\r\t\v\f\0"); off != 0 && off != json.npos) {
        auto [obj, eaten] = parse(json.substr(off));
        return {std::move(obj), eaten + off};
    } else if ('0' <= json[0] && json[0] <= '9' || json[0] == '+' || json[0] == '-') {
        std::regex num_re{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
        std::cmatch match;
        if (std::regex_search(json.data(), json.data() + json.size(), match, num_re)) {
            string_view str = match.str().c_str();
            if (auto num = try_parse_num<int>(str)) {
                return {JSONObject{*num}, str.size()};
            }
            if (auto num = try_parse_num<double>(str)) {
                return {JSONObject{*num}, str.size()};
            }
        }
    } else if (json[0] == '"') {
        string str;
        enum {
            Raw,
            Escaped,
        } phase = Raw;
        size_t i;
        for (i = 1; i < json.size(); i++) {
            char ch = json[i];
            if (phase == Raw) {
                if (ch == '\\') {
                    phase = Escaped;
                } else if (ch == '"') {
                    i += 1;
                    break;
                } else {
                    str += ch;
                }
            } else if (phase == Escaped) {
                str += unescaped_char(ch);
                phase = Raw;
            }
        }
        return {JSONObject{move(str)}, i};
    } else if (json[0] == '[') {
        JSONList res;
        size_t i;
        for (i = 1; i < json.size();) {
            if (json[i] == ']') {
                i += 1;
                break;
            }
            auto [obj, eaten] = parse(json.substr(i));
            if (eaten == 0) {
                i = 0;
                break;
            }
            res.push_back(std::move(obj));
            i += eaten;
            if (json[i] == ',') {
                i += 1;
            }
        }
        return {JSONObject{std::move(res)}, i};
    } else if (json[0] == '{') {
        JSONDict res{100};
        size_t i;
        for (i = 1; i < json.size();) {
            if (json[i] == '}') {
                i += 1;
                break;
            }
            auto [keyobj, keyeaten] = parse(json.substr(i));
            if (keyeaten == 0) {
                i = 0;
                break;
            }
            i += keyeaten;
            if (!holds_alternative<string>(keyobj.inner)) {
                i = 0;
                break;
            }
            if (json[i] == ':') {
                i += 1;
            }
            string key = std::move(keyobj.inner.get<string>());
            auto [valobj, valeaten] = parse(json.substr(i));
            if (valeaten == 0) {
                i = 0;
                break;
            }
            i += valeaten;
            res.emplace(std::move(key), std::move(valobj));
            if (json[i] == ',') {
                i += 1;
            }
        }
        return {JSONObject{std::move(res)}, i};
    }
    return {JSONObject{nullptr_t{}}, 0};
}

MSTL_END_NAMESPACE__
#endif // MSTL_VERSION_17__
#endif // MSTL_JSON_ANALYSE_HPP__

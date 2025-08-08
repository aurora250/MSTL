#ifndef MSTL_JSON_HPP__
#define MSTL_JSON_HPP__
#include "map.hpp"
#include "string.hpp"
#include "vector.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(JsonParseError, ValueError, "Json String Parse Failed")

class json_value;
class json_null;
class json_bool;
class json_number;
class json_string;
class json_object;
class json_array;


class json_value {
public:
    enum types {
        Null,
        Bool,
        Number,
        String,
        Object,
        Array
    };

    virtual ~json_value() = default;
    virtual types type() const = 0;

    virtual const json_null* as_null() const { return nullptr; }
    virtual const json_bool* as_bool() const { return nullptr; }
    virtual const json_number* as_number() const { return nullptr; }
    virtual const json_string* as_string() const { return nullptr; }
    virtual const json_object* as_object() const { return nullptr; }
    virtual const json_array* as_array() const { return nullptr; }

    bool is_null() const { return type() == Null; }
    bool is_bool() const { return type() == Bool; }
    bool is_number() const { return type() == Number; }
    bool is_string() const { return type() == String; }
    bool is_object() const { return type() == Object; }
    bool is_array() const { return type() == Array; }
};


class json_null final : public json_value {
public:
    types type() const override { return Null; }
    const json_null* as_null() const override { return this; }
};

class json_bool final : public json_value {
private:
    bool value;
public:
    json_bool(const bool v) : value(v) {}
    types type() const override { return Bool; }
    const json_bool* as_bool() const override { return this; }
    bool get_value() const { return value; }
};

class json_number final : public json_value {
private:
    double value;
public:
    json_number(const double v) : value(v) {}
    types type() const override { return Number; }
    const json_number* as_number() const override { return this; }
    double get_value() const { return value; }
};

class json_string final : public json_value {
private:
    string value;
public:
    json_string(string v) : value(_MSTL move(v)) {}
    types type() const override { return String; }
    const json_string* as_string() const override { return this; }
    const string& get_value() const { return value; }
};

class json_object final : public json_value {
private:
    _MSTL map<string, unique_ptr<json_value>> members;
public:
    types type() const override { return Object; }
    const json_object* as_object() const override { return this; }

    void add_member(const string& key, unique_ptr<json_value> value) {
        members[key] = _MSTL move(value);
    }

    const json_value* get_member(const string& key) const {
        auto it = members.find(key);
        if (it != members.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const map<string, unique_ptr<json_value>>& get_members() const {
        return members;
    }
};

class json_array final : public json_value {
private:
    vector<unique_ptr<json_value>> elements;
public:
    types type() const override { return Array; }
    const json_array* as_array() const override { return this; }

    void add_element(unique_ptr<json_value> value) {
        elements.push_back(_MSTL move(value));
    }

    const json_value* get_element(const size_t index) const {
        if (index < elements.size()) {
            return elements[index].get();
        }
        return nullptr;
    }

    size_t size() const { return elements.size(); }

    const vector<unique_ptr<json_value>>& get_elements() const {
        return elements;
    }
};


class json_parser {
private:
    string json;
    size_t pos;
    size_t length;

    void skip_space() {
        while (pos < length && _MSTL is_space(json[pos])) {
            pos++;
        }
    }

    char current() const {
        if (pos < length) {
            return json[pos];
        }
        return '\0';
    }

    bool eof() const {
        return pos >= length;
    }

    unique_ptr<json_string> parse_string() {
        pos++;
        string result;
        bool escaped = false;

        while (pos < length) {
            const char c = json[pos++];
            if (escaped) {
                escaped = false;
                switch (c) {
                    case '"':  result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/':  result += '/'; break;
                    case 'b':  result += '\b'; break;
                    case 'f':  result += '\f'; break;
                    case 'n':  result += '\n'; break;
                    case 'r':  result += '\r'; break;
                    case 't':  result += '\t'; break;
                    default:   result += c;    break;
                }
            } else if (c == '"') {
                return make_unique<json_string>(result);
            } else if (c == '\\') {
                escaped = true;
            } else {
                result += c;
            }
        }
        Exception(JsonParseError("Unterminated string"));
        return make_unique<json_string>("");
    }

    unique_ptr<json_number> parse_number() {
        const size_t start = pos;
        if (current() == '-') {
            pos++;
        }
        if (current() == '0') {
            pos++;
            if (pos < length && json[pos] == '.') {
                pos++;
                if (pos >= length || !_MSTL is_digit(json[pos])) {
                    Exception(JsonParseError("Invalid decimal part"));
                }
                while (pos < length && _MSTL is_digit(json[pos])) {
                    pos++;
                }
            }
        } else if (_MSTL is_digit(current())) {
            while (pos < length && _MSTL is_digit(json[pos])) {
                pos++;
            }
            if (pos < length && json[pos] == '.') {
                pos++;
                if (pos >= length || !isdigit(json[pos])) {
                    Exception(JsonParseError("Invalid decimal part"));
                }
                while (pos < length && isdigit(json[pos])) {
                    pos++;
                }
            }
        } else {
            Exception(JsonParseError("Invalid number format"));
        }

        if (pos < length && (json[pos] == 'e' || json[pos] == 'E')) {
            pos++;
            if (pos < length && (json[pos] == '+' || json[pos] == '-')) {
                pos++;
            }
            if (pos >= length || !isdigit(json[pos])) {
                Exception(JsonParseError("Invalid exponent part"));
            }
            while (pos < length && isdigit(json[pos])) {
                pos++;
            }
        }

        string numStr = json.substr(start, pos - start);
        try {
            double value = _MSTL to_float64(numStr.data());
            return make_unique<json_number>(value);
        } catch (...) {
            Exception(JsonParseError("Invalid number value"));
        }
        return make_unique<json_number>(FLOAT64_MAX_POSI_SIZE);
    }

    unique_ptr<json_value> parse_keyword() {
        const size_t start = pos;
        while (pos < length && isalpha(json[pos])) {
            pos++;
        }

        string keyword = json.substr(start, pos - start);
        if (keyword == "true") {
            return make_unique<json_bool>(true);
        } else if (keyword == "false") {
            return make_unique<json_bool>(false);
        } else if (keyword == "null") {
        } else {
            Exception(JsonParseError("Invalid keyword"));
        }
        return make_unique<json_null>();
    }

    unique_ptr<json_array> parse_array() {
        pos++;
        auto array = make_unique<json_array>();

        skip_space();
        if (current() == ']') {
            pos++;
            return array;
        }

        while (true) {
            skip_space();
            auto element = parse_value();
            array->add_element(_MSTL move(element));
            skip_space();

            if (current() == ']') {
                pos++;
                break;
            } else if (current() == ',') {
                pos++;
                skip_space();
            } else {
                Exception(JsonParseError("Expected comma or closing bracket in array"));
            }
        }
        return array;
    }

    unique_ptr<json_object> parse_object() {
        pos++;
        auto object = make_unique<json_object>();

        skip_space();
        if (current() == '}') {
            pos++;
            return object;
        }

        while (true) {
            skip_space();
            if (current() != '"') {
                Exception(JsonParseError("Expected string key in object"));
            }
            auto key_obj = parse_string();
            string key = key_obj->get_value();

            skip_space();
            if (current() != ':') {
                Exception(JsonParseError("Expected colon after key in object"));
            }
            pos++;
            skip_space();

            auto value = parse_value();
            object->add_member(key, _MSTL move(value));

            skip_space();
            if (current() == '}') {
                pos++;
                break;
            } else if (current() == ',') {
                pos++;
                skip_space();
            } else {
                Exception(JsonParseError("Expected comma or closing brace in object"));
            }
        }

        return object;
    }

    unique_ptr<json_value> parse_value() {
        skip_space();
        if (eof()) {
            Exception(JsonParseError("Unexpected end of input"));
        }

        const char c = current();
        switch (c) {
            case '{':
                return parse_object();
            case '[':
                return parse_array();
            case '"':
                return parse_string();
            case '-':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return parse_number();
            case 't': case 'f': case 'n':
                return parse_keyword();
            default:
                Exception(JsonParseError("Unexpected character"));
        }
        return parse_object();
    }

public:
    explicit json_parser(string jsonStr) : json(_MSTL move(jsonStr)), pos(0), length(json.size()) {}

    unique_ptr<json_value> parse() {
        auto value = parse_value();
        skip_space();
        if (!eof()) {
            Exception(JsonParseError("Unexpected characters after JSON value"));
        }
        return value;
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_JSON_HPP__

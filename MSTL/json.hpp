#ifndef MSTL_JSON_HPP__
#define MSTL_JSON_HPP__
#include "map.hpp"
#include "string.hpp"
#include "vector.hpp"
MSTL_BEGIN_NAMESPACE__

MSTL_ERROR_BUILD_FINAL_CLASS(JsonOperateError, ValueError, "Json String Parse Failed")

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
        Exception(JsonOperateError("Unterminated string"));
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
                    Exception(JsonOperateError("Invalid decimal part"));
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
                    Exception(JsonOperateError("Invalid decimal part"));
                }
                while (pos < length && isdigit(json[pos])) {
                    pos++;
                }
            }
        } else {
            Exception(JsonOperateError("Invalid number format"));
        }

        if (pos < length && (json[pos] == 'e' || json[pos] == 'E')) {
            pos++;
            if (pos < length && (json[pos] == '+' || json[pos] == '-')) {
                pos++;
            }
            if (pos >= length || !isdigit(json[pos])) {
                Exception(JsonOperateError("Invalid exponent part"));
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
            Exception(JsonOperateError("Invalid number value"));
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
            Exception(JsonOperateError("Invalid keyword"));
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
                Exception(JsonOperateError("Expected comma or closing bracket in array"));
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
                Exception(JsonOperateError("Expected string key in object"));
            }
            auto key_obj = parse_string();
            string key = key_obj->get_value();

            skip_space();
            if (current() != ':') {
                Exception(JsonOperateError("Expected colon after key in object"));
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
                Exception(JsonOperateError("Expected comma or closing brace in object"));
            }
        }

        return object;
    }

    unique_ptr<json_value> parse_value() {
        skip_space();
        if (eof()) {
            Exception(JsonOperateError("Unexpected end of input"));
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
                Exception(JsonOperateError("Unexpected character"));
        }
        return parse_object();
    }

public:
    explicit json_parser(string jsonStr) : json(_MSTL move(jsonStr)), pos(0), length(json.size()) {}

    unique_ptr<json_value> parse() {
        auto value = parse_value();
        skip_space();
        if (!eof()) {
            Exception(JsonOperateError("Unexpected characters after JSON value"));
        }
        return value;
    }
};


class json_builder {
private:
    enum types {
        Object,
        Array
    };
    
    struct frame {
        types type = Object;
        union {
            json_object* object_ptr = nullptr;
            json_array* array_ptr;
        };

        frame() = default;
        frame(const types t, json_object* obj) : type(t), object_ptr(obj) {}
        frame(const types t, json_array* arr) : type(t), array_ptr(arr) {}

        frame(const frame&) = default;
        frame& operator =(const frame&) = default;
        frame(frame&&) = default;
        frame& operator =(frame&&) = default;
        ~frame() = default;
    };
    
    _MSTL stack<frame> contexts;
    unique_ptr<json_value> root;
    string current_key;

private:
    template<typename T>
    json_builder& value_impl(unique_ptr<T> v) {
        if (contexts.empty()) {
            if (root) {
                Exception(JsonOperateError("Multiple root values not allowed"));
            }
            root = _MSTL move(v);
        } else {
            const auto& top = contexts.top();
            if (top.type == Array) {
                top.array_ptr->add_element(_MSTL move(v));
            } else if (top.type == Object) {
                if (current_key.empty()) {
                    Exception(JsonOperateError("No key set for value in object"));
                }
                top.object_ptr->add_member(current_key, _MSTL move(v));
                current_key.clear();
            }
        }
        return *this;
    }

    template <typename Iterable, enable_if_t<is_iterable_v<Iterable>, int> = 0>
    json_builder& value_iterable_dispatch(const Iterable& t) {
        return this->value_iterable_impl(t);
    }

    template <typename Map, enable_if_t<is_maplike_v<Map>, int> = 0>
    json_builder& value_iterable_impl(const Map& map) {
        begin_object();
        for (const auto& pair : map) {
            this->key(pair.first).value(pair.second);
        }
        end_object();
        return *this;
    }

    template <typename Iterable, enable_if_t<!is_maplike_v<Iterable>, int> = 0>
    json_builder& value_iterable_impl(const Iterable& t) {
        begin_array();
        for (const auto& element : t) {
            this->value(element);
        }
        end_array();
        return *this;
    }

public:
    json_builder() = default;

    json_builder& begin_object() {
        auto new_object = make_unique<json_object>();
        json_object* obj_ptr = new_object.get();
        
        if (contexts.empty()) {
            if (root) {
                Exception(JsonOperateError("Root value already set"));
            }
            root = _MSTL move(new_object);
        } else {
            auto& current = contexts.top();
            if (current.type == Array) {
                current.array_ptr->add_element(_MSTL move(new_object));
            } else if (current.type == Object) {
                if (current_key.empty()) {
                    Exception(JsonOperateError("No key set for object value"));
                }
                current.object_ptr->add_member(current_key, _MSTL move(new_object));
                current_key.clear();
            }
        }
        
        contexts.push(frame(Object, obj_ptr));
        return *this;
    }

    json_builder& begin_array() {
        auto new_array = make_unique<json_array>();
        json_array* arr_ptr = new_array.get();
        
        if (contexts.empty()) {
            if (root) {
                Exception(JsonOperateError("Root value already set"));
            }
            root = _MSTL move(new_array);
        } else {
            auto& current = contexts.top();
            if (current.type == Array) {
                current.array_ptr->add_element(_MSTL move(new_array));
            } else if (current.type == Object) {
                if (current_key.empty()) {
                    Exception(JsonOperateError("No key set for array value"));
                }
                current.object_ptr->add_member(current_key, _MSTL move(new_array));
                current_key.clear();
            }
        }
        
        contexts.push(frame(Array, arr_ptr));
        return *this;
    }

    json_builder& end_object() {
        if (contexts.empty() || contexts.top().type != Object) {
            Exception(JsonOperateError("No object to close or context mismatch"));
        }
        if (!current_key.empty()) {
            Exception(JsonOperateError("Incomplete key-value pair in object"));
        }
        contexts.pop();
        return *this;
    }

    json_builder& end_array() {
        if (contexts.empty() || contexts.top().type != Array) {
            Exception(JsonOperateError("No array to close or context mismatch"));
        }
        contexts.pop();
        return *this;
    }

    json_builder& key(const string& k) {
        if (contexts.empty() || contexts.top().type != Object) {
            Exception(JsonOperateError("Key can only be set inside an object"));
        }
        if (!current_key.empty()) {
            Exception(JsonOperateError("Key already set without corresponding value"));
        }
        current_key = k;
        return *this;
    }

    json_builder& value(nullptr_t) {
        return value_impl(make_unique<json_null>());
    }
    json_builder& value(const string& v) {
        return value_impl(make_unique<json_string>(v));
    }
    json_builder& value(const char* v) {
        return value(string(v));
    }
    json_builder& value(const string_view& v) {
        return value(string(v));
    }
    json_builder& value(const double v) {
        return value_impl(make_unique<json_number>(v));
    }
    json_builder& value(const int v) {
        return value_impl(make_unique<json_number>(static_cast<double>(v)));
    }
    json_builder& value(const bool v) {
        return value_impl(make_unique<json_bool>(v));
    }
    json_builder& value(unique_ptr<json_value>&& v) {
        return value_impl(_MSTL move(v));
    }
    template <typename Iterable>
    json_builder& value(const Iterable& t) {
        return this->value_iterable_dispatch(t);
    }


    json_builder& value_object(_MSTL function<void(json_builder&)>&& build_func) {
        json_builder inner_builder;
        inner_builder.begin_object();
        build_func(inner_builder);
        inner_builder.end_object();
        auto obj = inner_builder.build();
        return value_impl(_MSTL move(obj));
    }

    json_builder& value_array(_MSTL function<void(json_builder&)>&& build_func) {
        json_builder inner_builder;
        inner_builder.begin_array();
        build_func(inner_builder);
        inner_builder.end_array();
        auto arr = inner_builder.build();
        return value_impl(_MSTL move(arr));
    }


    unique_ptr<json_value> build() {
        if (!contexts.empty()) {
            Exception(JsonOperateError("Incomplete JSON structure - unclosed objects or arrays"));
        }
        if (!current_key.empty()) {
            Exception(JsonOperateError("Incomplete key-value pair"));
        }
        if (!root) {
            Exception(JsonOperateError("No JSON value built"));
        }
        return _MSTL move(root);
    }
};


static string json_value_to_string(const json_value* value) {
    if (!value) {
        return "null";
    }
    switch (value->type()) {
        case json_value::Null: {
            return "null";
        }
        case json_value::Bool: {
            const json_bool* bool_val = value->as_bool();
            return bool_val->get_value() ? "true" : "false";
        }
        case json_value::Number: {
            const json_number* num_val = value->as_number();
            const double val = num_val->get_value();

            if (val == static_cast<double>(static_cast<long long>(val)) &&
                val >= static_cast<double>(INT64_MIN_SIZE) &&
                val <= static_cast<double>(INT64_MAX_SIZE)) {
                return _MSTL to_string(static_cast<long long>(val));
            }
            string result = _MSTL to_string(val);
            if (result.find('.') != string::npos) {
                while (!result.empty() && result.back() == '0') {
                    result.pop_back();
                }
                if (!result.empty() && result.back() == '.') {
                    result.pop_back();
                }
            }
            return result;
        }
        case json_value::String: {
            const json_string* str_val = value->as_string();
            return "\"" + escape_string(str_val->get_value()) + "\"";
        }
        case json_value::Array: {
            const json_array* arr_val = value->as_array();
            string result = "[";

            const auto& elements = arr_val->get_elements();
            for (size_t i = 0; i < elements.size(); ++i) {
                if (i > 0) {
                    result += ",";
                }
                result += json_value_to_string(elements[i].get());
            }

            result += "]";
            return result;
        }
        case json_value::Object: {
            const json_object* obj_val = value->as_object();
            string result = "{";

            const auto& members = obj_val->get_members();
            bool first = true;

            for (const auto& pair : members) {
                if (!first) {
                    result += ",";
                }
                first = false;

                result += "\"" + escape_string(pair.first) + "\":";
                result += json_value_to_string(pair.second.get());
            }

            result += "}";
            return result;
        }
        default: return "null";
    }
}

inline string json_to_string(const unique_ptr<json_value>& value) {
    return json_value_to_string(value.get());
}
inline string json_to_string(unique_ptr<json_value>&& value) {
    string result = json_value_to_string(value.get());
    return _MSTL move(result);
}
inline string json_to_string(const json_value* value) {
    return json_value_to_string(value);
}

MSTL_END_NAMESPACE__
#endif // MSTL_JSON_HPP__

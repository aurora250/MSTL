#ifndef MSTL_JSON_HPP__
#define MSTL_JSON_HPP__
#include "string.hpp"
#include "vector.hpp"
#include "map.hpp"
MSTL_BEGIN_NAMESPACE__

class JsonValue;
class JsonNull;
class JsonBool;
class JsonNumber;
class JsonString;
class JsonObject;
class JsonArray;

// JSON值基类
class JsonValue {
public:
    enum Type {
        Null,
        Bool,
        Number,
        String,
        Object,
        Array
    };

    virtual ~JsonValue() = default;
    virtual Type type() const = 0;

    // 类型转换方法
    virtual const JsonNull* asNull() const { return nullptr; }
    virtual const JsonBool* asBool() const { return nullptr; }
    virtual const JsonNumber* asNumber() const { return nullptr; }
    virtual const JsonString* asString() const { return nullptr; }
    virtual const JsonObject* asObject() const { return nullptr; }
    virtual const JsonArray* asArray() const { return nullptr; }

    // 方便的转换函数
    bool isNull() const { return type() == Null; }
    bool isBool() const { return type() == Bool; }
    bool isNumber() const { return type() == Number; }
    bool isString() const { return type() == String; }
    bool isObject() const { return type() == Object; }
    bool isArray() const { return type() == Array; }
};

// 具体JSON类型实现
class JsonNull : public JsonValue {
public:
    Type type() const override { return Null; }
    const JsonNull* asNull() const override { return this; }
};

class JsonBool : public JsonValue {
private:
    bool value;
public:
    JsonBool(bool v) : value(v) {}
    Type type() const override { return Bool; }
    const JsonBool* asBool() const override { return this; }
    bool getValue() const { return value; }
};

class JsonNumber : public JsonValue {
private:
    double value;
public:
    JsonNumber(double v) : value(v) {}
    Type type() const override { return Number; }
    const JsonNumber* asNumber() const override { return this; }
    double getValue() const { return value; }
    int getIntValue() const { return static_cast<int>(value); }
};

class JsonString : public JsonValue {
private:
    string value;
public:
    JsonString(string v) : value(std::move(v)) {}
    Type type() const override { return String; }
    const JsonString* asString() const override { return this; }
    const string& getValue() const { return value; }
};

class JsonObject : public JsonValue {
private:
    map<string, unique_ptr<JsonValue>> members;
public:
    Type type() const override { return Object; }
    const JsonObject* asObject() const override { return this; }

    void addMember(const string& key, unique_ptr<JsonValue> value) {
        members[key] = std::move(value);
    }

    const JsonValue* getMember(const string& key) const {
        auto it = members.find(key);
        if (it != members.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const map<string, unique_ptr<JsonValue>>& getMembers() const {
        return members;
    }
};

class JsonArray : public JsonValue {
private:
    vector<unique_ptr<JsonValue>> elements;
public:
    Type type() const override { return Array; }
    const JsonArray* asArray() const override { return this; }

    void addElement(unique_ptr<JsonValue> value) {
        elements.push_back(std::move(value));
    }

    const JsonValue* getElement(size_t index) const {
        if (index < elements.size()) {
            return elements[index].get();
        }
        return nullptr;
    }

    size_t size() const { return elements.size(); }
    const vector<unique_ptr<JsonValue>>& getElements() const {
        return elements;
    }
};

// JSON解析器
class JsonParser {
private:
    string json;
    size_t pos;
    size_t length;

    void skipWhitespace() {
        while (pos < length && std::isspace(json[pos])) {
            pos++;
        }
    }

    char currentChar() const {
        if (pos < length) {
            return json[pos];
        }
        return '\0';
    }

    char nextChar() {
        if (pos < length) {
            return json[pos++];
        }
        return '\0';
    }

    bool eof() const {
        return pos >= length;
    }

    unique_ptr<JsonString> parseString() {
        pos++; // 跳过开头的引号

        string result;
        bool escaped = false;
        char escapeChar;

        while (pos < length) {
            char c = json[pos++];

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
                    default:   result += c; break; // 未知转义字符，直接添加
                }
            } else if (c == '"') {
                // 字符串结束
                return make_unique<JsonString>(result);
            } else if (c == '\\') {
                escaped = true;
            } else {
                result += c;
            }
        }

        throw std::runtime_error("Unterminated string");
    }

    unique_ptr<JsonNumber> parseNumber() {
        size_t start = pos;

        // 负号
        if (currentChar() == '-') {
            pos++;
        }

        // 整数部分
        if (currentChar() == '0') {
            pos++;
            // 检查是否有小数部分
            if (pos < length && json[pos] == '.') {
                pos++;
                // 小数部分必须有至少一位数字
                if (pos >= length || !isdigit(json[pos])) {
                    throw std::runtime_error("Invalid decimal part");
                }
                while (pos < length && isdigit(json[pos])) {
                    pos++;
                }
            }
        } else if (isdigit(currentChar())) {
            while (pos < length && isdigit(json[pos])) {
                pos++;
            }
            // 检查是否有小数部分
            if (pos < length && json[pos] == '.') {
                pos++;
                // 小数部分必须有至少一位数字
                if (pos >= length || !isdigit(json[pos])) {
                    throw std::runtime_error("Invalid decimal part");
                }
                while (pos < length && isdigit(json[pos])) {
                    pos++;
                }
            }
        } else {
            throw std::runtime_error("Invalid number format");
        }

        // 指数部分
        if (pos < length && (json[pos] == 'e' || json[pos] == 'E')) {
            pos++;
            // 可选的符号
            if (pos < length && (json[pos] == '+' || json[pos] == '-')) {
                pos++;
            }
            // 指数必须有至少一位数字
            if (pos >= length || !isdigit(json[pos])) {
                throw std::runtime_error("Invalid exponent part");
            }
            while (pos < length && isdigit(json[pos])) {
                pos++;
            }
        }

        // 解析数字
        string numStr = json.substr(start, pos - start);
        try {
            double value = std::stod(numStr);
            return make_unique<JsonNumber>(value);
        } catch (...) {
            throw std::runtime_error("Invalid number value: " + numStr);
        }
    }

    unique_ptr<JsonValue> parseKeyword() {
        size_t start = pos;
        while (pos < length && isalpha(json[pos])) {
            pos++;
        }

        string keyword = json.substr(start, pos - start);
        if (keyword == "true") {
            return make_unique<JsonBool>(true);
        } else if (keyword == "false") {
            return make_unique<JsonBool>(false);
        } else if (keyword == "null") {
            return make_unique<JsonNull>();
        } else {
            throw std::runtime_error("Invalid keyword: " + keyword);
        }
    }

    unique_ptr<JsonArray> parseArray() {
        pos++; // 跳过 '['
        auto array = make_unique<JsonArray>();

        skipWhitespace();
        if (currentChar() == ']') {
            pos++; // 空数组
            return array;
        }

        while (true) {
            skipWhitespace();
            auto element = parseValue();
            array->addElement(std::move(element));
            skipWhitespace();

            if (currentChar() == ']') {
                pos++;
                break;
            } else if (currentChar() == ',') {
                pos++;
                skipWhitespace();
            } else {
                throw std::runtime_error("Expected comma or closing bracket in array");
            }
        }

        return array;
    }

    unique_ptr<JsonObject> parseObject() {
        pos++; // 跳过 '{'
        auto object = make_unique<JsonObject>();

        skipWhitespace();
        if (currentChar() == '}') {
            pos++; // 空对象
            return object;
        }

        while (true) {
            skipWhitespace();

            // 解析键
            if (currentChar() != '"') {
                throw std::runtime_error("Expected string key in object");
            }
            auto keyObj = parseString();
            string key = keyObj->getValue();

            skipWhitespace();
            if (currentChar() != ':') {
                throw std::runtime_error("Expected colon after key in object");
            }
            pos++; // 跳过 ':'
            skipWhitespace();

            // 解析值
            auto value = parseValue();
            object->addMember(key, std::move(value));

            skipWhitespace();
            if (currentChar() == '}') {
                pos++;
                break;
            } else if (currentChar() == ',') {
                pos++;
                skipWhitespace();
            } else {
                throw std::runtime_error("Expected comma or closing brace in object");
            }
        }

        return object;
    }

    unique_ptr<JsonValue> parseValue() {
        skipWhitespace();
        if (eof()) {
            throw std::runtime_error("Unexpected end of input");
        }

        char c = currentChar();
        switch (c) {
            case '{':
                return parseObject();
            case '[':
                return parseArray();
            case '"':
                return parseString();
            case '-':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return parseNumber();
            case 't': case 'f': case 'n':
                return parseKeyword();
            default:
                throw std::runtime_error("Unexpected character: " + string(1, c));
        }
    }

public:
    JsonParser(string jsonStr) : json(std::move(jsonStr)), pos(0), length(json.size()) {}

    unique_ptr<JsonValue> parse() {
        auto value = parseValue();
        skipWhitespace();
        if (!eof()) {
            throw std::runtime_error("Unexpected characters after JSON value");
        }
        return value;
    }
};

MSTL_END_NAMESPACE__
#endif // MSTL_JSON_HPP__

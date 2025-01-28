#ifndef MSTL_JSONLIB_H__
#define MSTL_JSONLIB_H__
#include "concepts.hpp"
#include "vector.hpp"
#include "hash_map.hpp"
#include <variant>
#include <string_view>
#include <regex>
#include <charconv>

#define MSTL_BEGIN_JSON__ namespace JSON {
#define MSTL_END_JSON__ }
#define MSTL_JSON using namespace MSTL::JSON;

MSTL_BEGIN_NAMESPACE__
MSTL_BEGIN_JSON__
MSTL_CONCEPTS__

struct JSONObject {
	typedef std::variant<
		std::monostate,
		bool, int, double,
		std::string,
		vector<JSONObject*>,
		unordered_map<std::string, JSONObject*>
	> value_type;

	value_type inner;

	template <typename T>
		requires(ConstructibleFrom<value_type, T>)
	JSONObject(T&& x) {
		inner = std::forward<T>(x);
	}
};

template <typename T>
std::optional<T> try_parse_number(std::string_view str) {
	T value;
	std::from_chars_result res = std::from_chars(str.data(), str.data() + str.size(), value);
	if (res.ec == std::errc() && res.ptr == str.data() + str.size()) return value;
	return std::nullopt;
}

JSONObject parse(std::string_view json) {
	if (json.empty()) return JSONObject(std::monostate());
	char head = json[0];
	if ('0' <= head && head <= '9' || head == '+' || head == '-') {
		std::regex rex("[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?");
		std::cmatch match;
		if (std::regex_search(json.data(), json.data() + json.size(), match, rex)) {
			std::string str = match.str();
			if (auto number = try_parse_number<int>(str); number.has_value())
				return JSONObject(number.value());
			if (auto number = try_parse_number<double>(str); number.has_value())
				return JSONObject(number.value());
		}
	}
	return JSONObject(std::monostate());
}

MSTL_END_JSON__
MSTL_END_NAMESPACE__
#endif // MSTL_JSONLIB_H__

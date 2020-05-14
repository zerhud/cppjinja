/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <json.hpp>

namespace cppjinja::east {

typedef std::string string_t;

struct value_term;
struct array_v final {
	array_v(array_v&&) noexcept =default ;
	array_v& operator = (array_v&&) noexcept =default ;
	array_v(const array_v& other);
	array_v& operator = (const array_v& other);
	array_v() noexcept =default ;
	~array_v() noexcept =default ;
	std::vector<std::unique_ptr<value_term>> items;
};
struct map_v final {
	map_v(map_v&&) noexcept =default ;
	map_v& operator = (map_v&&) noexcept =default ;
	map_v(const map_v& other);
	map_v& operator = (const map_v& other);
	map_v() noexcept =default ;
	~map_v() noexcept =default ;
	std::map<string_t, std::unique_ptr<value_term>> items;
};

using var_name = std::vector<string_t>;
using value_term_var = std::variant<
  bool, std::int64_t, double, string_t
, array_v, map_v
>;

extern struct value_term : value_term_var
{
	using value_term_var::value_term_var;
	using value_term_var::operator=;
	value_term(bool v) : value_term_var(v) {}
	value_term(double v) : value_term_var(v) {}
	value_term(string_t v) : value_term_var(v) {}
	value_term(array_v v) : value_term_var(v) {}
	value_term(map_v v) : value_term_var(v) {}
	value_term(const char* v) : value_term(string_t(v)) {}
	template<typename Int>
	explicit value_term(Int v) : value_term_var((std::int64_t)v) {}
} nothing ;

struct function_parameter {
	std::optional<string_t> name;
	std::optional<nlohmann::json> jval;
};

struct function_call {
	var_name ref;
	std::vector<function_parameter> params;
};

bool operator == (const map_v& left, const map_v& right);
bool operator == (const array_v& left, const array_v& right);
bool operator == (const function_parameter& left, const function_parameter& right);
std::ostream& operator << (std::ostream& out, const value_term& val);

} // namespace cppjinja::east

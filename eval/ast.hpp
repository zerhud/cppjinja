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

namespace cppjinja::east {

typedef std::string string_t;

struct value_term;
struct array_v { std::vector<std::unique_ptr<value_term>> items; };
struct map_v {
	std::map<std::variant<double, string_t>, std::unique_ptr<value_term>> items;
};

using var_name = std::vector<string_t>;
using value_term_var = std::variant<
  double
, string_t
, array_v
, map_v
>;

struct value_term : value_term_var
{
	using value_term_var::value_term_var;
};

struct function_parameter {
	std::optional<string_t> name;
	value_term val;
};

struct function_call {
	var_name ref;
	std::vector<function_parameter> params;
};


} // namespace cppjinja::east

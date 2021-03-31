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
#include <absd/abstract_data.hpp>

namespace cppjinja::east {

typedef std::pmr::string string_t;

using var_name = std::pmr::vector<string_t>;

struct function_parameter {
	std::optional<string_t> name;
	std::optional<absd::data> val;
	auto operator <=> (const function_parameter&) const noexcept =default ;
};

struct function_call {
	var_name ref;
	std::pmr::vector<function_parameter> params;
	auto operator <=> (const function_call&) const noexcept =default ;
};

std::ostream& operator << (std::ostream& out, const function_parameter& val);

} // namespace cppjinja::east

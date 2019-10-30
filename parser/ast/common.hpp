/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <optional>

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace cppjinja::ast {

namespace x3 = boost::spirit::x3;
using string_t = std::string;

template<typename T>
using forward_ast = boost::spirit::x3::forward_ast<T>;

enum class comparator{ no, eq, less, more, less_eq, more_eq };

using var_name = std::vector<string_t>;

struct function_call;
struct binary_op;
struct value_term;
struct function_call_parameter;

struct binary_op : x3::position_tagged
{
	forward_ast<value_term> left;
	forward_ast<value_term> right;
};

struct function_call_parameter : x3::position_tagged
{
	std::optional<string_t> name;
	forward_ast<value_term> value;
};

struct function_call : x3::position_tagged
{
	var_name ref;
	std::vector<function_call_parameter> params;
};

struct value_term : x3::variant<
	  string_t
	, var_name
	, function_call
	, binary_op
>
{
	using base_type::base_type;
	using base_type::operator=;
};

} // namespace cppjinja::ast


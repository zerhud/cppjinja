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

namespace cppjinja {

template<typename T>
using forward_ast = boost::spirit::x3::forward_ast<T>;

enum class comparator{ no, eq, less, more, less_eq, more_eq };

using var_name = std::vector<std::string>;

template<typename String> struct fnc_call;

template<typename String>
struct value_term : x3::variant<
		  String
		, var_name
		, fnc_call<String>
		, binary_op<String>
>
{
	using base_type::base_type;
	using base_type::operator=;

};

template<typename String>
struct binary_op
{
	forward_ast<value_term> left;
	forward_ast<value_term> right;
};

template<typename String>
struct fnc_call_parameter
{
	std::optional<std::string> name;
	forward_ast<value_term<String>> value;
};

template<typename String>
struct fnc_call
{
	var_name ref;
	std::vector<fnc_call_param<String>> params;
};

} // namespace cppjinja


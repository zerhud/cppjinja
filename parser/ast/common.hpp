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
#include "../common.hpp"

namespace cppjinja::ast {

namespace x3 = boost::spirit::x3;
using string_t = std::string;

template<typename T>
using forward_ast = boost::spirit::x3::forward_ast<T>;

enum class comparator{ no, in, eq, neq, less, more, less_eq, more_eq };

using var_name = std::vector<string_t>;

struct function_call;
struct binary_op;
struct value_term;
struct function_call_parameter;

struct binary_op : x3::position_tagged
{
	forward_ast<value_term> left;
	comparator op;
	forward_ast<value_term> right;
};

struct function_call_parameter : x3::position_tagged
{
	function_call_parameter() =default ;
	function_call_parameter(const value_term& v) : value(v) {}
	function_call_parameter(string_t n, const value_term& v) : name(std::move(n)), value(v) {}
	std::optional<string_t> name;
	forward_ast<value_term> value;
};

struct function_call : x3::position_tagged
{
	function_call()=default ;
	function_call(var_name f, std::vector<function_call_parameter> p) : ref(std::move(f)), params(std::move(p)) {}
	var_name ref;
	std::vector<function_call_parameter> params;
};

using value_term_var = x3::variant<
	  double
	, string_t
	, var_name
	, function_call
	, binary_op
> ;

struct value_term : value_term_var
{
	using base_type::base_type;
	using base_type::operator=;
};

} // namespace cppjinja::ast


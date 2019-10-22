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
#include <variant>
#include <optional>
#include <boost/variant/recursive_wrapper.hpp>
#include "config.hpp"
#include "helpers.hpp"


namespace cppjinja {

using var_name = std::vector<std::string>;

enum class comparator{ no, eq, less, more, less_eq, more_eq };
enum class block_ref { no, name, op_if };

template<typename String> struct fnc_call;
template<typename String> struct b_block;

template<typename String>
using value_term = std::variant<String, var_name, boost::recursive_wrapper<fnc_call<String>>> ;

template<typename String>
struct fnc_call {
	var_name ref;
	std::vector<value_term<String>> params;
};

struct st_raw {};

template<typename String>
struct st_out {
	std::variant<String,var_name,fnc_call<String>> ref;
	std::vector<std::variant<var_name,fnc_call<String>>> params;
};

template<typename String>
struct st_for {
	std::vector<std::string> params;
	value_term<String> ref;
};

template<typename String>
struct st_if {
	comparator op=comparator::no;
	value_term<String> left, right;
};

template<typename String>
struct macro_parameter {
	macro_parameter()=default;
	macro_parameter(std::string n) : name(std::move(n)) {}
	macro_parameter(std::string n, value_term<String> v)
		: name(std::move(n)), value(std::move(v)) {}

	std::string name;
	std::optional<value_term<String>> value;
};

template<typename String>
struct st_macro {
	std::string name;
	std::vector<macro_parameter<String>> params;
};

template<typename String>
struct set_op {
	std::vector<std::string> names;
	value_term<String> value;
};

template<typename String>
struct extends_op {
	String name;
};

struct st_call {

};

template<typename String>
struct st_comment {
	String cnt;
};

struct jtmpl {
	std::string name;
	std::optional<std::string> extends;
};

template<typename String>
using block_content = std::variant<
      String
    , st_out<String>
    , st_comment<String>
    , set_op<String>
    , extends_op<String>
    , boost::recursive_wrapper<b_block<String>>
>;

template<typename String>
struct b_block {
	std::optional<std::variant<std::string,st_raw,jtmpl,st_if<String>,st_for<String>,st_macro<String>>> ref;
	std::vector<block_content<String>> cnt;
};

using s_block = b_block<std::string>;
using w_block = b_block<std::wstring>;

} // namespace cppjinja


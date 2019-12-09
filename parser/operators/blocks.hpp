/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/blocks.hpp"

#include "common.hpp"
#include "single.hpp"
#include "opterm.hpp"

DEFINE_OPERATORS(cppjinja::ast::block_content, left.var, right.var)
DEFINE_OPERATORS(cppjinja::ast::block_raw,     left.value, right.value,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::block_if,      left.condition, right.condition, left.content, right.content,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::block_for,
                 left.vars, right.vars, left.value.var, right.value.var, left.content, right.content, left.recursive, right.recursive,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::macro_parameter, left.name, right.name, left.value, right.value)
DEFINE_OPERATORS(cppjinja::ast::block_macro, left.name, right.name, left.params, right.params, left.content, right.content,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::block_named, left.name, right.name, left.params, right.params, left.content, right.content,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::block_filtered, left.name, right.name, left.params, right.params, left.content, right.content,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::block_set, left.name, right.name, left.filters, right.filters, left.content, right.content,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)
DEFINE_OPERATORS(cppjinja::ast::block_call, left.name, right.name, left.call_name, right.call_name, left.content, right.content,
                 left.params, right.params, left.call_values, right.call_values,
                 left.left_open, right.left_open, left.left_close, right.left_close,
                 left.right_open, right.right_open, left.right_close, right.right_close)


namespace cppjinja::ast {

template<typename ... Args>
std::vector<block_content> make_content(Args&&... args)
{
	return std::vector{ block_content{std::forward<Args>(args)}... };
}

inline op_comment make_comment(std::string v) { op_comment ret; ret.value=v; return ret; }

inline op_out make_out(var_name v) { op_out ret; ret.value=v; return ret; }

inline block_for make_for(std::vector<std::string> vars, boost::spirit::x3::variant<cppjinja::ast::var_name, cppjinja::ast::function_call> val, bool recurs=false)
{
	block_for ret;
	ret.vars = std::move(vars);
	ret.value = std::move(val);
	ret.recursive = recurs;
	return ret;
}

inline block_macro make_macro(std::string n, std::vector<macro_parameter> p)
{
	block_macro ret;
	ret.name = std::move(n);
	ret.params = std::move(p);
	return ret;
}

} // namespace cppjinja::ast

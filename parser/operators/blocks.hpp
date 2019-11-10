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
DEFINE_OPERATORS(cppjinja::ast::block_for, left.vars, right.vars, left.value, right.value, left.content, right.content,
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

inline block_for make_for(std::vector<std::string> vars, boost::spirit::x3::variant<cppjinja::ast::var_name, cppjinja::ast::function_call> val)
{
	block_for ret;
	ret.vars = std::move(vars);
	ret.value = std::move(val);
	return ret;
}

} // namespace cppjinja::ast

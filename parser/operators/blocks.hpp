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


namespace cppjinja::ast {

template<typename ... Args>
std::vector<block_content> make_content(Args&&... args)
{
	return std::vector{ block_content{std::forward<Args>(args)}... };
}

op_comment make_comment(std::string v) { op_comment ret; ret.value=v; return ret; }

op_out make_out(var_name v) { op_out ret; ret.value=v; return ret; }

} // namespace cppjinja::ast

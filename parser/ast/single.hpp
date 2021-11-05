/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "opterm.hpp"
#include "expr.hpp"

namespace cppjinja::ast {

struct op_out : x3::position_tagged
{
	expr_ops::expr value;
	op_term_start open;
	op_term_end close;
};

struct op_comment : x3::position_tagged
{
	string_t value;
	comment_term_start open;
	comment_term_end close;
};

struct op_set : x3::position_tagged
{
	expr_ops::eq_assign value;
	op_term_start open;
	op_term_end close;
};

struct op_include : x3::position_tagged
{
	string_t filename;
	std::optional<bool> with_context;
	std::optional<bool> ignore_missing;
	op_term_start open;
	op_term_end close;
};

struct op_import : x3::position_tagged
{
	string_t filename;
	expr_ops::single_var_name as;
	expr_ops::single_var_name tmpl_name;
	op_term_start open;
	op_term_end close;
};

} // namespace cppjinja::ast


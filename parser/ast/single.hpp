/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "common.hpp"
#include "opterm.hpp"
#include "expr.hpp"

namespace cppjinja::ast {

struct filter_call : x3::variant<var_name, function_call>
{
	using base_type::base_type;
	using base_type::operator=;
};

struct op_out : x3::position_tagged
{
	value_term value;
	std::vector<filter_call> filters;
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
	std::vector<string_t> mods;
	std::optional<bool> with_context;
};

} // namespace cppjinja::ast


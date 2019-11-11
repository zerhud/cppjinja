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
	std::string value;
	comment_term_start open;
	comment_term_end close;
};

struct op_set : x3::position_tagged
{
	var_name name;
	value_term value;
	op_term_start open;
	op_term_end close;
};

} // namespace cppjinja::ast


/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "blocks.hpp"

namespace cppjinja::ast {

struct extend_st : x3::position_tagged
{
	string_t file_name;
	std::optional<string_t> tmpl_name;
	block_term_start open;
	block_term_end close;
};

struct tmpl : x3::position_tagged
{
	string_t name;
	std::vector<block_content> content;
	std::vector<extend_st> extends;

	block_term_start left_open, left_close;
	std::optional<block_term_end> right_open, right_close;
};

struct file : x3::position_tagged
{
	string_t name;
	std::vector<tmpl> tmpls;
};

} // namespace cppjinja::ast

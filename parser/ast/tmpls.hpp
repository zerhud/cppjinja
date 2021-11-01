/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "blocks.hpp"
#include "single.hpp"

namespace cppjinja::ast {

struct extend_st : x3::position_tagged
{
	string_t file_name;
	std::optional<string_t> tmpl_name;
};

struct tmpl : x3::position_tagged
{
	string_t name;
	std::vector<extend_st> extends;
	std::vector<block_content> content;

	string_t file_name;
	std::vector<op_import> file_imports;
};

struct file : x3::position_tagged
{
	string_t name;
	std::vector<tmpl> tmpls;
	std::vector<op_include> includes;
	std::vector<op_import> imports;
};

} // namespace cppjinja::ast

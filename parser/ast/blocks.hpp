/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "common.hpp"
#include "single.hpp"

namespace cppjinja::ast {

struct block_if;
struct block_raw;
struct block_for;
struct block_macro;
struct block_named;
struct block_filtered;

using block_content = x3::variant
<
	  string_t
	, op_out
	, op_comment
	, op_set
	, forward_ast<block_raw>
	, forward_ast<block_if>
	, forward_ast<block_for>
	, forward_ast<block_macro>
	, forward_ast<block_named>
	, forward_ast<block_filtered>
>;

struct block : x3::position_tagged
{
	block_term_start left_open, right_open;
	block_term_end left_close, right_close;
};

struct block_raw : block
{
	string_t value;
};

struct block_if : block
{
	binary_op condition;
	std::vector<block_content> content;
};

struct block_for : block
{
	typedef x3::variant<var_name, function_call> value_t;
	std::vector<string_t> vars;
	value_t value;
	std::vector<block_content> content;
};

struct macro_parameter
{
	string_t name;
	std::optional<value_term> value;
};

struct block_with_name : block
{
	string_t name;
	std::vector<macro_parameter> params;
	std::vector<block_content> content;
};

struct block_macro : block_with_name { };
struct block_named : block_with_name { };
struct block_filtered : block_with_name { };

} // namespace cppjinja::ast

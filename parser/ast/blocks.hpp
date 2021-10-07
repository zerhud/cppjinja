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
struct block_set;
struct block_macro;
struct block_named;
struct block_filtered;

// feature requests
struct block_with;
struct block_translate;
struct block_call;

//TODO: rename block_content: now bock_* used for blocks
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
	, forward_ast<block_set>
	, forward_ast<block_call>
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

struct else_thread
{
	block_term_start left_open;
	block_term_end left_close;
	std::vector<block_content> content;
};

struct block_elif : block
{
	expr_ops::expr_bool condition;
	std::vector<block_content> content;
};

struct block_if : block
{
	expr_ops::expr_bool condition;
	std::vector<block_content> content;
	std::vector<block_elif> elifs;
	std::optional<else_thread> else_block;
};

struct block_for : block
{
	typedef x3::variant<var_name, function_call> value_t;

	expr_ops::expr value;
	std::optional<expr_ops::expr_bool> condition;
	bool recursive=false;
	std::vector<string_t> vars;
	std::vector<block_content> content;
	std::optional<else_thread> else_block;
};

struct macro_parameter
{
	string_t name;
	std::optional<expr_ops::expr> value;
};

struct block_with_name : block
{
	string_t name;
	std::vector<macro_parameter> params;
	std::vector<block_content> content;
};

struct block_macro : block_with_name { };
struct block_named : block_with_name { };

struct block_filtered : block
{
	expr_ops::filter_call flt;
	std::vector<block_content> content;
};

struct block_call : block_with_name
{
	std::vector<macro_parameter> call_params;
};

struct block_set : block
{
	string_t name;
	std::vector<expr_ops::filter_call> filters;
	std::vector<block_content> content;
};

} // namespace cppjinja::ast

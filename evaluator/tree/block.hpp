/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/ast/blocks.hpp"

namespace cppjinja {

class evaluate_tree;
class data_provider;

namespace eval_tree {

class block
{
public:
	using all_blocks = std::variant
	< ast::block_if
	, ast::block_for
	, ast::block_raw
	, ast::block_set
	, ast::block_call
//	, ast::block_with
	, ast::block_macro
	, ast::block_named
	, ast::block_filtered
	>;

	block(all_blocks cur, const evaluate_tree* et);

	std::optional<ast::string_t> name() const ;
	ast::string_t render(const std::vector<ast::macro_parameter>& param, const data_provider& datas) const ;

	ast::string_t variable(ast::string_t name) const ;
	bool has_variable(ast::string_t name) const ;
private:
	ast::string_t render(const ast::value_term& val, const std::vector<ast::filter_call>& filters) const ;
	ast::string_t render(const ast::var_name& var) const ;
	ast::string_t render(const ast::function_call& var) const ;
	ast::string_t render(const ast::binary_op& var) const ;
	ast::string_t render(const std::string& base, const ast::filter_call& filter) const ;

	all_blocks cur_block_;
	const evaluate_tree* evaluator_;
};

} // namespace eval_tree
} // namespace cppjinja

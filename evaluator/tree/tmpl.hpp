/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/ast/tmpls.hpp"

#include "block.hpp"

namespace cppjinja {

class evaluate_tree;
class data_provider;

namespace eval_tree {

class tmpl
{
	ast::tmpl cur_tmpl_;
	const evaluate_tree* evaluator_;
	std::vector<eval_tree::block> blocks_;
	mutable const data_provider* cur_data_;

	std::vector<eval_tree::tmpl*> children_tmpls_;

	void extract_blocks(std::vector<ast::block_content>& cnt);
public:
	tmpl(ast::tmpl cur, const evaluate_tree* et);
	std::string name() const ;
	ast::string_t render(const data_provider& data) const ;
	bool has_block(ast::string_t name) const ;
	ast::string_t render(const ast::function_call& fnc) const ;


	void add_child(tmpl* t);
	void add_child(block b);

	std::vector<tmpl*> child_tmpls() ;
	std::vector<const tmpl*> child_tmpls() const ;
	std::vector<const block*> child_blocks() const ;
};

} // namespace eval_tree
} // namespace cppjinja

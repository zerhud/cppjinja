/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>

#include "eval/eval.hpp"
#include "parser/ast/tmpls.hpp"

#include "nodes/tmpl.hpp"
#include "nodes/block_named.hpp"
#include "nodes/block_macro.hpp"
#include "nodes/content.hpp"
#include "nodes/op_out.hpp"
#include "nodes/op_set.hpp"
#include "nodes/block_if.hpp"
#include "nodes/content_block.hpp"

namespace cppjinja {

class evtree : public evaluator {
	std::vector<std::unique_ptr<evt::node>> nodes;

	void tbuild_blocks(evt::node* p, ast::tmpl& t);
	const evt::node* search_child(
	          const ast::string_t& name
	        , const evt::node* par
	        ) const ;
	bool is_tmpl(const evt::node& n) const ;
public:
	/// debug function
	const evtnodes::tmpl* search_tmpl(const ast::var_name& name) const ;
	const evt::node* search(
	          const ast::var_name& name
	        , const evt::node* ctx=nullptr
	        ) const ;
	std::vector<const evt::node*> all_tree() const ;
	std::vector<const evt::node*> children(
	          const evt::node* selected
	        , bool add_subs=false) const ;

	/// debug function
	void print_subtree(const evt::node* par=nullptr, std::string prefix="") const ;

	evtree& add_tmpl(ast::tmpl& tmpl) override ;
	void render(
	          std::ostream& to
	        , const data_provider& data
	        , const ast::string_t& name
	        ) const override ;
};

} // namespace cppjinja

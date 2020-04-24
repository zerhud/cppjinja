/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>

#include "tmpl_compiler.hpp"

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
	std::vector<evt::compiled_tmpl> templates;

	const evt::compiled_tmpl& tmpl_by_node(const evt::node* n) const ;
public:
	/// debug function
	const evtnodes::tmpl* search_tmpl(const ast::string_t& name) const ;
	std::vector<const evt::node*> children(
	          const evt::node* selected
	        , bool add_subs=false) const ;
	std::vector<const evtnodes::callable*> roots(const evtnodes::tmpl* tmpl) const ;

	/// debug function
	std::string print_subtree(const evt::node* par=nullptr) const ;

	evtree& add_tmpl(ast::tmpl& tmpl) override ;
	void render(
	          std::ostream& to
	        , const data_provider& data
	        , const ast::string_t& name
	        ) const override ;
};

} // namespace cppjinja

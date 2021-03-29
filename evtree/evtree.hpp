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
	std::pmr::vector<evt::compiled_tmpl> templates;

	const evt::compiled_tmpl& tmpl_by_node(const evt::node* n) const ;
public:
	/// debug function
	const evtnodes::tmpl* search_tmpl(const ast::string_t& name) const ;
	std::pmr::vector<const evt::node*> children(const evt::node* selected) const ;
	std::pmr::vector<const evtnodes::callable*> roots(const evtnodes::tmpl* tmpl) const ;

	evtree& add_tmpl(ast::tmpl& tmpl) override ;
	void render(
	          std::ostream& to
	        , const data_provider& data
	        , const ast::string_t& name
	        ) const override ;
};

} // namespace cppjinja

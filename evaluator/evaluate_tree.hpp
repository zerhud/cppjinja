/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <ostream>

#include "eval.hpp"
#include "tree/tmpl.hpp"

namespace cppjinja {

class evaluate_tree
{
	std::vector<eval_tree::tmpl> collection;
	std::vector<eval_tree::tmpl> roots;

	bool insert_to_tree(ast::tmpl& t);
	void insert_content(eval_tree::tmpl& dest, ast::tmpl& src);
	eval_tree::tmpl* find_tmpl_in_tree(
	          const std::vector<eval_tree::tmpl*>& tree
	        , const std::string& name
	        ) const ;
	std::vector<eval_tree::tmpl*> roots_ptrs() ;
public:
	evaluate_tree(std::vector<ast::tmpl> tmpls);

	void render(
	          std::ostream& to
	        , const data_provider& data
	        , const ast::string_t& name
	        ) const ;

	std::vector<const eval_tree::tmpl*> all_tmpls() const ;
	const eval_tree::block* find_block(
	          ast::var_name name
	        , const eval_tree::block* cur
	        ) const ;
};

} // namespace cppjinja

/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "eval/eval.hpp"
#include "parser/ast/tmpls.hpp"

#include "context.hpp"

namespace cppjinja {

class evtree;

class node {
	std::vector<cppjinja::node*> parents_;
public:
	struct render_info
	{
		bool trim_left;
		bool trim_right;
	};

	virtual ~node() noexcept =default ;

	virtual render_info rinfo() const =0 ;

	virtual ast::string_t name() const =0 ;
	virtual bool is_leaf() const =0 ;
	virtual void render(
	          std::ostream& out
	        , const evtree& tree
	        , evt::context& ctx
	        ) const =0 ;

	void add_parent(node* np) ;
	std::vector<node*> parents() ;
	std::vector<const node*> parents() const ;
	bool is_parent(const node* n) const ;
};

} // namespace cppjinja

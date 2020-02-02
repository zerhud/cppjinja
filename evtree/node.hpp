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

namespace evt {

class node {
	std::vector<node*> parents_;
public:
	struct render_info
	{
		struct iinfo {
			bool trim_left;
			bool trim_right;
		} sibling, content;
	};

	virtual ~node() noexcept =default ;

	virtual render_info rinfo() const =0 ;

	virtual ast::string_t name() const =0 ;
	virtual bool is_leaf() const =0 ;
	virtual void render( evt::context& ctx ) const =0 ;

	void add_parent(node* np) ;
	std::vector<node*> parents() ;
	std::vector<const node*> parents() const ;
	bool is_parent(const node* n) const ;
protected:
	void render_value(
	          evt::context& ctx
	        , const ast::value_term& value
	        ) const ;
};

}} // namespace cppjinja::evt
/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "eval/eval.hpp"
#include "parser/ast/tmpls.hpp"

#include "exenv.hpp"

namespace cppjinja {
namespace evt {

class node {
	std::vector<node*> parents_;
public:

	virtual ~node() noexcept =default ;

	virtual render_info rinfo() const =0 ;

	virtual ast::string_t name() const =0 ;
	virtual bool is_leaf() const =0 ;
	virtual void render( evt::exenv& ctx ) const =0 ;

	void add_parent(node* np) ;
	std::vector<node*> parents(bool all=false) ;
	std::vector<const node*> parents(bool all=false) const ;
	bool is_parent(const node* n) const ;
protected:
	void render_children(
	    const std::vector<const node*>& children,
	    exenv& ctx, render_info default_ri) const ;
	void render_value(
	          evt::exenv& ctx
	        , const ast::value_term& value
	        ) const ;
};

}} // namespace cppjinja::evt

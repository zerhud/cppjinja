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
public:

	virtual ~node() noexcept =default ;

	virtual render_info rinfo() const =0 ;
	virtual void render( evt::exenv& ctx ) const =0 ;
protected:
	void render_children(
	    const std::vector<const node*>& children,
	    exenv& ctx, render_info default_ri) const ;
};

class named_node : virtual public node {
public:
	virtual ast::string_t name() const =0 ;
};

}} // namespace cppjinja::evt

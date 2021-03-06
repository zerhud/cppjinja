/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "evtree/declarations.hpp"

namespace cppjinja::evt {

class context {
public:
	virtual ~context() noexcept =default ;

	virtual void current_node(const node* n) =0 ;
	virtual const node* nth_node_on_stack(std::size_t ind) const =0 ;

	virtual void pop(const node* n)=0 ;
	virtual void push(const node* n)=0 ;
	virtual void push_shadow(const node* n)=0 ;
	virtual const node* maker() const =0 ;
	virtual const evtnodes::tmpl* current_tmpl() const =0 ;

	virtual std::ostream& out() =0 ;
	virtual std::pmr::string result() const =0 ;

	virtual context_object& cur_namespace() =0 ;
};

} // namespace cppjinja::evt

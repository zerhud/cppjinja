/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "exenv/raii.hpp"
#include "eval/eval.hpp"

namespace cppjinja::evt {

struct render_info
{
	bool trim_left:4;
	bool trim_right:4;
};

class exenv {
public:
	virtual ~exenv() noexcept =default ;

	virtual const evtree& tmpl() const =0 ;
	virtual std::vector<const node*> children(
		const node* selected ) const =0 ;

	virtual const data_provider* data() const =0 ;
	virtual std::ostream& out() =0 ;

	virtual context& ctx() =0 ;
	virtual const context& ctx() const =0 ;

	virtual void current_node(const node* n) =0 ;

	virtual callstack& calls() =0 ;
	virtual const callstack& calls() const =0 ;

	virtual std::optional<render_info> rinfo() const =0 ;
	virtual void rinfo(std::optional<render_info> ri) =0 ;
};

} // namespace cppjinja::evt

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
#include "exenv/result_formatter.hpp"
#include "exenv/context_objects/queue.hpp"

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
	virtual std::vector<const evtnodes::callable*> roots(
	        const evtnodes::tmpl* tmpl) const =0 ;

	virtual std::shared_ptr<std::pmr::memory_resource> storage() const =0;

	virtual const data_provider* data() const =0 ;
	virtual std::ostream& out() =0 ;
	virtual east::string_t result() const =0 ;

	virtual context& ctx() =0 ;
	virtual const context& ctx() const =0 ;
	virtual void current_node(const node* n) =0 ;

	virtual context_object& locals() =0 ;
	virtual context_object& globals() =0 ;
	virtual const context_objects::queue params() const =0 ;
	virtual const context_object& user_data() const =0 ;
	virtual const context_objects::queue all_ctx() const =0 ;

	virtual callstack& calls() =0 ;

	virtual result_formatter& render_format() =0 ;
	virtual std::optional<render_info> rinfo() const =0 ;
	virtual void rinfo(std::optional<render_info> ri) =0 ;
};

bool operator == (const render_info& left, const render_info& right);
std::ostream& operator << (std::ostream& out, const render_info& ri);

} // namespace cppjinja::evt

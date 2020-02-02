/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "op_out.hpp"

using namespace std::literals;

cppjinja::evtnodes::op_out::op_out(cppjinja::ast::op_out o)
    : op(std::move(o))
{
}

cppjinja::evt::node::render_info cppjinja::evtnodes::op_out::rinfo() const
{
	return render_info{
		{op.open.trim, op.close.trim},
		{op.open.trim, op.close.trim}
	};
}

cppjinja::ast::string_t cppjinja::evtnodes::op_out::name() const
{
	return "op_out"s;
}

bool cppjinja::evtnodes::op_out::is_leaf() const
{
	return true;
}

void cppjinja::evtnodes::op_out::render(cppjinja::evt::context& ctx) const
{
	ctx.current_node(this);

	if(op.filters.empty()) render_value(ctx, op.value);
	else {
		struct {
			evt::context& ctx;
			void operator()(const ast::var_name& var)
			{
				ctx.render_filter(var);
			}
			void operator()(const ast::function_call& fnc)
			{
				ctx.call_params(fnc.params);
				ctx.render_filter(fnc.ref);
			}

		} caller { ctx };


		ctx.push_callstack(this, true);
		render_value(ctx, op.value);
		for(auto& filter:op.filters)
			boost::apply_visitor(caller, filter.var);
		ctx.pop_callstack(this);
	}
}

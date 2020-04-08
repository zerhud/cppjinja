/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <string>
#include "op_out.hpp"
#include "evtree/exenv/expr_solver.hpp"
#include "evtree/exenv/expr_filter.hpp"

using namespace std::literals;

cppjinja::evtnodes::op_out::op_out(cppjinja::ast::op_out o)
    : op(std::move(o))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::op_out::rinfo() const
{
	return {op.open.trim, op.close.trim};
}

cppjinja::ast::string_t cppjinja::evtnodes::op_out::name() const
{
	return "op_out ("s + std::to_string((std::size_t)this) + ')';
}

bool cppjinja::evtnodes::op_out::is_leaf() const
{
	return true;
}

void cppjinja::evtnodes::op_out::render(evt::exenv& ctx) const
{
	using evt::expr_filter;
	ctx.ctx().current_node(this);

	if(op.filters.empty()) render_value(ctx, op.value);
	else {
		struct {
			evt::exenv& ctx;
			east::value_term base;
			void operator()(const ast::var_name& var)
			{
				base = expr_filter(&ctx, base)(var);
			}
			void operator()(const ast::function_call& fnc)
			{
				ctx.calls().call_params(fnc.params);
				base = expr_filter(&ctx, base)(fnc);
			}

		} caller { ctx, evt::expr_solver(&ctx)(op.value) };


		for(auto& filter:op.filters)
			boost::apply_visitor(caller, filter.var);

		render_value(ctx, caller.base);
	}
}

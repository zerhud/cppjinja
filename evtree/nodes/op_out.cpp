/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <string>
#include "op_out.hpp"

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

void cppjinja::evtnodes::op_out::render(cppjinja::evt::context& ctx) const
{
	ctx.current_node(this);

	if(op.filters.empty()) render_value(ctx, op.value);
	else {
		struct {
			evt::context& ctx;
			east::value_term base;
			void operator()(const ast::var_name& var)
			{
				base = ctx.filter(base, ast::value_term{var});
			}
			void operator()(const ast::function_call& fnc)
			{
				ctx.call_params(fnc.params);
				base = ctx.filter(base, ast::value_term{fnc});
			}

		} caller { ctx, ctx.concreate_value(op.value) };


		for(auto& filter:op.filters)
			boost::apply_visitor(caller, filter.var);

		render_value(ctx, caller.base);
	}
}

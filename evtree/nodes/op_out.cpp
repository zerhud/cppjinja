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
#include "evtree/exenv/callstack.hpp"

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

void cppjinja::evtnodes::op_out::render(evt::exenv& env) const
{
	env.current_node(this);
	evt::expr_filter flt(&env, evt::expr_solver(&env)(op.value));
	for(auto& f:op.filters) flt(f);
	env.out() << flt;
}

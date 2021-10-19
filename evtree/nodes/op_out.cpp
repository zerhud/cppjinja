/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <string>
#include "op_out.hpp"
#include "evtree/exenv/expr_eval.hpp"

using namespace std::literals;

cppjinja::evtnodes::op_out::op_out(cppjinja::ast::op_out o)
    : op(std::move(o))
{
}

cppjinja::evt::render_info cppjinja::evtnodes::op_out::rinfo() const
{
	return {op.open.trim, op.close.trim};
}

void cppjinja::evtnodes::op_out::render(evt::exenv& env) const
{
	env.current_node(this);
	env.out() << evt::expr_eval(&env)(op.value);
}

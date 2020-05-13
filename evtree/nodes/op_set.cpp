/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "op_set.hpp"
#include "exenv.hpp"
#include "evtree/exenv/expr_eval.hpp"
#include "evtree/exenv/expr_solver.hpp"
#include "evtree/exenv/context_objects/value.hpp"

using namespace cppjinja::evtnodes;
using namespace std::literals;

op_set::op_set(cppjinja::ast::op_set o) : op(std::move(o))
{
}

cppjinja::evt::render_info op_set::rinfo() const
{
	return {op.open.trim, op.close.trim};
}

void op_set::render(evt::exenv& env) const
{
	env.current_node(this);
	const ast::expr_ops::single_var_name& name =
	        boost::get<ast::expr_ops::single_var_name>(op.value.names.at(0));
	env.locals().add(name.name, evt::expr_eval(&env)(op.value.value.get()));
}

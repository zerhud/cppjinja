/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "op_set.hpp"
#include "exenv.hpp"
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
	if(op.value.var.type() == typeid(ast::var_name))
		inject_object(env);
	else inject_value(env);
}

void op_set::inject_value(cppjinja::evt::exenv& env) const
{
	auto obj = std::make_unique<evt::context_objects::value>(
	            evt::expr_solver(&env)(op.value));
	env.locals().add(op.name, std::move(obj));
}

void op_set::inject_object(cppjinja::evt::exenv& env) const
{
	ast::var_name name = boost::get<ast::var_name>(op.value.var);
	assert(!name.empty());
	auto obj = env.all_ctx().find(evt::expr_solver(&env).reduce(name));
	env.locals().add(op.name, std::move(obj));
}

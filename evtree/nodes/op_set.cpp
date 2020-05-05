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
	else if(op.value.var.type() == typeid(ast::function_call))
		inject_call_object(env);
	else if(op.value.var.type() == typeid(ast::array_calls))
		inject_array_calls(env);
	else inject_value(env);
}

void op_set::add_var_name(cppjinja::evt::exenv& env, cppjinja::ast::var_name name) const
{
	auto obj = env.all_ctx().find(name);
	env.locals().add(op.name, obj);
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
	add_var_name(env, evt::expr_solver(&env).reduce(name));
}

void op_set::inject_call_object(cppjinja::evt::exenv& env) const
{
	auto call = boost::get<ast::function_call>(op.value.var);
	auto obj = env.all_ctx().find(evt::expr_solver(&env).reduce(call.ref));
	env.locals().add(op.name, obj->call(evt::expr_solver(&env).reduce(call.params)));
}

void op_set::inject_array_calls(cppjinja::evt::exenv& env) const
{
	auto name = boost::get<ast::array_calls>(op.value.var);
	assert(!name.empty());
	add_var_name(env, evt::expr_solver(&env).reduce(name));
}
